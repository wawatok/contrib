/* $Id$ */

/*
     toccata.library - AHI-based Toccata emulation library
     Copyright (C) 1997-2003 Martin Blom <martin@blom.org> and Teemu Suikki.
     
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.
     
     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.
     
     You should have received a copy of the GNU Library General Public
     License along with this library; if not, write to the
     Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
     MA 02139, USA.
*/

/*
 * $Log$
 * Revision 1.3  2003/04/29 18:18:00  chodorowski
 * Updated to 5.8.
 *
 * Revision 1.4  2003/01/19 12:22:59  martin
 * Another year, another copyright update.
 *  ... which seems to have caused the translation files to change slightly.
 *
 * Revision 1.3  2002/01/03 08:56:24  martin
 * Added license notice.
 *
 * Revision 1.2  1997/06/25 19:55:04  lcs
 * Modified to make the library base compatible with the public
 * part of ToccataBase.
 * Added __UserLibOpen() and __UserLibClose().
 *
 *
 */

#define  _USEOLDEXEC_ 1
#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/memory.h>
#include <exec/resident.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <libraries/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <string.h>

long _WBenchMsg;

/* Prototypes */
ULONG __asm _LibExpunge( register __a6 struct MyLibrary *libbase );
ULONG __asm _LibInit   ( register __a0 APTR seglist,
                         register __d0 struct MyLibrary *libbase );

int  __saveds __asm __UserLibInit   (register __a6 struct MyLibrary *libbase);
void __saveds __asm __UserLibCleanup(register __a6 struct MyLibrary *libbase);

int  __saveds __asm __UserLibOpen   (register __a6 struct MyLibrary *libbase);
void __saveds __asm __UserLibClose  (register __a6 struct MyLibrary *libbase);

int  __saveds __asm __UserDevInit   (register __d0 long unit,
                                     register __a0 struct IORequest *ior,
                                     register __a6 struct MyLibrary *libbase);
void __saveds __asm __UserDevCleanup(register __a0 struct IORequest *ior,
                                     register __a6 struct MyLibrary *libbase);

int  __saveds __asm __libfpinit     (register __a6 struct MyLibrary *libbase);
void __saveds __asm __libfpterm     (register __a6 struct MyLibrary *libbase);

struct MyLibrary {
        struct             Library ml_Lib;
        APTR               ml_BoardAddress;   /* Toccata */
        APTR               ml_HardInfo;       /* Toccata */
        ULONG              ml_SegList;
        ULONG              ml_Flags;
        APTR               ml_ExecBase; /* pointer to exec base  */
#ifndef ONE_GLOBAL_SECTION
        long *             ml_relocs;   /* pointer to relocs.    */
        struct MyLibrary * ml_origbase; /* pointer to original library base  */
        long               ml_numjmps;
#endif
};

typedef LONG (*myPFL)();   /* pointer to function returning 32-bit int      */

/* library initialization table, used for AUTOINIT libraries                */
struct InitTable {
        ULONG        *it_DataSize;       /* library data space size         */
        myPFL        *it_FuncTable;      /* table of entry points           */
        APTR         it_DataInit;        /* table of data initializers      */
        myPFL        it_InitFunc;        /* initialization function to run  */
};

#ifndef ONE_GLOBAL_SECTION
long _WBenchMsg;
long _OSERR;
long __base;
#endif

/* symbols generated by blink */
extern char __far _LibID[];             /* ID string                        */
extern char __far _LibName[];           /* Name string                      */
extern char __far RESLEN;               /* size of init data                */
extern long __far NEWDATAL;             /* size of global data              */
extern long __far NUMJMPS;              /* number of jmp vectors to copy    */
extern myPFL _LibFuncTab[];             /* my function table                */
extern long __far _LibVersion;          /* Version of library               */
extern long __far _LibRevision;         /* Revision of library              */
#define MYVERSION ((long)&_LibVersion)
#define MYREVISION ((long)&_LibRevision)
#define DATAWORDS ((long)&NEWDATAL)     /* magic to get right tpye of reloc */ 
#define SIZEJMPTAB ((long)libbase->ml_origbase->ml_numjmps)
                                        /* size in bytes of jmp table       */

