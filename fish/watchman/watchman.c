/****************************************************************
*	A little hack inspired by "EyeCon" on the Suns.  
* 
* 	I use something like this on my workbench: 
* 	SetRGB4(vp, 0, 6, 6, 4); 
* 	SetRGB4(vp, 1, 4, 3, 0); 
* 	SetRGB4(vp, 2, 15, 12, 5); 
* 	SetRGB4(vp, 3, 10, 8, 3); 
*	Looks good in interlace and gives Amy a more 'pro' look.  
* 
* 	(C) 1989 Jonas Petersson & Sirius Soft
*
*	Comments to zaphod@malmax.maxcimator.se or
*		    zaphod@magrathea.sirius.se
*/


/* Figure out the #includes for yourself - or use the trick... */

#include <aros/oldprograms.h>
#include <exec/memory.h>
#include <proto/alib.h>

#include <stdlib.h>

#define Lx 29		/* The eyes offset from upper left corner */
#define Ly 49
#define Rx 49
#define Ry 46

#if (AROS_BIG_ENDIAN == 0)
#   define CONV_WORD(x) (((x) >> 8) | ((x) << 8))
#else
#   define CONV_WORD(x) x
#endif

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;

struct MsgPort *timerport;          /* for timer communications */
struct IOStdReq *timermsg;          /* for timer */

USHORT 	*Manmap_chip[2]= {NULL,NULL};	/* Chip copy */

