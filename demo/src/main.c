#include "genesis.h"
#include "gske.h"
#include "music.h"

#include "texture.h"
#include "rotozoom.h"
#include "rotozoom_back.h"
#include "fade.h"

#include "gouletex.h"
#include "goule.h"

#include "recording.h"
#include "girl.h"

#include "eat_1.h"
#include "eat_2.h"
#include "eat_3.h"
#include "eat_4.h"
#include "eat_5.h"
#include "eat_6.h"

#include "floor_plane.h"
#include "floor_text.h"

#include "credits_1.h"
#include "credits_2.h"
#include "credits_3.h"

#include "logo.h"
#include "logo_text.h"
#include "logo_sega.h"
#include "island.h"

#include "movie.h"
#include "finalflag.h"
#include "mrgreen.h"
#include "mrblue.h"

void Clear()
{
    memset(GSKE_FrameBuffer, 0, GSKE_Width*GSKE_Height);
    GSKE_update(0);
    memset(GSKE_FrameBuffer, 0, GSKE_Width*GSKE_Height);
    GSKE_update(0);
}

u16 pal[16];
inline void BeginPaletteRoll()
{
    VDP_getPalette(PAL0, pal);
}

inline void RollPalette()
{
    u16 y;
    u16 o = pal[1];
    for (y=1 ; y<15 ; ++y)
        pal[y] = pal[y+1];
    pal[15] = o;
    VDP_setPalette(PAL0, pal);
}

u32 SND_getTime_PCM()
{
    Z80_requestBus(1);
    vu8 *pb = (u8*)(Z80_DRV_PARAMS+0x10);
    u8 a=pb[0];
    u8 b=pb[1];
    Z80_releaseBus();
    u32 cr = (u32)a<<8|(u32)b<<16;
    cr -= (u32)music;
    cr /= 16;
    return cr;
}