/* From libent.o, needed to determine where data is loaded by loadseg       */
extern long far _Libmergeddata; 

#define MYLIBRARYSIZE ((sizeof(struct MyLibrary) +3) & ~3)



struct InitTable __far _LibInitTab =  {
        (long *)(&RESLEN+MYLIBRARYSIZE),
        _LibFuncTab,
        NULL,                        /* will initialize my own data */
        _LibInit,
};

__asm ULONG _LibInit( register __a0 APTR seglist,
                      register __d0 struct MyLibrary *libbase )
{
#ifdef ONE_GLOBAL_SECTION
    long *reloc;
#endif
    long *sdata;
    char *ddata;
    long nrelocs;

      
    libbase->ml_SegList = (ULONG) seglist;

    /* init. library structure (since I don't do automatic data init.) */
#ifdef DEVICE
    libbase->ml_Lib.lib_Node.ln_Type = NT_DEVICE;
#else
    libbase->ml_Lib.lib_Node.ln_Type = NT_LIBRARY;
#endif
    libbase->ml_Lib.lib_Node.ln_Name =  _LibName;
    libbase->ml_Lib.lib_Flags = LIBF_SUMUSED | LIBF_CHANGED;
    libbase->ml_Lib.lib_Version = MYVERSION;
    libbase->ml_Lib.lib_Revision = MYREVISION;
    libbase->ml_Lib.lib_IdString = (APTR) _LibID;
#ifndef ONE_GLOBAL_SECTION
    libbase->ml_relocs = NULL;
    libbase->ml_origbase = libbase;
    sdata = (long *)_LibInitTab.it_FuncTable;
    libbase->ml_numjmps = (long)&NUMJMPS;
#endif
     /* Start of copy of global data after structure */
    ddata = (char *)libbase + MYLIBRARYSIZE; 

    sdata = (long *)&_Libmergeddata; /* where loadseg loaded the data */
    memcpy(ddata, (void *)sdata, DATAWORDS*4);

    /* perform relocs if we want one global section for all programs */
    /* that have this lib open. If we want a global section for each */
    /* open, copy the relocs, and do them on each open call.         */
    sdata = sdata + DATAWORDS;
    nrelocs = *sdata;
#ifdef ONE_GLOBAL_SECTION
    sdata++;
    while (nrelocs > 0)
    {
       reloc = (long *)((long)ddata + *sdata++);
       *reloc += (long)ddata;
       nrelocs--;
    }
    
#ifndef DEVICE
    if (__UserLibInit(libbase) != 0)
       return NULL; /* abort if user init failed */
#endif

#else
    if (nrelocs) 
    {
      if ((libbase->ml_relocs = AllocMem((nrelocs * 4) + 4, MEMF_PUBLIC)) == NULL)
        return 0;
      memcpy((void *)libbase->ml_relocs, (void *)sdata, (nrelocs * 4) + 4);
    }
#endif

    return ( (ULONG) libbase );
}

