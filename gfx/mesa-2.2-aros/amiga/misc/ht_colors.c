/*#include "headers.h"*/
#include <intuition/screens.h>
#include "ht_colors.h"
#include <proto/intuition.h>
#include <proto/graphics.h>


#define c8tc32(x) (((((((x)<<8)|(x))<<8)|(x))<<8)|(x))

BOOL AllocColors(struct ColorEntry *Cols,long Number,struct Screen *Screen)
{
	long i,fail=0,pen;
/*  UWORD hi_col,lo_col;*/
	ULONG ColTab[3];
	for(i=0;i<Number;i++){
		pen=ObtainBestPen(Screen->ViewPort.ColorMap,c8tc32(Cols[i].r),c8tc32(Cols[i].g),c8tc32(Cols[i].b),
			OBP_Precision, PRECISION_EXACT,TAG_DONE);
		if((Cols[i].PenNo=pen)==-1)
			fail=1;
		else{
			GetRGB32(Screen->ViewPort.ColorMap,pen,1,ColTab);
			Cols[i].ar=ColTab[0]>>24;
			Cols[i].ag=ColTab[1]>>24;
			Cols[i].ab=ColTab[2]>>24;
			}
		}
	return (BOOL)fail;
}

void FreeColors(struct ColorEntry *Cols,long Number,struct Screen *Screen)
{
	long i;
	for(i=0;i<Number;i++)
		ReleasePen(Screen->ViewPort.ColorMap,Cols[i].PenNo);
}

#define sqr(x) ((x)*(x))

#ifdef __GNUC__
ULONG GetPen(register struct ColorEntry *Cols,register UWORD Number,register long R,register long G,register long B)
{
	long i,cn=-1,lcn,r;
	long dist=0x7ffffff,ldist;
//  kprintf("Requesting pen r: %ld g: %ld b: %ld\n",R,G,B);
	for(i=0;i<Number;i++){
		if(!(r=sqr(Cols[i].ar-R)+sqr(Cols[i].ag-G)+sqr(Cols[i].ab-B))){
			lcn=-1;
			cn=i;
			break;
			}
		if(r<dist){
			ldist=dist;
			dist=r;
			lcn=cn;
			cn=i;
			}
		}
//  kprintf("Returned pen no: %ld Col r: %ld g: %ld b: %ld | dist: %ld 2nd dist: %ld \n",Cols[cn].PenNo,Cols[cn].ar,Cols[cn].ag,Cols[cn].ab,dist,ldist);
	if(lcn==-1)
		lcn=cn;
	else{
		if(dist*2 < ldist)
			lcn=cn;
		}
	return (ULONG)(Cols[cn].PenNo | (Cols[lcn].PenNo<<16));
}
#else
/*
__asm ULONG GetPen(register __a0 struct ColorEntry *Cols,register __d0 UWORD Number,register __d1 long R,register __d2 long G,register __d3 long B)
{
	long i,cn=-1,lcn,r;
	long dist=0x7ffffff,ldist;
//  kprintf("Requesting pen r: %ld g: %ld b: %ld\n",R,G,B);
	for(i=0;i<Number;i++){
		if(!(r=sqr(Cols[i].ar-R)+sqr(Cols[i].ag-G)+sqr(Cols[i].ab-B))){
			lcn=-1;
			cn=i;
			break;
			}
		if(r<dist){
			ldist=dist;
			dist=r;
			lcn=cn;
			cn=i;
			}
		}
//  kprintf("Returned pen no: %ld Col r: %ld g: %ld b: %ld | dist: %ld 2nd dist: %ld \n",Cols[cn].PenNo,Cols[cn].ar,Cols[cn].ag,Cols[cn].ab,dist,ldist);
	if(lcn==-1)
		lcn=cn;
	else{
		if(dist*2 < ldist)
			lcn=cn;
		}
	return (ULONG)(Cols[cn].PenNo | (Cols[lcn].PenNo<<16));
}
*/
#endif