USHORT Manmap[2][528] = {		/* Background head image */
					/* Probably in fastmem */
    {	0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFF00,
	0x8000,0x0000,0x0000,0x0000,0x0000,0x0100,
	0x9555,0x5555,0x7555,0x5555,0x5555,0x5700,
	0xAAAA,0xAAAF,0xFAAF,0xAAAA,0xAAAA,0xAB00,
	0x9555,0x557F,0xFD7F,0xD555,0x5555,0x5700,
	0xAAAA,0xBFFF,0xFDFF,0xFEAA,0xAAAA,0xAB00,
	0x9555,0xFFFF,0xFE7F,0xFFD5,0x5555,0x5700,
	0xAAFF,0xFFFF,0xFFBF,0xFFFA,0xAAAA,0xAB00,
	0x97FF,0xFFFF,0xFFCF,0xFFF5,0x5555,0x5700,
	0xBFFF,0xFFFF,0xFFF7,0xFFFA,0xAAAA,0xAB00,
	0xBFFF,0xFFFF,0xFFF9,0xFFF5,0x5555,0x5700,
	0xBFFF,0xFFFF,0xFFFE,0xFFFA,0xAAAA,0xAB00,
	0x9FFF,0xFFFF,0xFFFF,0x3FFD,0x5555,0x5700,
	0xAFFF,0xFFFF,0xFFFF,0xDFFA,0xAAAA,0xAB00,
	0x97FF,0xFFFF,0xFFFF,0xFFFD,0x5555,0x5700,
	0xAAFF,0xFFFF,0xFFFF,0xFFFE,0xAAAA,0xAB00,
	0x957F,0xFFFF,0xFFFF,0xFFFD,0x5555,0x5700,
	0xAABF,0xFFFF,0xFFFF,0xFFFE,0xAAAA,0xAB00,
	0x955F,0xFFFF,0xFFFF,0x5FFF,0x5555,0x5700,
	0xAAAF,0xFFFF,0xFFAA,0xAAFE,0xAAAA,0xAB00,
	0x9555,0xFFFF,0x807F,0xFD55,0x5555,0x5700,
	0xAAAA,0xFFF0,0x0FFF,0xFAAA,0xAAAA,0xAB00,
	0x9555,0x500F,0xFFFF,0xFF55,0x5555,0x5700,
	0xAAAA,0xA1FF,0xFFFF,0xFDAA,0xAAAA,0xAB00,
	0x9555,0x5FFF,0xFFFF,0xFAD5,0x5555,0x5700,
	0xAAAA,0xAFFF,0xFFFF,0xFD6A,0xAAAA,0xAB00,
	0x9555,0x5FFF,0xFFFF,0xFEB5,0x5555,0x5700,
	0xAAAA,0xAFFF,0xFFFF,0xFF5E,0xAAAA,0xAB00,
	0x9555,0x5FFF,0xFFFF,0xFFAD,0x5555,0x5700,
	0xAAAA,0xAFFF,0xFFFF,0xFFD6,0xAAAA,0xAB00,
	0x9555,0x5FFF,0xFFFF,0xFFAB,0x5555,0x5700,
	0xAAAA,0xAFFF,0xFFFF,0xFFD5,0xAAAA,0xAB00,
	0x9555,0x5FFF,0xFFFF,0xFFEB,0x5555,0x5700,
	0xAAAA,0xAFFF,0xFFFF,0xFFF5,0xAAAA,0xAB00,
	0x9555,0x5FFF,0xFFFF,0xFFEA,0xD555,0x5700,
	0xAAAA,0xAFFF,0xFFFF,0xFFF5,0x6AAA,0xAB00,
	0x9555,0x5FFF,0xFFFF,0xFFFA,0xB555,0x5700,
	0xAAAA,0xAFFF,0xFFFF,0xFFFD,0x6EAA,0xAB00,
	0x9555,0x5FFF,0xFFFF,0xFFEA,0xBB55,0x5700,
	0xAAAA,0xAFFF,0xFFFF,0xFD55,0x56AA,0xAB00,
	0x9555,0x5FFF,0xFFEA,0xBAAA,0xAB55,0x5700,
	0xAAAA,0xAFFF,0xFF55,0x7F55,0x56AA,0xAB00,
	0x9555,0x5FFE,0xAABA,0xFFFA,0xAD75,0x5700,
	0xAAAA,0xAF7F,0x555F,0xFFFD,0x5ABA,0xAB00,
	0x9555,0x5AFF,0xAEBF,0xC3FE,0xAD7D,0x5700,
	0xAAAA,0xADD5,0xFFFE,0x00FD,0x5ABC,0xAB00,
	0x9555,0x56BF,0xFFFC,0x007E,0xB57B,0x5700,
	0xAAAA,0xABF0,0x1FF4,0x007F,0x5ABC,0xAB00,
	0x9555,0x55E0,0x0FFC,0x007E,0xB57B,0x5700,
	0xAAAA,0xAAC0,0x07F4,0x007F,0x5AB4,0xAB00,
	0x9555,0x55C0,0x07FA,0x00FE,0xAF6B,0x5700,
	0xAAAA,0xAAC0,0x07F7,0xC7FF,0x55B6,0xAB00,
	0x9555,0x5560,0x0FEB,0xFF55,0x5551,0x5700,
	0xAAAA,0xAAF0,0x1FF7,0xEAAA,0xAAAA,0xAB00,
	0x9555,0x5578,0x3FAA,0x0005,0x5553,0x5700,
	0xAAAA,0xAABF,0xED50,0x0002,0xAAA8,0xAB00,
	0x9555,0x557D,0x5A00,0x0001,0x5555,0x5700,
	0xAAAA,0xAAAA,0xA000,0x0000,0xAAAA,0xAB00,
	0x9555,0x5550,0x0000,0x0001,0x5555,0x5700,
	0xAAAA,0xAA00,0x0000,0x0000,0xAAAA,0xAB00,
	0x9555,0x5400,0x0000,0x0000,0x5555,0x5700,
	0xAAAA,0xAA00,0x0000,0x0000,0x2AAA,0xAB00,
	0x9555,0x5500,0x0000,0x0000,0x5555,0x5700,
	0xAAAA,0xAA00,0x1E00,0x0000,0x2AAA,0xAB00,
	0x9555,0x5500,0x3F00,0x0020,0x1555,0x5700,
	0xAAAA,0xAA80,0xFF01,0x5550,0x0AAA,0xAB00,
	0x9555,0x5502,0xFF2A,0xAAA0,0x0555,0x5700,
	0xAAAA,0xAA87,0xFD55,0x5000,0x0FFF,0xFB00,
	0x9555,0x5545,0x7AA8,0x0000,0x1FFF,0xF700,
	0xAAAA,0xAA82,0xD540,0x0000,0x1FFF,0xFB00,
	0x9555,0x5541,0x0000,0x0000,0x3FFF,0xF700,
	0xAAAA,0xAA80,0x0000,0x0002,0xBFFF,0xFB00,
	0x9555,0x5540,0x0000,0x0555,0x7FFF,0xF700,
	0xAAAA,0xAA80,0x0002,0xAAAA,0xBFFF,0xEB00,
	0x9555,0x5540,0x0555,0x5555,0x7FFF,0xF700,
	0xAAAA,0xAA82,0xAAAA,0xAAAA,0xBFFF,0xEB00,
	0x9555,0x5555,0x5555,0x5555,0x5FFF,0xD700,
	0xAAAA,0xAAAA,0xAAAA,0xAAAA,0xBFFF,0xAB00,
	0x9555,0x5555,0x5555,0x5555,0x5FFF,0xD700,
	0xAAAA,0xAAAA,0xAAAA,0xAAAA,0xBFFF,0xAB00,
	0x9555,0x5555,0x5555,0x5555,0x5FEA,0xD700,
	0xAAAA,0xAAAA,0xAAAA,0xAAAA,0xD555,0x2B00,
	0x9555,0x5555,0x5555,0x56AA,0xAA55,0x5700,
	0xAAAA,0xAAAA,0xAAAB,0x5555,0x56AA,0xAB00,
	0x9555,0x5555,0x5555,0x5555,0x5555,0x5700,
	0xAAAA,0xAAAA,0xAAAA,0xAAAA,0xAAAA,0xAB00,
	0xBFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFF00,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFF00,
    },{
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0xA000,0x0000,0x0000,
	0x0000,0x0000,0x0055,0x5501,0x0000,0x0000,
	0x0000,0x0000,0x7FFF,0xFAAA,0x8000,0x0000,
	0x0000,0x000F,0xFFFF,0xF555,0x0000,0x0000,
	0x0000,0x0FFF,0xFFFF,0xFAAA,0x8000,0x0000,
	0x0000,0x1FFF,0xFFFF,0xFF55,0x4000,0x0000,
	0x0000,0x0FFF,0xFFFF,0xFFAA,0xA000,0x0000,
	0x0000,0x07FF,0xFFFF,0xFFD5,0x5000,0x0000,
	0x0000,0x0FFF,0xFFFF,0xFFEA,0xA000,0x0000,
	0x0000,0x07FF,0xFFFF,0xFFF5,0x5000,0x0000,
	0x0000,0x0FFF,0xFFFF,0xFFFA,0xA800,0x0000,
	0x0000,0x07FF,0xFFFF,0xFFFD,0x5400,0x0000,
	0x0000,0x0FFF,0xFFFF,0xFFFE,0xAA00,0x0000,
	0x0000,0x07FF,0xFFFF,0xFFFF,0x5500,0x0000,
	0x0000,0x0FFF,0xFFFF,0xFFFE,0xAA00,0x0000,
	0x0000,0x07FF,0xFFFF,0xFFFF,0x5500,0x0000,
	0x0000,0x0FFF,0xFFFF,0xFFFF,0xAA80,0x0000,
	0x0000,0x07FF,0xFFFF,0xFFFF,0xD540,0x0000,
	0x0000,0x0FFF,0xFFFF,0xFFFF,0xEAA0,0x0000,
	0x0000,0x07FF,0xFFFF,0xFFFF,0xD540,0x0000,
	0x0000,0x0FFF,0xFFFF,0xFFFF,0xEEA0,0x0000,
	0x0000,0x07FF,0xFFFF,0xFFFF,0xFD50,0x0000,
	0x0000,0x0FFF,0xFFFF,0xE7FF,0xFEA8,0x0000,
	0x0000,0x07FF,0xFFFF,0xC0FF,0xFD54,0x0000,
	0x0000,0x0FE3,0xFFE7,0x0FFF,0xFAAA,0x0000,
	0x0000,0x07C0,0xFFE0,0x3FFF,0xF554,0x0000,
	0x0000,0x0F80,0x71F8,0x03FF,0xFAB8,0x0000,
	0x0000,0x067E,0x0FFE,0x00FF,0xF55E,0x0000,
	0x0000,0x03FF,0xBFFC,0x007F,0xEABE,0x0000,
	0x0000,0x01F0,0x1FFC,0x007F,0xF55E,0x0000,
	0x0000,0x00E0,0x0FFC,0x007F,0xEABE,0x0000,
	0x0000,0x0040,0x07FC,0x007F,0xF55E,0x0000,
	0x0000,0x00C0,0x07FE,0x00FF,0xFABE,0x0000,
	0x0000,0x0040,0x07FF,0xC7FF,0xFF5C,0x0000,
	0x0000,0x0020,0x0FFF,0xFEAA,0xAAAC,0x0000,
	0x0000,0x0070,0x1FFF,0xD555,0x5556,0x0000,
	0x0000,0x0038,0x3FFF,0xFFFA,0xAAAE,0x0000,
	0x0000,0x001F,0xD7FF,0xFFFD,0x5557,0x0000,
	0x0000,0x003A,0xAFFF,0xFFFE,0xAAAA,0x0000,
	0x0000,0x0055,0x5FFF,0xFFFF,0x5555,0x0000,
	0x0000,0x00AF,0xFFFF,0xFFFE,0xAAAA,0x8000,
	0x0000,0x01FF,0xFFFF,0xFFFF,0x5555,0x4000,
	0x0000,0x03FF,0xFFE0,0x7FFF,0xAAAA,0x8000,
	0x0000,0x01FF,0xFF00,0x01FF,0xD555,0x4000,
	0x0000,0x00FF,0xF800,0x000F,0xAAAA,0xA000,
	0x0000,0x01FF,0xE000,0x000F,0xD555,0x4000,
	0x0000,0x00FF,0xC000,0x007F,0xEAAA,0xA000,
	0x0000,0x007F,0x0003,0xFFFF,0xF555,0x5000,
	0x0000,0x00FC,0x003F,0xFFFF,0xFAAA,0xA800,
	0x0000,0x0078,0x03FF,0xFFFF,0xF000,0x0000,
	0x0000,0x0038,0x0FFF,0xFFFF,0xE000,0x0000,
	0x0000,0x007C,0x3FFF,0xFFFF,0xE000,0x0000,
	0x0000,0x003E,0xFFFF,0xFFFF,0xC000,0x0000,
	0x0000,0x007F,0xFFFF,0xFFFD,0x4000,0x0000,
	0x0000,0x003F,0xFFFF,0xFAAA,0x8000,0x0000,
	0x0000,0x007F,0xFFFD,0x5555,0x4000,0x0000,
	0x0000,0x003F,0xFAAA,0xAAAA,0x8000,0x0000,
	0x0000,0x007D,0x5555,0x5555,0x4000,0x0000,
	0x0000,0x00AA,0xAAAA,0xAAAA,0xA000,0x0000,
	0x0000,0x0055,0x5555,0x5555,0x4000,0x0000,
	0x0000,0x002A,0xAAAA,0xAAAA,0xA000,0x0000,
	0x0000,0x0015,0x5555,0x5555,0x4000,0x0000,
	0x0000,0x000A,0xAAAA,0xAAAA,0xA03F,0x8000,
	0x0000,0x0005,0x5555,0x5555,0x7FFF,0x8000,
	0x0000,0x0002,0xAAAA,0xABFF,0xFF00,0x0000,
	0x0000,0x0001,0x5555,0xFFFF,0xFC00,0x0000,
	0x0000,0x0000,0xAAAA,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
   }
};