int main(u16 hard)
{
    u16* palette;
    int loop = 0;

    palette = (u16*)texture;    
begin:
    GSKE_init(hard);
    SND_startPlay_PCM(music, sizeof(music), SOUND_RATE_16000, SOUND_PAN_CENTER, 1);    
    
    // ************ SEGA ************
    {
        Clear();
        VDP_setPalette(PAL0, (u16*)&logo_sega[2]);
        u8* logo = (u8*)&logo_sega[18];
        u32 t=0;    
        while(SND_getTime_PCM()<3500)
        {
            GSKE_update(1);
            t+=6;
                
            u32 y;
            u32 s = t%128;
            for (y=0 ; y<56 ; ++y)
            {
                u32 off = sinFix16(y*40+t)*(y>s-6&&y<s+6?1:0)/10;
                memcpy(GSKE_FrameBuffer+(y+(GSKE_Height-56)/2)*GSKE_Width+(GSKE_Width-84)/2+off, logo+y*84, 84);
            }
            
            for (y=4 ; y<16 ; ++y)
            {
                u32 col = sinFix16(-y*60+t*10)/20;
                if (loop==0) col = (col/20)&15;
                else if (loop==1) col = ((col/20)&15)<<4;
                else col = ((col/20)&15)<<8;
                VDP_setPaletteColor(PAL0, y, col);
            }
            
            u32 c = (t%45)/12+2;
            VDP_setPaletteColor(PAL0, 3, c|(c<<4)|(c<<8));
        }
    }
    
    // ************ MEGADRIVE ************
    {
        Clear();
        VDP_setPalette(PAL0, (u16*)&palette[2]);
        u8* buffer = (u8*)&logo[18];
        u8* picture = (u8*)&logo_text[18];
        u32 t=0;    
        int px, py;
        u32 x, y;
        
        for (x=0 ; x<2 ; ++x)
        {
            GSKE_update(1);
            for (y=0 ; y<64 ; ++y)
                memcpy(GSKE_FrameBuffer+(y+(GSKE_Height-64)/2)*GSKE_Width+50, picture+y*64, 64);
        }
        
        while(SND_getTime_PCM()<7600)
        {
            GSKE_update(0);
            
            px = sinFix16(t)-32;
            py = cosFix16(t)-32;
            
            for (y=0 ; y<64 ; ++y)
            for (x=0 ; x<31 ; ++x)
            {
                int nx = (int)(buffer[y*32+x]&15)-(int)(buffer[y*32+x+1]&15);
                int dx = nx*px/8;
                if (dx<0) dx = 0;
                else if (dx>7) dx = 7;
                
                int ny = (int)(buffer[y*32+x]&15)-(int)(buffer[(y+1)*32+x]&15);
                int dy = ny*py/8;
                if (dy<0) dy = 0;
                else if (dy>7) dy = 7;
                
                GSKE_FrameBuffer[(y+(GSKE_Height-64)/2)*GSKE_Width+x+16] = dx|(dy)<<4;
            }
            
            t+=40;
        }
    }
    
    // ************ ZOMBIE! ************
    {
        u8* credits[6];
        credits[0] = (u8*)&credits_1[18];
        credits[1] = (u8*)&credits_2[18];
        credits[2] = (u8*)&credits_3[18];
        u8* buf = (u8*)MEM_alloc(BMP_WIDTH*BMP_HEIGHT/2);
        u32 i, j;
        u32 t=0;    
        for (j=0 ; j<BMP_HEIGHT/2 ; ++j)
            memcpy(buf+j*BMP_WIDTH, GSKE_FrameBuffer+j*BMP_WIDTH*2, BMP_WIDTH);
        while(1)
        {  
            u32 time = SND_getTime_PCM();
            GSKE_update(0);   
            for (j=0 ; j<800 ; ++j)
            {
                u32 x = random()%BMP_WIDTH;
                u32 y = (random()%(BMP_HEIGHT/2-1))+1;
                if (time>9000)
                {
                    u32 val1 = buf[(y-1)*BMP_WIDTH+x]&15;
                    u32 val2 = buf[(y-1)*BMP_WIDTH+x]>>4;
                    if (val1>0 && val1<8) val1+=8;
                    if (val2>0 && val2<8) val2+=8;
                    buf[y*BMP_WIDTH+x] = val1|(val2<<4);
                    if (y+1<BMP_HEIGHT/2)
                    {
                        // smooth linear effect
                        if (j%2==0) buf[(y+1)*BMP_WIDTH+x] = val1|(val1<<4);
                        else  buf[(y+1)*BMP_WIDTH+x] = val1;
                    }
                }
                else
                {
                    buf[y*BMP_WIDTH+x] = buf[(y-1)*BMP_WIDTH+x];
                }
            }
            for (j=0 ; j<180 ; ++j)
            {
                u32 x = (random()%(BMP_WIDTH-2))+1;
                u32 y = (random()%(BMP_HEIGHT/2-2))+1;
                u32 val = ((buf[y*BMP_WIDTH+x+1]&15)
                    + (buf[y*BMP_WIDTH+x-1]&15))/2;
                if (val>0 && val<8) val+=8;
                if (val>0 && val<15) ++val;
                buf[y*BMP_WIDTH+x] = val|(val<<4);
            }
            
            if (time>=9500 && time<=13500)
            {
                int offx = ((time-9500)*14/400-64);
                int offy = (time-9500)/160;
                if (offx>16) offx=16;
                if (offy>20) offy=20;
                int diff = 0;
                if (offx<0) diff = -offx;
                for (j=0 ; j<16 ; ++j)
                {
                    memcpy(buf+(j+offy)*BMP_WIDTH+offx+diff, credits[0]+j*64+diff, 64-diff);
                }
            }
            if (time>=11000 && time<=19300)
            {
                int offx = BMP_WIDTH-(time-11000)/90;
                int offy = BMP_HEIGHT/2-16-(time-11000)/600;
                int diff = 0;
                if (offx>BMP_WIDTH-64) diff = offx-(BMP_WIDTH-64);
                for (j=0 ; j<16 ; ++j)
                {
                    memcpy(buf+(j+offy)*BMP_WIDTH+offx, credits[1]+j*64, 64-diff);
                }
            }
            if (time>=25000 && time<=40000)
            {
                for (j=0 ; j<56 ; ++j)
                for (i=0 ; i<BMP_WIDTH ; ++i)
                {
                    u8 val = *(credits[2]+j*BMP_WIDTH+i);
                    if (val!=0)
                        buf[(j+5)*BMP_WIDTH+i] = val;
                }
            }
            if (time>45000)
                break;
            
            for (j=0 ; j<BMP_HEIGHT/2 ; ++j)
            {
                memcpy(GSKE_FrameBuffer+j*BMP_WIDTH*2, buf+j*BMP_WIDTH, BMP_WIDTH);
                memcpy(GSKE_FrameBuffer+j*BMP_WIDTH*2+BMP_WIDTH, buf+j*BMP_WIDTH, BMP_WIDTH);
            }
            ++t;
        }
        // Guitare transition of the death
        while(SND_getTime_PCM()<46500)
        {  
            GSKE_update(0);    
            u32 y;
            for (y=0 ; y<20 ; ++y)
                memset(GSKE_FrameBuffer+(random()%(BMP_HEIGHT-4))*BMP_WIDTH, 0, BMP_WIDTH*3);
        }
        MEM_free(buf);
    }
    
    // ************ SINUS DE LA GOULE ************
    {
        Clear();
        u8* tex = (u8*)&gouletex[18];
        u8* lagoule = (u8*)&goule[18];
        u32 t;  
        int time;        
        VDP_setPalette(PAL0, (u16*)&palette[2]);
        do
        {
            GSKE_update(1);
            time = SND_getTime_PCM()-46500;
            t = time/20;
            if (t<GSKE_Height)
            {
                memcpy(GSKE_FrameBuffer, lagoule, GSKE_Width*t);
                u32 y;
                for (y=t ; y<GSKE_Height ; ++y)
                    memcpy(GSKE_FrameBuffer+GSKE_Width*y, lagoule+GSKE_Width*t, GSKE_Width);
            }
            else
            {
                memcpy(GSKE_FrameBuffer, lagoule, GSKE_Width*GSKE_Height);
            }
        }
        while(time<4000);
        do
        {
            time = SND_getTime_PCM();
            t = (time-50500)/80;
            int s = GSKE_Width;
            int tits = 0;
            if (t*2<GSKE_Width)
                s = t*2;
            int yo=0;
            if (time>55700)
            {
                yo = ((time-55700)/1000)*20;
                tits = GSKE_Width-((time-55700)/125)%8*GSKE_Width/8;
                GSKE_update(1);
            }
            else
            {
                GSKE_update(0);
            }
            u32 y;
            if (tits>0 && yo>0 && yo<GSKE_Height+1)
            {
                if (yo>GSKE_Height) yo=GSKE_Height;
                for (y=yo-20 ; y<yo ; ++y)
                {
                    u32 scrolly = cosFix16(t*16+y*4)/2;
                    memcpy(GSKE_FrameBuffer+y*GSKE_Width, tex+(y*128+scrolly)%(128*128-s), tits);
                }
            }
            for (y=yo ; y<GSKE_Height ; ++y)
            {
                u32 scrolly = cosFix16(t*16+y*4)/2;
                memcpy(GSKE_FrameBuffer+y*GSKE_Width, tex+(y*128+scrolly)%(128*128-s), s);
            }
        }
        while(time<65700);
    }
    
    // ************ PICTURE SCROLLING ************
    {
        VDP_setPalette(PAL0, (u16*)&palette[2]);
        u8 *picture = (u8*)&girl[18];
        u8 *rec = (u8*)&recording[18];
        u32 time=0;
        u32 start = 65700;
        int tt=0;
        do
        {
            time=SND_getTime_PCM()-start;
            if (time>11000) GSKE_update(0);
            else GSKE_update(1);
            
            int offS, offB, size;
            int t = time/80;
            if (t<GSKE_Width)
            {
                size = t;
                offS = GSKE_Width-t;
                offB = 0;
            }
            else
            {
                size = GSKE_Width;
                offS = 0;
                offB = t-GSKE_Width;
                if (offB>=GSKE_Width)
                    offB = GSKE_Width-1;
            }
            
            int y;
            for (y=0 ; y<GSKE_Height ; ++y)
                memcpy(GSKE_FrameBuffer+y*GSKE_Width+offS, picture+y*GSKE_Width*2+offB, size);
            
            int j;
            for (j=0 ; j<60 ; ++j)
            {
                u32 x = random()%BMP_WIDTH;
                u32 y = random()%BMP_HEIGHT;
                GSKE_FrameBuffer[y*BMP_WIDTH+x] |= (random()%4+2)|(15<<4);
            }
            {
                u32 y = random()%BMP_HEIGHT;
                memset(GSKE_FrameBuffer+y*BMP_WIDTH, 0, BMP_WIDTH);
            }
            
            ++tt;
            if (time<20000)
            for (y=0 ; y<16 ; ++y)
                memcpy(GSKE_FrameBuffer+(y+4)*GSKE_Width+2, rec+y*32, 32);
                
            int w = (tt/10)%3;
            u32 a = 15|(15<<4)|(15<<8);
            u32 b=a;
            u32 c=a;
            u32 d=a;
            u32 max = 15;
            if (loop==1) max = 15<<4;
            else if (loop==2) max =15<<8;
            
            if (w==0) a = max;
            else
            {
                b = max;
                if (w==1) c = max;
                else d = max;
            }
            VDP_setPaletteColor(PAL0, 10, a);
            VDP_setPaletteColor(PAL0, 11, b);
            VDP_setPaletteColor(PAL0, 12, c);
            VDP_setPaletteColor(PAL0, 13, d);
        }
        while(time<22000);
    }
    
    // ************ FAST GUITARE ************
    {
        u32 time;
        u32 start = 87200;
        VDP_setPalette(PAL0, (u16*)&palette[2]);
        BeginPaletteRoll();
        do
        {  
            time=SND_getTime_PCM()-start;
            GSKE_update(0);        
            u32 x, y, j=time/10;
            u8* buffer = GSKE_FrameBuffer;
            
            u32 o = 0;
            if (time>9000)
            {
                o = (time-9000)*BMP_HEIGHT/12000;
                if (o>BMP_HEIGHT/4)
                    o = BMP_HEIGHT/4;
                memset(buffer, 0, o*BMP_WIDTH);
                memset(buffer+BMP_WIDTH*BMP_HEIGHT-o*BMP_WIDTH, 0, o*BMP_WIDTH);
                buffer += o*BMP_WIDTH;
            }
                
            for (y=o ; y<BMP_HEIGHT-o ; ++y)
            {
                u32 val = cosFix16(y+j*2)%16;
                val = val|(val<<4);
                for (x=0 ; x<BMP_WIDTH ; x+=2)
                {
                    *buffer = val+(x+j*8);
                    buffer+=2;
                }
            }       
                
            RollPalette();
        }
        while(time<12000);
    }
    
    // ************ AWESOME SINUS EFFECT ************
    {
        u32 time, x, y, j, start = 99200;
        do
        {  
            time=SND_getTime_PCM()-start;
            GSKE_update(1);        
            j=time/10;
                    
            u8* buffer = BMP_getWritePointer(0, BMP_HEIGHT/4);
            
            u32 o = 0;
            if (time>9000)
            {
                o = (time-9000)*BMP_WIDTH/6000;
                if (o>BMP_WIDTH/2)
                    o = BMP_WIDTH/2;
                buffer += o;
            }
            
            for (y=BMP_HEIGHT/4 ; y<BMP_HEIGHT/4+BMP_HEIGHT/2 ; y+=2)
            {
                u32 val = sinFix16(y*4+j);
                for (x=o ; x<BMP_WIDTH-o ; ++x)
                    *buffer++ = val+cosFix16(x*4+j);
                buffer+=BMP_WIDTH+o*2;
            }
            
            RollPalette();
        }
        while(time<12000);
    }    
    
    // 111200
    // ************ MEGA DEFORMATION ************
    {
        Clear();
        u8* tex = (u8*)&rotozoom[18];
        u8* back = (u8*)&rotozoom_back[18];
        int x, y;
        u32 time;
        VDP_setPalette(PAL0, (u16*)&palette[2]);
        do
        {
            GSKE_update(1);
            time = SND_getTime_PCM()-111200;
            int t = time/20;
            if (t<GSKE_Height)
            {
                memcpy(GSKE_FrameBuffer+(GSKE_Height-t)*GSKE_Width, back+(GSKE_Height-t)*GSKE_Width, GSKE_Width*t);
                u32 y;
                for (y=0 ; y<GSKE_Height-t ; ++y)
                    memcpy(GSKE_FrameBuffer+GSKE_Width*y, back+GSKE_Width*(GSKE_Height-t), GSKE_Width);
            }
            else
            {
                memcpy(GSKE_FrameBuffer, back, GSKE_Width*GSKE_Height);
            }
        }
        while(time<4000);
        
        do
        {
            GSKE_update(0);
            time=SND_getTime_PCM();
            u32 t=(time-115200)/3;
            
            u32 cosA = cosFix16(t);
            u32 sinA = sinFix16(t);
                
            int a = (GSKE_Width/2-20)*4;
            int o = 0;
            if (t<1000)
            {
                o = (1000-t)*a/4000;
            }
            for (y=12 ; y<GSKE_Height-12 ; y+=4)
            {
                for (x=GSKE_Width/2+12+o*4 ; x<GSKE_Width-8 ; x+=4)
                {
                    u32 scrollx = ((x-GSKE_Width*3/4)*cosA-(y-GSKE_Height/2)*sinA/2)/128+16;
                    u32 scrolly = ((x-GSKE_Width*3/4)*sinA*2+(y-GSKE_Height/2)*cosA)/128+32;
                    memset(GSKE_FrameBuffer+y*GSKE_Width+x, tex[(scrolly%64)*32+(scrollx%32)], 4);
                }
                memcpy(GSKE_FrameBuffer+(y+1)*GSKE_Width+GSKE_Width/2+12, GSKE_FrameBuffer+y*GSKE_Width+GSKE_Width/2+12, GSKE_Width/2-20);
                memcpy(GSKE_FrameBuffer+(y+2)*GSKE_Width+GSKE_Width/2+12, GSKE_FrameBuffer+y*GSKE_Width+GSKE_Width/2+12, GSKE_Width/2-20);
                memcpy(GSKE_FrameBuffer+(y+3)*GSKE_Width+GSKE_Width/2+12, GSKE_FrameBuffer+y*GSKE_Width+GSKE_Width/2+12, GSKE_Width/2-20);
            }
        }
        while(time<123200);
    }    
   
    // ************ SCREEN FADE ************
    {   
        u8* fadeTex = (u8*)&fade[18];
        u32 x, y;
        int time;
        do
        {  
            time = SND_getTime_PCM();
            GSKE_update(0);

            int p = (time-123200)*15/4000;
            p = p|(p<<4);
            for (y=0 ; y<GSKE_Height/8 ; ++y)
            for (x=0 ; x<GSKE_Width/8 ; ++x)
            {
                if (fadeTex[y*GSKE_Width/8+x]<=p)
                {
                    int k;
                    for (k=0 ; k<8 ; ++k)
                        memset(GSKE_FrameBuffer+(y*8+k)*GSKE_Width+x*8, 0, 8);
                }
            }
        }
        while(time<127200);
    }
    
    // 127200
    // ************ SCROLLING PLANES ************
    {
        u8* picture = (u8*)&floor_plane[18];
        u8* text = (u8*)&floor_text[18];
        Clear();
        VDP_setPalette(PAL0, (u16*)&palette[2]);
        int x, y;
        u8* buf = (u8*)MEM_alloc(BMP_WIDTH*BMP_HEIGHT/2);
        for (y=0 ; y<BMP_HEIGHT ; ++y)
        {
            int div = y*y+1;
            for (x=0 ; x<BMP_WIDTH/2 ; ++x)
                buf[y*(BMP_WIDTH/2)+x] = (x*2048*8/div)%64;
        }
        int to = BMP_WIDTH/2;
        u32 time;
        
        do
        {  
            time=SND_getTime_PCM()-127200;
            u32 j=time/30;
            int end=0;
            if (time<1000)
            {
                end = BMP_WIDTH/2;
            }
            else if (time<4000)
            {
                GSKE_update(1);
                end = (BMP_WIDTH/2)*(4000-time)/3000;
            }
            else GSKE_update(0);
            
            u8* buffer = BMP_getWritePointer(BMP_WIDTH/2, BMP_HEIGHT/2);
            u8* bu = BMP_getWritePointer(0, 0);
            for (y=BMP_HEIGHT/2 ; y<BMP_HEIGHT ; ++y)
            {
                int div = y*y+1;
                u8 val = ((y)*1600*20/div+j)%128;
                for (x=BMP_WIDTH/2 ; x<BMP_WIDTH-end ; x++)
                {
                    u32 v = picture[val*64+(u32)buf[y*to+x-to]];
                    *(buffer-(x-to)*2) = v;
                    *buffer++ = v;
                }
                memcpy(bu+(BMP_HEIGHT-y)*BMP_WIDTH, bu+y*BMP_WIDTH, BMP_WIDTH);
                buffer += BMP_WIDTH/2+end;
            }
            
            u32 size = 0;
            if (j>50)
            {
                if ((j-50)/2<64) size = (j-50)/2;
                else if (j>250) { if ((j-250)/2>64) size=0; else size = 64-(j-250)/2; }
                else size = 64;
            }
            for (y=0 ; y<32 ; ++y)
                memcpy(GSKE_FrameBuffer+(y+BMP_HEIGHT-48)*BMP_WIDTH, text+y*64, size);
        }
        while(time<12000);
        MEM_free(buf);
    }
    
    // ************ BLOOD ************
    {
        u8* buf = (u8*)MEM_alloc(BMP_WIDTH*BMP_HEIGHT/2);
        u32 j, t=0;  
        for (j=0 ; j<BMP_HEIGHT/2 ; ++j)
            memcpy(buf+j*BMP_WIDTH, GSKE_FrameBuffer+j*BMP_WIDTH*2, BMP_WIDTH);
        while(SND_getTime_PCM()<142200)
        {  
            GSKE_update(0);   
            for (j=0 ; j<800 ; ++j)
            {
                u32 x = random()%BMP_WIDTH;
                u32 y = (random()%(BMP_HEIGHT/2-1))+1;
                u32 val1 = buf[(y-1)*BMP_WIDTH+x]&15;
                u32 val2 = buf[(y-1)*BMP_WIDTH+x]>>4;
                if (val1>0 && val1<8) val1+=8;
                if (val2>0 && val2<8) val2+=8;
                buf[y*BMP_WIDTH+x] = val1|(val2<<4);
            }
            for (j=0 ; j<180 ; ++j)
            {
                u32 x = (random()%(BMP_WIDTH-2))+1;
                u32 y = (random()%(BMP_HEIGHT/2-2))+1;
                u32 val = ((buf[y*BMP_WIDTH+x+1]&15)
                    + (buf[y*BMP_WIDTH+x-1]&15))/2;
                if (val>0 && val<8) val+=8;
                if (val>0 && val<15) ++val;
                buf[y*BMP_WIDTH+x] = val|(val<<4);
            }
            for (j=0 ; j<BMP_HEIGHT/2 ; ++j)
            {
                memcpy(GSKE_FrameBuffer+j*BMP_WIDTH*2, buf+j*BMP_WIDTH, BMP_WIDTH);
                memcpy(GSKE_FrameBuffer+j*BMP_WIDTH*2+BMP_WIDTH, buf+j*BMP_WIDTH, BMP_WIDTH);
            }
            ++t;
        }
        MEM_free(buf);
        while(SND_getTime_PCM()<143200)
        {  
            GSKE_update(0);    
            u32 y;
            for (y=0 ; y<20 ; ++y)
                memset(GSKE_FrameBuffer+(random()%(BMP_HEIGHT-7))*BMP_WIDTH, 0, BMP_WIDTH*6);
        }
    }
    
    // 143200
    // ************ CUBE VIDEO ************
    {
        Clear();
        GSKE_Mesh* cube = GSKE_GenerateCube();
        cube->texture = (u8*)&texture[18];
        cube->material = 0;
        u16 cubeId = GSKE_AddMesh(cube);
        VDP_setPalette(PAL0, (u16*)&palette[2]);
        
        u8* picture[6];
        picture[0] = (u8*)&eat_1[18];
        picture[1] = (u8*)&eat_2[18];
        picture[2] = (u8*)&eat_3[18];
        picture[3] = (u8*)&eat_4[18];
        picture[4] = (u8*)&eat_5[18];
        picture[5] = (u8*)&eat_6[18];
        u32 time;
        int tt=0;
        
        do
        {
            time=SND_getTime_PCM()-143200;
            u32 j=time/20;            
        
            GSKE_update(0);
            memcpy(GSKE_FrameBuffer, picture[(j/8)%6], GSKE_Width*GSKE_Height);
        
            cube->translation.x = FIX16(-1.5f);
            cube->translation.y = FIX16(-0.5f);
            cube->translation.z = FIX16(3)+sinFix16(j*4);
            cube->rotation.x = cosFix16(j*2)*100;
            cube->rotation.y = sinFix16(j*4)*100;
            cube->rotation.z = cosFix16(j*8)*100;
            GSKE_UpdateScene();
            
            cube->material = sinFix16(j*4)>0?2:0;
            
            if (time>17000)
            {
                if (tt%2==0) VDP_setPalette(PAL0, (u16*)&palette[2]);
                else VDP_setPalette(PAL0, (u16*)&island[2]);
                ++tt;
            }
        }
        while(time<20000);
        GSKE_RemoveMesh(cubeId);
    }
    
    //161200
    // ************ THE ISLAND ************
    {
        Clear();
        Clear();
        Clear();
        Clear();
        GSKE_Scene.maxFov = 1;
        VDP_setPalette(PAL0, (u16*)&island[2]);
        
        GSKE_Mesh* islandMesh = GSKE_CreateMesh();
        const u16 size = 9;
        islandMesh->vertex_count = size*size;
        islandMesh->vertex_positions = (Vect3D_f16*)MEM_alloc(islandMesh->vertex_count*sizeof(Vect3D_f16));
        islandMesh->vertex_colors = (u8*)MEM_alloc(islandMesh->vertex_count);
        islandMesh->face_count = (size-1)*(size-1)*2;
        islandMesh->face_indices = (u16*)MEM_alloc(islandMesh->face_count*3*sizeof(u16));
        islandMesh->material = 1;
        
        u16 i,j;
        u16 fi=0;
        for (j=0 ; j<size ; ++j)
        {
            for (i=0 ; i<size ; ++i)
            {
                islandMesh->vertex_positions[j*size+i].x = intToFix16(i-(size/2))*3;
                islandMesh->vertex_positions[j*size+i].y = 0;
                islandMesh->vertex_positions[j*size+i].z = intToFix16(j-(size/2))*3;
                islandMesh->vertex_colors[j*size+i] = 1;            
                
                if (i<size-1 && j<size-1)
                {
                    islandMesh->face_indices[fi++] = j*size+i;
                    islandMesh->face_indices[fi++] = j*size+i+1;
                    islandMesh->face_indices[fi++] = j*size+i+size+1;
                    islandMesh->face_indices[fi++] = j*size+i;
                    islandMesh->face_indices[fi++] = j*size+i+size+1;
                    islandMesh->face_indices[fi++] = j*size+i+size;                
                }
            }
        }
        
        u16 islandId = GSKE_AddMesh(islandMesh);
        u16 tt=0;
        u32 time;
        for (time=0 ; time<2 ; ++time)
        {
            memset(GSKE_FrameBuffer, 2|(2<<4), GSKE_Width*72);
            memset(GSKE_FrameBuffer+GSKE_Width*72, 1|(1<<4), GSKE_Width*(GSKE_Height-72));
            for (tt=0 ; tt<32 ; ++tt)
                memcpy(GSKE_FrameBuffer+(tt+0)*GSKE_Width+0, ((u8*)&island[18])+tt*64, 64);
            GSKE_update(0);
        }
        tt=0;
        do
        {
            time = SND_getTime_PCM()-161200;
            GSKE_update(0);
            GSKE_UpdateScene();
        
            GSKE_Scene.camera.translation.x = FIX16(-1);
            GSKE_Scene.camera.translation.z = FIX16(15);
            GSKE_Scene.camera.translation.y = FIX16(2);
            GSKE_Scene.camera.rotation.x = FIX16(80+340+sinFix16(tt*14)/4);
            GSKE_Scene.camera.rotation.y = FIX16(0);
            GSKE_Scene.camera.rotation.z = FIX16(0);
            
            islandMesh->rotation.y = FIX16(512+tt*14);
            
            ++tt;
            for (j=0 ; j<size ; ++j)
            {
                for (i=0 ; i<size ; ++i)
                {
                    int h;
                    if (j<size-1) h = fix16ToInt(islandMesh->vertex_positions[(j+1)*size+i].y)*20;
                    else
                    {
                        if (i>0 && i<size-1) h = (random()%200+fix16ToInt(islandMesh->vertex_positions[(j-1)*size+i].y)*16)/2;
                        else h = 0;
                    }
                    
                    islandMesh->vertex_positions[j*size+i].y = intToFix16(h/20);
                    if (h>119) h=119;
                    islandMesh->vertex_colors[j*size+i] = h/8+1;
                }
            }
        }
        while(time<40000);
        GSKE_RemoveMesh(islandId);
    }
    
    // ************ SIMPLE TRANSITION ************
    {   
        u32 y;
        while(SND_getTime_PCM()<202200)
        {  
            GSKE_update(0);    
            for (y=0 ; y<20 ; ++y)
                memset(GSKE_FrameBuffer+(random()%(BMP_HEIGHT-4))*BMP_WIDTH, 0, BMP_WIDTH*3);
        }
    }
    
    //202200
    // ************ MOVIE PICTURE ************
    {
        u8* picture = (u8*)&movie[18];
        Clear();
        VDP_setPalette(PAL0, (u16*)&palette[2]);
        u32 time;
        do
        {  
            GSKE_update(0);
            time=SND_getTime_PCM()-202200;
            // Flickering ftw!
            GSKE_update(0);
            memcpy(GSKE_FrameBuffer, picture, GSKE_Width*GSKE_Height);
        }
        while (time<14000);
    }
    
    // 216200
    // ************ FINAL FLAG ************
    {
        Clear();
        GSKE_Scene.maxFov = 0;
        VDP_setPalette(PAL0, (u16*)&palette[2]);
        
        GSKE_Mesh* flag = GSKE_CreateMesh();
        const u16 size = 4;
        flag->vertex_count = size*size;
        flag->vertex_positions = (Vect3D_f16*)MEM_alloc(flag->vertex_count*sizeof(Vect3D_f16));
        flag->vertex_texcoords = (Vect2D_s16*)MEM_alloc(flag->vertex_count*sizeof(Vect2D_s16));
        flag->face_count = (size-1)*(size-1)*2;
        flag->face_indices = (u16*)MEM_alloc(flag->face_count*3*sizeof(u16));
        flag->material = 2;
        flag->texture = (u8*)&finalflag[18];
            
        u16 i,j;
        u16 fi=0;
        for (j=0 ; j<size ; ++j)
        {
            for (i=0 ; i<size ; ++i)
            {
                flag->vertex_positions[j*size+i].x = intToFix16(i-(size/2))*3;
                flag->vertex_positions[j*size+i].y = 0;
                flag->vertex_positions[j*size+i].z = intToFix16(j-(size/2))*3;
                flag->vertex_texcoords[j*size+i].x = i*44;            
                flag->vertex_texcoords[j*size+i].y = j*43;            
                
                if (i<size-1 && j<size-1)
                {
                    flag->face_indices[fi++] = j*size+i;
                    flag->face_indices[fi++] = j*size+i+1;
                    flag->face_indices[fi++] = j*size+i+size+1;
                    flag->face_indices[fi++] = j*size+i;
                    flag->face_indices[fi++] = j*size+i+size+1;
                    flag->face_indices[fi++] = j*size+i+size;                
                }
            }
        }
        
        u16 flagId = GSKE_AddMesh(flag);
        u16 tt=0;
        int time;
        
        do
        {
            time = SND_getTime_PCM();
            
            GSKE_Scene.camera.translation.x = FIX16(1);
            GSKE_Scene.camera.translation.z = FIX16(0);
            GSKE_Scene.camera.translation.y = FIX16(1);
            GSKE_Scene.camera.rotation.x = FIX16(0);
            GSKE_Scene.camera.rotation.y = FIX16(0);
            GSKE_Scene.camera.rotation.z = FIX16(0);
            
            flag->translation.z = FIX16(12);
            flag->rotation.x = FIX16(256);
            flag->rotation.y = FIX16(80+sinFix16(tt*10)/4);
            
            ++tt;
            for (j=0 ; j<size ; ++j)
            {
                for (i=0 ; i<size ; ++i)
                {
                    int h;
                    if (i<size-1) h = fix16ToInt(flag->vertex_positions[j*size+i+1].y)*20;
                    else
                    {
                        if (j>0 && j<size-1) h = (random()%140+fix16ToInt(flag->vertex_positions[(j-1)*size+i].y)*20
                            +fix16ToInt(flag->vertex_positions[(j+1)*size+i].y)*20)/3;
                        else h = (random()%180)/3;
                    }                
                    flag->vertex_positions[j*size+i].y = intToFix16(h/20);
                }
            }
            GSKE_update(1);
            GSKE_UpdateScene();
            
            if (time<237200)
                memset(GSKE_FrameBuffer+GSKE_Width*(GSKE_Height-4), 8|(8<<4), (time-216200)*(GSKE_Width-1)/(237200-216200));
        }
        while(time<237200);
        GSKE_RemoveMesh(flagId);
    }
    
    palette = (u16*)mrgreen;
    ++loop;
    if (loop==2)
        palette = (u16*)mrblue;
    if (loop==3)
    {
        palette = (u16*)texture;
        loop = 0;
    }
    GSKE_exit();
    SND_stopPlay_PCM();
    goto begin; 
    while(1)
    {
        GSKE_update(1);
    }
    return 0;
}
