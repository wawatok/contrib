/* volumes.c:
 *
 * Volume management.
 *
 * ----------------------------------------------------------------------
 * This code is (C) Copyright 1993,1994 by Frank Munkert.
 * All rights reserved.
 * This software may be freely distributed and redistributed for
 * non-commercial purposes, provided this notice is included.
 * ----------------------------------------------------------------------
 * History:
 * 
 * 19-Sep-94   fmu   Fixed bug in Reinstall_Locks()
 * 22-May-94   fmu   Performance improvement for lock+filehandle processing.
 * 09-Jul-02 sheutlin  various changes when porting to AROS
 *                     - global variables are now in a struct Globals *global
 */

#include <proto/exec.h>
#include <proto/utility.h>
#include <stdlib.h>
#include <string.h>

#include "volumes.h"
#include "cdrom.h"
#include "device.h"
#include "devsupp.h"
#include "generic.h"
#include "debug.h"
#include "path.h"
#include "globals.h"

#include "clib_stuff.h"

extern struct Globals *global;

#ifdef SysBase
#	undef SysBase
#endif
#define SysBase global->SysBase
#ifdef UtilityBase
#	undef UtilityBase
#endif
#define UtilityBase global->UtilityBase

/*  Associate p_lock with the pathname of the locked object on the current
 *  volume.
 */

void Register_Lock (LOCK *p_lock)
{
  t_lock_node *new;
  BUG(char pathname[300];)

  BUG
  (
    if (!Full_Path_Name ((CDROM_OBJ*) p_lock->fl_Link, pathname, sizeof (pathname))) {
      dbprintf ("[Cannot install lock / cannot determine pathname]");
      return;
    }
  );

  new = (t_lock_node*) AllocMem (sizeof (t_lock_node), MEMF_PUBLIC);
  if (!new) {
    BUG(dbprintf ("[Cannot install lock on '%s']", pathname);)
    return;
  }

  new->pathlist = Copy_Path_List (((CDROM_OBJ*) p_lock->fl_Link)->pathlist, FALSE);

  new->vol_name = (char*) AllocMem (strlen (global->g_vol_name+1) + 1,
  				    MEMF_PUBLIC);
  if (!new->vol_name) {
    BUG(dbprintf ("[Cannot install lock on '%s']", pathname);)
    FreeMem (new, sizeof (t_lock_node));
    return;
  }
  strcpy (new->vol_name, global->g_vol_name+1);

  new->lock = p_lock;
  new->next = global->g_lock_list;
  global->g_lock_list = new;
  
  BUG(dbprintf ("[Installing lock on '%s']", pathname);)
}

/*  Remove the entry for p_lock in the list g_lock_list.
 */

void Unregister_Lock (LOCK *p_lock)
{
  t_lock_node *ptr, *old;
  BUG(char pathname[300];)

  for (ptr=global->g_lock_list, old = NULL; ptr; old = ptr, ptr = ptr->next)
    if (ptr->lock == p_lock) {
      BUG
      (
        if (!Path_Name_From_Path_List (ptr->pathlist, pathname,
                                       sizeof (pathname))) {
          dbprintf ("[cannot determine pathname]");
          return;
        }
        dbprintf ("[Removing lock from '%s']", pathname);
      );
      if (old)
        old->next = ptr->next;
      else
        global->g_lock_list = ptr->next;
      Free_Path_List (ptr->pathlist);
      FreeMem (ptr->vol_name, strlen (ptr->vol_name) + 1);
      FreeMem (ptr, sizeof (t_lock_node));
      return;
    }
  BUG(dbprintf ("[Lock cannot be removed %08lx]", (unsigned long) p_lock);)
}

/*  Update the fl_Link values for all locks that have been
 *  registered for a volume with the name g_vol_name.
 *  The fl_Link value is a pointer to a CDROM_OBJ object which
 *  respresents the locked file or directory.
 *
 *  Returns the number of locks on the volume.
 */

int Reinstall_Locks (void)
{
  t_lock_node *ptr;
  CDROM_OBJ* obj;
  int result = 0;
  char pathname[300];

  for (ptr=global->g_lock_list; ptr; ptr = ptr->next) {
    if (strcmp (global->g_vol_name+1, ptr->vol_name) == 0) {
      result++;
      if (!Path_Name_From_Path_List (ptr->pathlist, pathname, sizeof (pathname))) {
        BUG(dbprintf ("[cannot determine pathname]");)
        break;
      }
      BUG(dbprintf ("[Reinstalling lock on '%s'", pathname);)
      obj = Open_Object (global->g_top_level_obj, pathname);
      if (obj) {
        BUG(dbprintf ("]\n");)
      } else {
        BUG(dbprintf ("(FAILED) ]\n");)
	continue;
      }
      ptr->lock->fl_Link = (LONG) obj;
    }
  }
  return result;
}