USHORT (*Eyemap_chip);         		/* Eye */
USHORT Eyemap [4] = {
        0x6000,
        0xf000,
        0xf000,
        0x6000
};

static struct Gadget gadget =		/* Used to move and quit */
{ NULL,
  0, 0, 0, 0,
  GADGHNONE|GRELWIDTH|GRELHEIGHT,
  GADGIMMEDIATE|RELVERIFY,
  WDRAGGING,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  0, 0
};

static struct NewWindow IconNW =	/* The window */
{ 351, 11,
  88, 88,
  -1, -1,
  GADGETDOWN,
  RMBTRAP|BORDERLESS|SMART_REFRESH|NOCAREREFRESH,
  &gadget,
  NULL,
  NULL,
  NULL,
  NULL,
  0, 0, 0, 0,
  WBENCHSCREEN
};

struct Window *win;

void CloseStuff(int);

void OpenStuff () {
        int i,j;
        extern USHORT *Eyemap_chip;
        extern USHORT *Manmap_chip[];
        PLANEPTR pl;

        if (!(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0L)))
                CloseStuff (1);
        if (!(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0L)))
                CloseStuff (1);
        if (!(win = OpenWindow (&IconNW)))
                CloseStuff (1);
        if ((Eyemap_chip = AllocMem((LONG)sizeof(Eyemap),MEMF_CHIP))== 0) 
        	CloseStuff (1);   /* allocate chip memory for images */
        for (i=0 ; i < 4 ;i++) {
		/* Do endian conversion */
                Eyemap_chip[i] = CONV_WORD(Eyemap[i]);
        }
        if ((Manmap_chip[0] = AllocMem((LONG)sizeof(Manmap[0]),MEMF_CHIP)) == NULL)
             CloseStuff (1);
        if ((Manmap_chip[1] = AllocMem((LONG)sizeof(Manmap[1]),MEMF_CHIP)) == NULL)
             CloseStuff (1);

        for (i=0 ; i < 528 ;i++) {
		
		/* Do endian conversion */
                Manmap_chip[0][i] = CONV_WORD(Manmap[0][i]);
                Manmap_chip[1][i] = CONV_WORD(Manmap[1][i]);
        }
        SetAPen(win->RPort, 1L);
        SetDrMd(win->RPort, JAM1);
        BltTemplate(Manmap_chip[0], 0L, 12L, win->RPort, 0L, 0L, 88L, 88L);

        SetAPen(win->RPort, 2L);
	SetWriteMask(win->RPort, 2L);       /* Braindead COMPLEMENT */
        BltTemplate(Manmap_chip[1], 0L, 12L, win->RPort, 0L, 0L, 88L, 88L);
        SetWriteMask(win->RPort, 255L);     /* Braindead COMPLEMENT */
        if (!(timerport = CreatePort(NULL, 0L))
            || !(timermsg = CreateStdIO(timerport))
            || OpenDevice(TIMERNAME,UNIT_VBLANK, timermsg, 0L))
                CloseStuff(1);
}

