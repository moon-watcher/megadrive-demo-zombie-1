#ifndef _GSKE_H_
#define _GSKE_H_

#include "genesis.h"

extern u8* GSKE_FrameBuffer; // <3 <3 <3
extern u16 GSKE_Width;
extern u16 GSKE_Height;
extern u16 GSKE_HLineOffsets[256];

void GSKE_init(u16 hard);
void GSKE_exit();
void GSKE_update(u8 clear);

#include "sort.h"
#include "raster.h"
#include "scene.h"

#endif