struct ColorEntry pal[]={
{0x00,0x00,0x00,0,0,0,0,0,0},
{0x00,0x00,0x3f,0,0,0,0,0,0},
{0x00,0x00,0x7f,0,0,0,0,0,0},
{0x00,0x00,0xbf,0,0,0,0,0,0},
{0x00,0x00,0xff,0,0,0,0,0,0},
{0x00,0x3f,0x00,0,0,0,0,0,0},
{0x00,0x3f,0x3f,0,0,0,0,0,0},
{0x00,0x3f,0x7f,0,0,0,0,0,0},
{0x00,0x3f,0xbf,0,0,0,0,0,0},
{0x00,0x3f,0xff,0,0,0,0,0,0},
{0x00,0x7f,0x00,0,0,0,0,0,0},
{0x00,0x7f,0x3f,0,0,0,0,0,0},
{0x00,0x7f,0x7f,0,0,0,0,0,0},
{0x00,0x7f,0xbf,0,0,0,0,0,0},
{0x00,0x7f,0xff,0,0,0,0,0,0},
{0x00,0xbf,0x00,0,0,0,0,0,0},
{0x00,0xbf,0x3f,0,0,0,0,0,0},
{0x00,0xbf,0x7f,0,0,0,0,0,0},
{0x00,0xbf,0xbf,0,0,0,0,0,0},
{0x00,0xbf,0xff,0,0,0,0,0,0},
{0x00,0xff,0x00,0,0,0,0,0,0},
{0x00,0xff,0x3f,0,0,0,0,0,0},
{0x00,0xff,0x7f,0,0,0,0,0,0},
{0x00,0xff,0xbf,0,0,0,0,0,0},
{0x00,0xff,0xff,0,0,0,0,0,0},
{0x3f,0x00,0x00,0,0,0,0,0,0},
{0x3f,0x00,0x3f,0,0,0,0,0,0},
{0x3f,0x00,0x7f,0,0,0,0,0,0},
{0x3f,0x00,0xbf,0,0,0,0,0,0},
{0x3f,0x00,0xff,0,0,0,0,0,0},
{0x3f,0x3f,0x00,0,0,0,0,0,0},
{0x3f,0x3f,0x3f,0,0,0,0,0,0},
{0x3f,0x3f,0x7f,0,0,0,0,0,0},
{0x3f,0x3f,0xbf,0,0,0,0,0,0},
{0x3f,0x3f,0xff,0,0,0,0,0,0},
{0x3f,0x7f,0x00,0,0,0,0,0,0},
{0x3f,0x7f,0x3f,0,0,0,0,0,0},
{0x3f,0x7f,0x7f,0,0,0,0,0,0},
{0x3f,0x7f,0xbf,0,0,0,0,0,0},
{0x3f,0x7f,0xff,0,0,0,0,0,0},
{0x3f,0xbf,0x00,0,0,0,0,0,0},
{0x3f,0xbf,0x3f,0,0,0,0,0,0},
{0x3f,0xbf,0x7f,0,0,0,0,0,0},
{0x3f,0xbf,0xbf,0,0,0,0,0,0},
{0x3f,0xbf,0xff,0,0,0,0,0,0},
{0x3f,0xff,0x00,0,0,0,0,0,0},
{0x3f,0xff,0x3f,0,0,0,0,0,0},
{0x3f,0xff,0x7f,0,0,0,0,0,0},
{0x3f,0xff,0xbf,0,0,0,0,0,0},
{0x3f,0xff,0xff,0,0,0,0,0,0},
{0x7f,0x00,0x00,0,0,0,0,0,0},
{0x7f,0x00,0x3f,0,0,0,0,0,0},
{0x7f,0x00,0x7f,0,0,0,0,0,0},
{0x7f,0x00,0xbf,0,0,0,0,0,0},
{0x7f,0x00,0xff,0,0,0,0,0,0},
{0x7f,0x3f,0x00,0,0,0,0,0,0},
{0x7f,0x3f,0x3f,0,0,0,0,0,0},
{0x7f,0x3f,0x7f,0,0,0,0,0,0},
{0x7f,0x3f,0xbf,0,0,0,0,0,0},
{0x7f,0x3f,0xff,0,0,0,0,0,0},
{0x7f,0x7f,0x00,0,0,0,0,0,0},
{0x7f,0x7f,0x3f,0,0,0,0,0,0},
{0x7f,0x7f,0x7f,0,0,0,0,0,0},
{0x7f,0x7f,0xbf,0,0,0,0,0,0},
{0x7f,0x7f,0xff,0,0,0,0,0,0},
{0x7f,0xbf,0x00,0,0,0,0,0,0},
{0x7f,0xbf,0x3f,0,0,0,0,0,0},
{0x7f,0xbf,0x7f,0,0,0,0,0,0},
{0x7f,0xbf,0xbf,0,0,0,0,0,0},
{0x7f,0xbf,0xff,0,0,0,0,0,0},
{0x7f,0xff,0x00,0,0,0,0,0,0},
{0x7f,0xff,0x3f,0,0,0,0,0,0},
{0x7f,0xff,0x7f,0,0,0,0,0,0},
{0x7f,0xff,0xbf,0,0,0,0,0,0},
{0x7f,0xff,0xff,0,0,0,0,0,0},
{0xbf,0x00,0x00,0,0,0,0,0,0},
{0xbf,0x00,0x3f,0,0,0,0,0,0},
{0xbf,0x00,0x7f,0,0,0,0,0,0},
{0xbf,0x00,0xbf,0,0,0,0,0,0},
{0xbf,0x00,0xff,0,0,0,0,0,0},
{0xbf,0x3f,0x00,0,0,0,0,0,0},
{0xbf,0x3f,0x3f,0,0,0,0,0,0},
{0xbf,0x3f,0x7f,0,0,0,0,0,0},
{0xbf,0x3f,0xbf,0,0,0,0,0,0},
{0xbf,0x3f,0xff,0,0,0,0,0,0},
{0xbf,0x7f,0x00,0,0,0,0,0,0},
{0xbf,0x7f,0x3f,0,0,0,0,0,0},
{0xbf,0x7f,0x7f,0,0,0,0,0,0},
{0xbf,0x7f,0xbf,0,0,0,0,0,0},
{0xbf,0x7f,0xff,0,0,0,0,0,0},
{0xbf,0xbf,0x00,0,0,0,0,0,0},
{0xbf,0xbf,0x3f,0,0,0,0,0,0},
{0xbf,0xbf,0x7f,0,0,0,0,0,0},
{0xbf,0xbf,0xbf,0,0,0,0,0,0},
{0xbf,0xbf,0xff,0,0,0,0,0,0},
{0xbf,0xff,0x00,0,0,0,0,0,0},
{0xbf,0xff,0x3f,0,0,0,0,0,0},
{0xbf,0xff,0x7f,0,0,0,0,0,0},
{0xbf,0xff,0xbf,0,0,0,0,0,0},
{0xbf,0xff,0xff,0,0,0,0,0,0},
{0xff,0x00,0x00,0,0,0,0,0,0},
{0xff,0x00,0x3f,0,0,0,0,0,0},
{0xff,0x00,0x7f,0,0,0,0,0,0},
{0xff,0x00,0xbf,0,0,0,0,0,0},
{0xff,0x00,0xff,0,0,0,0,0,0},
{0xff,0x3f,0x00,0,0,0,0,0,0},
{0xff,0x3f,0x3f,0,0,0,0,0,0},
{0xff,0x3f,0x7f,0,0,0,0,0,0},
{0xff,0x3f,0xbf,0,0,0,0,0,0},
{0xff,0x3f,0xff,0,0,0,0,0,0},
{0xff,0x7f,0x00,0,0,0,0,0,0},
{0xff,0x7f,0x3f,0,0,0,0,0,0},
{0xff,0x7f,0x7f,0,0,0,0,0,0},
{0xff,0x7f,0xbf,0,0,0,0,0,0},
{0xff,0x7f,0xff,0,0,0,0,0,0},
{0xff,0xbf,0x00,0,0,0,0,0,0},
{0xff,0xbf,0x3f,0,0,0,0,0,0},
{0xff,0xbf,0x7f,0,0,0,0,0,0},
{0xff,0xbf,0xbf,0,0,0,0,0,0},
{0xff,0xbf,0xff,0,0,0,0,0,0},
{0xff,0xff,0x00,0,0,0,0,0,0},
{0xff,0xff,0x3f,0,0,0,0,0,0},
{0xff,0xff,0x7f,0,0,0,0,0,0},
{0xff,0xff,0xbf,0,0,0,0,0,0},
{0xff,0xff,0xff,0,0,0,0,0,0},
{0x1f,0x1f,0x1f,0,0,0,0,0,0},
{0x1f,0x1f,0x5f,0,0,0,0,0,0},
{0x1f,0x1f,0x9f,0,0,0,0,0,0},
{0x1f,0x1f,0xdf,0,0,0,0,0,0},
{0x1f,0x5f,0x1f,0,0,0,0,0,0},
{0x1f,0x5f,0x5f,0,0,0,0,0,0},
{0x1f,0x5f,0x9f,0,0,0,0,0,0},
{0x1f,0x5f,0xdf,0,0,0,0,0,0},
{0x1f,0x9f,0x1f,0,0,0,0,0,0},
{0x1f,0x9f,0x5f,0,0,0,0,0,0},
{0x1f,0x9f,0x9f,0,0,0,0,0,0},
{0x1f,0x9f,0xdf,0,0,0,0,0,0},
{0x1f,0xdf,0x1f,0,0,0,0,0,0},
{0x1f,0xdf,0x5f,0,0,0,0,0,0},
{0x1f,0xdf,0x9f,0,0,0,0,0,0},
{0x1f,0xdf,0xdf,0,0,0,0,0,0},
{0x5f,0x1f,0x1f,0,0,0,0,0,0},
{0x5f,0x1f,0x5f,0,0,0,0,0,0},
{0x5f,0x1f,0x9f,0,0,0,0,0,0},
{0x5f,0x1f,0xdf,0,0,0,0,0,0},
{0x5f,0x5f,0x1f,0,0,0,0,0,0},
{0x5f,0x5f,0x5f,0,0,0,0,0,0},
{0x5f,0x5f,0x9f,0,0,0,0,0,0},
{0x5f,0x5f,0xdf,0,0,0,0,0,0},
{0x5f,0x9f,0x1f,0,0,0,0,0,0},
{0x5f,0x9f,0x5f,0,0,0,0,0,0},
{0x5f,0x9f,0x9f,0,0,0,0,0,0},
{0x5f,0x9f,0xdf,0,0,0,0,0,0},
{0x5f,0xdf,0x1f,0,0,0,0,0,0},
{0x5f,0xdf,0x5f,0,0,0,0,0,0},
{0x5f,0xdf,0x9f,0,0,0,0,0,0},
{0x5f,0xdf,0xdf,0,0,0,0,0,0},
{0x9f,0x1f,0x1f,0,0,0,0,0,0},
{0x9f,0x1f,0x5f,0,0,0,0,0,0},
{0x9f,0x1f,0x9f,0,0,0,0,0,0},
{0x9f,0x1f,0xdf,0,0,0,0,0,0},
{0x9f,0x5f,0x1f,0,0,0,0,0,0},
{0x9f,0x5f,0x5f,0,0,0,0,0,0},
{0x9f,0x5f,0x9f,0,0,0,0,0,0},
{0x9f,0x5f,0xdf,0,0,0,0,0,0},
{0x9f,0x9f,0x1f,0,0,0,0,0,0},
{0x9f,0x9f,0x5f,0,0,0,0,0,0},
{0x9f,0x9f,0x9f,0,0,0,0,0,0},
{0x9f,0x9f,0xdf,0,0,0,0,0,0},
{0x9f,0xdf,0x1f,0,0,0,0,0,0},
{0x9f,0xdf,0x5f,0,0,0,0,0,0},
{0x9f,0xdf,0x9f,0,0,0,0,0,0},
{0x9f,0xdf,0xdf,0,0,0,0,0,0},
{0xdf,0x1f,0x1f,0,0,0,0,0,0},
{0xdf,0x1f,0x5f,0,0,0,0,0,0},
{0xdf,0x1f,0x9f,0,0,0,0,0,0},
{0xdf,0x1f,0xdf,0,0,0,0,0,0},
{0xdf,0x5f,0x1f,0,0,0,0,0,0},
{0xdf,0x5f,0x5f,0,0,0,0,0,0},
{0xdf,0x5f,0x9f,0,0,0,0,0,0},
{0xdf,0x5f,0xdf,0,0,0,0,0,0},
{0xdf,0x9f,0x1f,0,0,0,0,0,0},
{0xdf,0x9f,0x5f,0,0,0,0,0,0},
{0xdf,0x9f,0x9f,0,0,0,0,0,0},
{0xdf,0x9f,0xdf,0,0,0,0,0,0},
{0xdf,0xdf,0x1f,0,0,0,0,0,0},
{0xdf,0xdf,0x5f,0,0,0,0,0,0},
{0xdf,0xdf,0x9f,0,0,0,0,0,0},
{0xdf,0xdf,0xdf,0,0,0,0,0,0}
};

void AllocCMap(struct Screen *Scr)
{
	AllocColors(pal,htc_NumCols,Scr);
}

void FreeCMap(struct Screen *Scr)
{
	FreeColors(pal,htc_NumCols,Scr);
}