void CloseStuff (exitcode)
int exitcode; 
{
        if (timermsg)       {
                CloseDevice(timermsg);
                DeleteStdIO(timermsg);
        }
        if (timerport)          DeletePort(timerport);
        if (win)                CloseWindow(win);
        if (IntuitionBase)      CloseLibrary(IntuitionBase);
        if (GfxBase)            CloseLibrary(GfxBase);
        if (Eyemap_chip)        FreeMem(Eyemap_chip, (LONG)sizeof(Eyemap));
        if (Manmap_chip[0])     FreeMem(Manmap_chip[0], (LONG)sizeof(Manmap[0]));
        if (Manmap_chip[1])     FreeMem(Manmap_chip[1], (LONG)sizeof(Manmap[1]));
        exit (exitcode);
}

/* Hmm, this seems to work - a bit strange, but it works... */
#define ABS(x) (x<0 ? -x : x)
#define index(x,y)   (((y<0)<<2) + ((x<0)<<1) + (ABS(x)>ABS(y)))

/****************************************************************
*	Draw the eye at OffX, OffY but with an extra offset
*	given by idx in a direct[]-table.
*	col is used to erase/draw.
*/

void DrawEye(idx, OffX, OffY, col)
int idx, OffX, OffY, col;
{
        static int direct[2][8] = {
                { 1, 2,-1,-2, 1, 2,-1,-2},
                { 2, 1, 2, 1,-2,-1,-2,-1}};

        SetAPen(win->RPort, (LONG)col);
        BltTemplate(Eyemap_chip, 0L, 2L, win->RPort,
                (LONG)(OffX + direct[0][idx&7]), 
                (LONG)(OffY + direct[1][idx&7]), 4L, 4L);
}