LONG __asm _LibOpen( 
#ifdef DEVICE
                     register __d0 long unit,
                     register __a1 struct IORequest *ior,
#endif
                     register __a6 struct MyLibrary *libbase )
{
#ifndef ONE_GLOBAL_SECTION
    struct MyLibrary *origbase = libbase;
    struct ExecBase *SysBase = *(struct ExecBase **)4;
    char *newlib;
    long *sdata, *ddata, *reloc;
    long nrelocs;
#endif

    /* mark us as having another customer */
    libbase->ml_Lib.lib_OpenCnt++;

    /* clear delayed expunges (standard procedure) */
    libbase->ml_Lib.lib_Flags &= ~LIBF_DELEXP;

#ifndef ONE_GLOBAL_SECTION
    /* Allocate new lib base */
    newlib = AllocMem((long)(MYLIBRARYSIZE + 
                             ((long)&RESLEN) + SIZEJMPTAB), 
                             MEMF_PUBLIC|MEMF_CLEAR);

    if (newlib == NULL) goto error;
        
    /* copy over data */
    memcpy(newlib, (char *)libbase - SIZEJMPTAB, 
           (long)(MYLIBRARYSIZE + DATAWORDS*4 + SIZEJMPTAB));
    
    libbase = (struct MyLibrary *)(newlib+SIZEJMPTAB);
    libbase->ml_relocs = NULL;
    
    /* perform relocs */       
    ddata = (long *)((char *)libbase + MYLIBRARYSIZE); 
    sdata = libbase->ml_origbase->ml_relocs;
    if (sdata)
    {
       nrelocs = *sdata++;
       while (nrelocs > 0)
       {
          reloc = (long *)((long)ddata + *sdata++);
          *reloc += (long)ddata;
          nrelocs--;
       }
    }

    /* now we need to flush the cache because we copied the jmp table */
    if (SysBase->LibNode.lib_Version >= 36) 
      CacheClearU();

#ifdef DEVICE
    ior->io_Device = (struct Device *)libbase; /* local copy of libary base */
#endif

    if (__libfpinit(libbase) || 
#ifdef DEVICE
        __UserDevInit(unit, ior, libbase) != 0
#else
        (__UserLibInit(libbase) != 0) ||
        __UserLibOpen(libbase) != 0)
#endif
       )
    {
       __libfpterm(libbase);
       FreeMem(newlib, (long)(MYLIBRARYSIZE + 
                       ((long)&RESLEN) + SIZEJMPTAB));
error:
       origbase->ml_origbase->ml_Lib.lib_OpenCnt--;
       return NULL; /* abort if user init failed */
    }

#else
#ifdef DEVICE
    if (__UserDevInit(unit, ior, libbase) != 0)
    {
        libbase->ml_Lib.lib_OpenCnt--;
        return NULL;
    }    
#endif
    if (__UserLibOpen(libbase) != 0)
    {
        libbase->ml_Lib.lib_OpenCnt--;
        return NULL;
    }    
#endif

    return ( (LONG) libbase );
}

ULONG __asm _LibClose( 
#ifdef DEVICE
                       register __a1 struct IORequest *ior,
#endif                       
                       register __a6 struct MyLibrary *libbase )
{
    ULONG retval = 0;
    
#ifndef ONE_GLOBAL_SECTION
    struct MyLibrary *origbase;

    if (libbase != libbase->ml_origbase)
    {
#ifdef DEVICE
       __UserDevCleanup(ior, libbase);
#else
       __UserLibClose(libbase);
       __UserLibCleanup(libbase);
#endif
       __libfpterm(libbase);
       origbase = libbase->ml_origbase;
       FreeMem((char *)libbase-SIZEJMPTAB, 
               (long)(MYLIBRARYSIZE + ((long)&RESLEN)+SIZEJMPTAB));
       libbase = origbase;
    }
#else
    __UserLibClose(libbase);
#ifdef DEVICE
    __UserDevCleanup(ior, libbase);
#endif
#endif

    if (( --libbase->ml_Lib.lib_OpenCnt == 0 ) &&
                        ( libbase->ml_Lib.lib_Flags & LIBF_DELEXP ))
    {
        /* no more people have me open,
         * and I have a delayed expunge pending
         */
         retval = _LibExpunge( libbase ); /* return segment list        */
    }

    return (retval);
}

ULONG __asm _LibExpunge( register __a6 struct MyLibrary *libbase )
{
    ULONG seglist = 0;
    LONG  libsize;

#ifndef ONE_GLOBAL_SECTION
    libbase = libbase->ml_origbase;
#endif

    libbase->ml_Lib.lib_Flags |= LIBF_DELEXP;
    if ( libbase->ml_Lib.lib_OpenCnt == 0 )
    {
        /* really expunge: remove libbase and freemem        */
#ifndef ONE_GLOBAL_SECTION
        if (libbase->ml_relocs)
           FreeMem(libbase->ml_relocs, (*libbase->ml_relocs * 4) + 4);
#else
        __UserLibCleanup(libbase);
#endif
        seglist = libbase->ml_SegList;

        Remove( (struct Node *) libbase);

        libsize = libbase->ml_Lib.lib_NegSize + libbase->ml_Lib.lib_PosSize;
        FreeMem( (char *) libbase - libbase->ml_Lib.lib_NegSize,(LONG) libsize );
    }

    /* return NULL or real seglist                                */
    return ( (ULONG) seglist );
}