/*  Associate p_obj with the pathname of the associated disk object on the current
 *  volume.
 */

void Register_File_Handle(CDROM_OBJ *p_obj)
{
t_fh_node *new;

	new = (t_fh_node*) AllocMem (sizeof (t_fh_node), MEMF_PUBLIC);
	if (!new)
	{
		return;
	}

	new->vol_name = (char*) AllocMem (StrLen (global->g_vol_name+1) + 1, MEMF_PUBLIC);
	if (!new->vol_name)
	{
		FreeMem (new, sizeof (t_fh_node));
		return;
	}
	StrCpy (new->vol_name, global->g_vol_name+1);

	new->obj = p_obj;
	new->devlist = global->DevList;
	new->next = global->g_fh_list;
	global->g_fh_list = new;

}

/*  Remove the entry for the file handle p_obj in the list g_fh_list.
 */

void Unregister_File_Handle(CDROM_OBJ *p_obj) {
t_fh_node *ptr, *old;
  
	for (ptr=global->g_fh_list, old = NULL; ptr; old = ptr, ptr = ptr->next)
		if (ptr->obj == p_obj && StrCmp (global->g_vol_name+1, ptr->vol_name) == 0)
		{
			if (old)
				old->next = ptr->next;
			else
				global->g_fh_list = ptr->next;
			FreeMem (ptr->vol_name, StrLen (ptr->vol_name) + 1);
			FreeMem (ptr, sizeof (t_fh_node));
			return;
		}
}

/*  Update the volume pointer for all CDROM_OBJs which are used as
 *  filehandles for the current volume.
 *
 *  Returns the number of file handles on the volume.
 */

int Reinstall_File_Handles (void)
{
  t_fh_node *ptr;
  int result = 0;

  for (ptr=global->g_fh_list; ptr; ptr = ptr->next) {
    if (StrCmp (global->g_vol_name+1, ptr->vol_name) == 0) {
      result++;
      ptr->obj->volume = global->g_volume;
    }
  }
  return result;
}

struct DeviceList *Find_Dev_List (CDROM_OBJ *p_obj) {
t_fh_node *ptr;

	for (ptr=global->g_fh_list; ptr; ptr = ptr->next)
	{
		if (ptr->obj == p_obj)
		{
			return ptr->devlist;
		}
	}
	return NULL;
}

/*  Register a volume node as owned by this handler.
 */

void Register_Volume_Node(struct DeviceList *p_volume) {
t_vol_reg_node *new;
int len;
  
	new = (t_vol_reg_node*) AllocMem (sizeof (t_vol_reg_node), MEMF_PUBLIC);
	if (!new)
		return;

	new->volume = p_volume;
	len = AROS_BSTR_StrLen(p_volume->dl_OldName);
	new->name = (char*) AllocMem (len + 1, MEMF_PUBLIC);
	if (!new)
	{
		FreeMem (new, sizeof (t_vol_reg_node));
		return;
	}
	CopyMem((char*) AROS_BSTR_ADDR(p_volume->dl_OldName), new->name, len);
	new->name[len] = 0;
	new->next = global->g_volume_list;
	global->g_volume_list = new;
}

/*  Remove the registration for the volume node.
 */

void Unregister_Volume_Node(struct DeviceList *p_volume) {
t_vol_reg_node *ptr, *old;

	for (ptr=global->g_volume_list, old=NULL; ptr; old=ptr, ptr=ptr->next)
	{
		if (p_volume == ptr->volume)
		{
			if (old)
				old->next = ptr->next;
			else
				global->g_volume_list = ptr->next;
			FreeMem (ptr->name, StrLen (ptr->name) + 1);
			FreeMem (ptr, sizeof (t_vol_reg_node));
			return;
		}
	}
}

/*  Find a volume node with a matching name.
 */

struct DeviceList *Find_Volume_Node(char *p_name) {
t_vol_reg_node *ptr;

	for (ptr=global->g_volume_list; ptr; ptr=ptr->next)
	{
		if (Stricmp (ptr->name, p_name) == 0)
			return ptr->volume;
	}
	return NULL;
}
