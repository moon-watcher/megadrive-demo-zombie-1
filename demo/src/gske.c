#include "gske.h"

u8* GSKE_FrameBuffer;
u16 GSKE_Width;
u16 GSKE_Height;
u16 GSKE_HLineOffsets[256]; // speed up the rastering

void GSKE_init(u16 hard)
{
    // handle reset
    if (hard==0)
    {
        GSKE_DestroyScene();
        // note: sometimes the cube disappear after reseting, it's an enjoyable bug so I kept it! <3
    }
    
    GSKE_FrameBuffer = 0;
    VDP_init();
    VDP_setHInterrupt(0);
    VDP_setHilightShadow(0);
    VDP_setScreenWidth320(); 
    BMP_init(BMP_ENABLE_ASYNCFLIP | BMP_ENABLE_EXTENDEDBLANK /*| BMP_ENABLE_FPSDISPLAY*/);   // fps.. what for..
    GSKE_Width = BMP_WIDTH;
    GSKE_Height = BMP_HEIGHT;
    
    u16 i;
    for (i=0 ; i<BMP_HEIGHT ; ++i)
        GSKE_HLineOffsets[i] = i*GSKE_Width;
        
    M3D_reset();
    M3D_setViewport(GSKE_Width, GSKE_Height);    
    GSKE_CreateScene();    
}

void GSKE_exit()
{
    GSKE_DestroyScene();
}

void GSKE_update(u8 clear)
{
    BMP_flip();
    if (clear!=0)
        BMP_clear();
    GSKE_FrameBuffer = BMP_getWritePointer(0, 0);
}
