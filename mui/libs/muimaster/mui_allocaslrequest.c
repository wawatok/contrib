/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: english
*/

#include "muimaster_intern.h"

/*****************************************************************************

    NAME */
#ifndef _AROS
__asm APTR MUI_AllocAslRequest(register __d0 unsigned long reqType, register __a0 struct TagItem *tagList)
#else
	AROS_LH2(ULONG, MUI_AllocAslRequest,

/*  SYNOPSIS */
	AROS_LHA(unsigned long, reqType, D0),
	AROS_LHA(struct TagItem *, tagList, A0),

/*  LOCATION */
	struct MUIMasterBase *, MUIMasterBase, 8, MUIMaster)
#endif
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS
	The function itself is a bug ;-) Remove it!

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct MUIMasterBase *,MUIMasterBase)

    AROS_LIBFUNC_EXIT

} /* MUIA_AllocAslRequest */