/****************************************************************
*	Get mouse position relative to windows upper left corner.
*	(A lot easier than I thought...) A very nice programmer
*	probably would get the mouse coords in another way, but
*	I'm not very paranoid...
*/

void GetRelMp(x,y)
int *x,*y;
{
        *x = win->WScreen->MouseX - win->LeftEdge;
        *y = win->WScreen->MouseY - win->TopEdge;
}

/****************************************************************
*	The main loop. Ok, I COULD do the timer and intuimsgs
*	in a nicer way, but it wouldn't make much of a difference
*	so why bother?
*/

main ()
{
    ULONG msgclass;  		/* Message class from Intuition message */
    struct IntuiMessage *msg;	/* The msg we got */
    int Mx, My,			/* Mouse's current relative offset */ 
    	LastL, LastR, 		/* Last index for left&right Eye */
    	NewL, NewR;		/* New index    --   ""   --     */
    ULONG PrevSecs, PrevMics;	/* Time of last click */

    /* Init */
    OpenStuff ();
    DrawEye(LastL, Lx, Ly, 1);
    DrawEye(LastR, Rx, Ry, 1);

    for(;;) {
        timermsg->io_Command = TR_ADDREQUEST;   /* add a new timer request */
        timermsg->io_Actual = 0;                /* seconds */
        timermsg->io_Length = 100000;           /* microseconds */
        DoIO(timermsg);                         /* Wait for a while... */
        while(msg = (struct IntuiMessage *)GetMsg(win->UserPort)) {    
            /* User action */
            msgclass = msg->Class;
            ReplyMsg (msg);
            switch (msgclass) {
            case GADGETDOWN:
                if (DoubleClick(PrevSecs, PrevMics, msg->Seconds, msg->Micros))
                    CloseStuff(0);
                else {
                    PrevSecs = msg->Seconds;
                    PrevMics = msg->Micros;
                }
            }
        }
	
        GetRelMp(&Mx, &My);                      /* Action ! */
        if (LastL != (NewL = index(Mx - Lx, My - Ly))) {
            DrawEye(LastL, Lx, Ly, 0);
            DrawEye(LastL=NewL, Lx, Ly, 1);
        }
        if (LastR != (NewR = index(Mx - Rx, My - Ry))) {
            DrawEye(LastR, Rx, Ry, 0);
            DrawEye(LastR=NewR, Rx, Ry, 1);
        }
    }
}
