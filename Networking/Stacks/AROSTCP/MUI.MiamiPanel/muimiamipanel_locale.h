#ifndef _LOCALE_H_
#define _LOCALE_H_

/*
    Copyright � 2003-2004, The AROS Development Team. All rights reserved.
    $Id: locale.h 21058 2004-02-23 19:14:08Z chodorowski $
*/

#include <exec/types.h>
#include <libraries/gadtools.h>
#include <MUI/TheBar_mcc.h>

#define CATCOMP_NUMBERS
#include "strings.h"

/*** Prototypes *************************************************************/
/* Main *********************************************************************/
CONST_STRPTR _(ULONG ID,struct MiamiPanelBase_intern *MiamiPanelBaseIntern);       /* Get a message, as a STRPTR */
#define __(id) ((IPTR) _(id, MiamiPanelBaseIntern))   /* Get a message, as an IPTR */

void localizeArray ( UBYTE **strings , ULONG *ids );
void localizeMenus ( struct NewMenu *menu , ULONG *ids );
void localizeButtonsBar ( struct MUIS_TheBar_Button *buttons , ULONG *ids );

/* Setup ********************************************************************/
VOID Locale_Initialize(struct MiamiPanelBase_intern *MiamiPanelBaseIntern);
VOID Locale_Deinitialize(struct MiamiPanelBase_intern *MiamiPanelBaseIntern);

#endif /* _LOCALE_H_ */
