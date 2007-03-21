/*
    Copyright (C) 2006 by Michal Schulz
    $Id$

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Library General Public License as 
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this program; if not, write to the
    Free Software Foundation, Inc.,
    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#define DEBUG 1

#include <aros/debug.h>
#include <aros/libcall.h>

#include <exec/types.h>
#include <oop/oop.h>
#include <dos/dos.h>
#include <dos/dosextens.h>

#include <hidd/hidd.h>
#include <usb/usb.h>

#include <proto/oop.h>
#include <proto/dos.h>

#include <stdint.h>

#include "usb.h"
#include "misc.h"

OOP_Object *METHOD(USB, Root, New)
{
    D(bug("[USB] USB::New()\n"));

    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg) msg);
    if (o)
    {
        SD(cl)->usb = o;
    }    

    D(bug("[USB] USB::New() = %p\n", o));

    return o;
}

struct pRoot_Dispose {
    OOP_MethodID        mID;
};

void METHOD(USB, Root, Dispose)
{
    D(bug("[USB] USB::Dispose\n"));

    OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
}

BOOL METHOD(USB, Hidd_USB, AttachDriver)
{
    BOOL retval = FALSE;
    D(bug("[USB] USB::AttachDriver(%p)\n", msg->driverObject));

    ObtainSemaphore(&SD(cl)->global_lock);

    if (msg->driverObject)
    {	
        struct usb_driver *drv = AllocPooled(SD(cl)->MemPool, sizeof(struct usb_driver));

        if (drv)
        {
            int i;
            drv->d_Driver = msg->driverObject;

            for (i=0; i < (BITMAP_SIZE/32); i++) {
                drv->bitmap[i] = 0;
            }
            setBitmap(drv->bitmap, 0);
            setBitmap(drv->bitmap, 1);

            AddTail(&SD(cl)->driverList, &drv->d_Node);
            
            HIDD_USBHub_OnOff(drv->d_Driver, TRUE);

            retval = TRUE;
        }
    }

    ReleaseSemaphore(&SD(cl)->global_lock);    

    return retval;
}

BOOL METHOD(USB, Hidd_USB, DetachDriver)
{
    D(bug("[USB] USB::DetachDriver()\n"));

    return FALSE;	
}

uint8_t METHOD(USB, Hidd_USB, AllocAddress)
{
    struct usb_driver *drv = NULL;
    uint8_t addr = 0;

    ObtainSemaphore(&SD(cl)->global_lock);
    ForeachNode(&SD(cl)->driverList, drv)
    {
        if (drv->d_Driver == msg->driverObject)
            break;
    }
    ReleaseSemaphore(&SD(cl)->global_lock);

    if (drv)
        addr = allocBitmap(drv->bitmap);

    return addr;
}

void METHOD(USB, Hidd_USB, FreeAddress)
{
    struct usb_driver *drv = NULL;

    ObtainSemaphore(&SD(cl)->global_lock);
    ForeachNode(&SD(cl)->driverList, drv)
    {
        if (drv->d_Driver == msg->driverObject)
            break;
    }
    ReleaseSemaphore(&SD(cl)->global_lock);    

    if (drv)
        freeBitmap(drv->bitmap, msg->address);
}

OOP_Object *METHOD(USB, Hidd_USB, NewDevice)
{
    OOP_Object *new_device = NULL;
    APTR pipe;
    OOP_Object *bus;
    usb_device_descriptor_t descriptor;
    usb_config_descriptor_t config;
    void *cdesc;
    
    USBDevice_Request request = {
            bmRequestType:      UT_READ_DEVICE,
            bRequest:           UR_GET_DESCRIPTOR,
            wValue:             UDESC_DEVICE << 8,
            wIndex:             0,
            wLength:            8
    };
    
    memset(&descriptor, 0, sizeof(descriptor));
    
    OOP_GetAttr(msg->hub, aHidd_USBDevice_Bus, (IPTR*)&bus);
    
    if (bus)
    {
        struct usb_ExtClass *ec;
        
        pipe = HIDD_USBDrv_CreatePipe(bus, PIPE_Control, msg->fast, 0, 0, 0, 8, 100);
        
        HIDD_USBDrv_ControlTransfer(bus, pipe, &request, &descriptor, 8);
        
        request.wValue = UDESC_CONFIG << 8;
        request.wLength = USB_CONFIG_DESCRIPTOR_SIZE;
        
        HIDD_USBDrv_ControlTransfer(bus, pipe, &request, &config, USB_CONFIG_DESCRIPTOR_SIZE);
        
        cdesc = AllocVecPooled(SD(cl)->MemPool, AROS_LE2WORD(config.wTotalLength));
        if (cdesc)
        {
            request.wLength = config.wTotalLength;
            HIDD_USBDrv_ControlTransfer(bus, pipe, &request, cdesc, AROS_LE2WORD(config.wTotalLength));
        }
        
        HIDD_USBDrv_DeletePipe(bus, pipe);
        
        D(bug("[USB] USB::NewDevice()\n"));
        D(
          DumpDescriptor(&descriptor);
        );
    
        struct TagItem tags[] = {
                { aHidd_USBDevice_Interface,        0 },
                { aHidd_USBDevice_Address,          0 },
                { aHidd_USBDevice_Next,             0 },
                { aHidd_USBDevice_Hub,              (uintptr_t)msg->hub },
                { aHidd_USBDevice_Fast,             msg->fast },
                { aHidd_USBDevice_MaxPacketSize,    descriptor.bMaxPacketSize },
                { TAG_DONE, 0UL },
        };

        switch(descriptor.bDeviceClass)
        {
            case UDCLASS_HUB:
                new_device = OOP_NewObject(NULL, (STRPTR)CLID_Hidd_USBHub, tags);
                HIDD_USBHub_OnOff(new_device, TRUE);
                break;
    
            default:
            {
                int i;
                int addr = 0;
                
                /* Try a match for every interface */
                for (i = config.bNumInterface; i > 0; i--)
                {
                    tags[0].ti_Data = i - 1;
                    tags[1].ti_Data = addr;
                    
                    ForeachNode(&SD(cl)->extClassList, ec)
                    {
                        
                        D(bug("[USB] Trying external class \"%s\"\n", ec->ec_Node.ln_Name));
                        
                        if (!ec->ec_LibBase)
                            ec->ec_LibBase = OpenLibrary(ec->ec_Node.ln_Name, 0);
                        
                        if (ec->ec_LibBase)
                        {
                            new_device = NULL;
                            
                            void *clid = AROS_LVO_CALL3(void *, 
                                                          AROS_LCA(usb_device_descriptor_t *, &descriptor, A0),
                                                          AROS_LCA(usb_config_descriptor_t *, cdesc, A1),
                                                          AROS_LCA(int, i - 1, D0),
                                                          struct Library *, ec->ec_LibBase, 5,);
                            if (clid)
                            {
                                new_device = OOP_NewObject(NULL, (STRPTR)clid, tags);
                            
                                if (new_device)
                                {
                                    intptr_t a;
                                    OOP_GetAttr(new_device, aHidd_USBDevice_Address, &a);
                                    addr = a;
                                    tags[2].ti_Data = (intptr_t)new_device;
                                }
                            }
                        }
                    }
                }
                
                if (!new_device)
                    new_device = OOP_NewObject(NULL, (STRPTR)CLID_Hidd_USBDevice, tags);
                
                break;
            }
        }
        if (cdesc)
            FreeVecPooled(SD(cl)->MemPool, cdesc);
    }
    
    return new_device;
}
