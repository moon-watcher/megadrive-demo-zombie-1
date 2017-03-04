#include "raster.h"
#include "gske.h"

inline void GSKE_xchg(long* a, long* b)
{
    long t = *a;
    *a = *b;
    *b = t;
}

inline void GSKE_xchg8(u8* a, u8* b)
{
    u8 t = *a;
    *a = *b;
    *b = t;
}

inline void GSKE_DrawHLineFlat(long x1, long x2, long y, u8 color)
{
    if (x1==x2) return;
    if (x1>x2) GSKE_xchg(&x1, &x2);
    if (x1>=GSKE_Width || x2<0) return;
    if (x1<0) x1 = 0;
    if (x2>=GSKE_Width) x2 = GSKE_Width-1;
    
    const long start = x1+GSKE_HLineOffsets[y];
    const long end = x2+GSKE_HLineOffsets[y];
    long p;
    // awesome blend! <3
    for (p=start ; p<end ; ++p)
        GSKE_FrameBuffer[p] += color;
    //memset(GSKE_FrameBuffer+start, color, end-start);
}

void GSKE_DrawTriangleFlat(Vect2D_s16* v1, Vect2D_s16* v2, Vect2D_s16* v3, u8 color)
{
    // dot the backface poulet!
    if ((v3->x-v1->x)*(v2->y-v1->y) > (v3->y-v1->y)*(v2->x-v1->x))
        return;
    
    color = color|(color<<4);
    Vect2D_s16* p1 = v1;
    Vect2D_s16* p2 = v2;
    Vect2D_s16* p3 = v3;
    Vect2D_s16* t;
    
    if (p2->y>p3->y) { p2 = v3; p3 = v2; }
    if (p1->y>p3->y) { p1 = p3; p3 = v1; }
    if (p1->y>p2->y) { t = p1; p1 = p2; p2 = t; }
    if (p3->y==p1->y) return;

    long ix1 = (p3->x-p1->x)*65536/(p3->y-p1->y);

    if (p2->y!=p1->y)
    {
        long ix2 = (p2->x-p1->x)*65536/(p2->y-p1->y);
        long dx1 = p1->x*65536;
        long dx2 = dx1;
        long ly;
        for (ly=p1->y ; ly<p2->y ; ++ly)
        {
            if (ly>=0 && ly<GSKE_Height)
                GSKE_DrawHLineFlat(dx1/65536, dx2/65536, ly, color);
            dx1 += ix1;
            dx2 += ix2;
        }
    }

    if (p3->y!=p2->y)
    {
        long ix2 = (p3->x-p2->x)*65536/(p3->y-p2->y);
        long dx1 = p1->x*65536+((p2->y-p1->y)*ix1);
        long dx2 = p2->x*65536;
        long ly;
        for (ly=p2->y ; ly<p3->y ; ++ly)
        {
            if (ly>=0 && ly<GSKE_Height)
                GSKE_DrawHLineFlat(dx1/65536, dx2/65536, ly, color);
            dx1 += ix1;
            dx2 += ix2;
        }
    }
}

inline void GSKE_DrawHLineGouraud(long x1, long c1, long x2, long c2, long y)
{
    if (x1==x2) return;
    if (x1>x2) { GSKE_xchg(&x1, &x2); GSKE_xchg(&c1, &c2); }
    if (x1>GSKE_Width || x2<0) return;
    long ic = (c2-c1)/(x2-x1); 
    if (x1<0) { c1 += -x1*ic; x1 = 0; }
    if (x2>GSKE_Width) x2 = GSKE_Width;
    const long start = x1+GSKE_HLineOffsets[y];
    const long end = x2+GSKE_HLineOffsets[y];
    long p;
    // awesome interpolation!!! <3
    for (p=start ; p<end ; ++p)
    {
        GSKE_FrameBuffer[p] = c1/65536|((c1/65536)<<4);
        c1 += ic;
    }
}

void GSKE_DrawTriangleGouraud(Vect2D_s16* v1, u8 c1, Vect2D_s16* v2, u8 c2, Vect2D_s16* v3, u8 c3)
{
    // Mr Dot, the product
    if ((v3->x-v1->x)*(v2->y-v1->y) > (v3->y-v1->y)*(v2->x-v1->x))
        return;
    
    Vect2D_s16* p1 = v1;
    Vect2D_s16* p2 = v2;
    Vect2D_s16* p3 = v3;
    Vect2D_s16* t;
    
    if (p2->y>p3->y) { p2 = v3; p3 = v2; GSKE_xchg8(&c2, &c3); }
    if (p1->y>p3->y) { p1 = p3; p3 = v1; GSKE_xchg8(&c1, &c3); }
    if (p1->y>p2->y) { t = p1; p1 = p2; p2 = t; GSKE_xchg8(&c1, &c2); }
    if (p3->y==p1->y) return;

    long ix1 = (p3->x-p1->x)*65536/(p3->y-p1->y);
    long ic1 = (c3-c1)*65536/(p3->y-p1->y);
    
    if (p2->y!=p1->y)
    {
        long ix2 = (p2->x-p1->x)*65536/(p2->y-p1->y);
        long ic2 = (c2-c1)*65536/(p2->y-p1->y);
        long dx1 = p1->x*65536;
        long dxc1 = c1*65536;
        long dx2 = dx1;
        long dxc2 = dxc1;
        long ly;
        for (ly=p1->y ; ly<p2->y ; ++ly)
        {
            if (ly>=0 && ly<GSKE_Height) // && ly%4==0) <- awesome test
                GSKE_DrawHLineGouraud(dx1/65536, dxc1, dx2/65536, dxc2, ly);
            dx1 += ix1;
            dx2 += ix2;
            dxc1 += ic1;
            dxc2 += ic2;
        }
    }

    if (p3->y!=p2->y)
    {
        long ix2 = (p3->x-p2->x)*65536/(p3->y-p2->y);
        long ic2 = (c3-c2)*65536/(p3->y-p2->y);
        long dx1 = p1->x*65536+((p2->y-p1->y)*ix1);
        long dxc1 = c1*65536+((p2->y-p1->y)*ic1);
        long dx2 = p2->x*65536;
        long dxc2 = c2*65536;
        long ly;
        for (ly=p2->y ; ly<p3->y ; ++ly)
        {
            if (ly>=0 && ly<GSKE_Height) // && ly%4==0) <- same as
                GSKE_DrawHLineGouraud(dx1/65536, dxc1, dx2/65536, dxc2, ly);
            dx1 += ix1;
            dx2 += ix2;
            dxc1 += ic1;
            dxc2 += ic2;
        }
    }
}

inline void GSKE_DrawHLineTexMap(long x1, long tx1, long ty1, long x2, long tx2, long ty2, long y, const u8* texmap)
{
    if (x1==x2) return;
    if (x1>x2) { GSKE_xchg(&x1, &x2); GSKE_xchg(&tx1, &tx2); GSKE_xchg(&ty1, &ty2); }
    if (x1>=GSKE_Width || x2<0) return;
    long ix = (tx2-tx1)/(x2-x1); 
    long iy = (ty2-ty1)/(x2-x1); 
    if (x1<0) { tx1 += -x1*ix; ty1 += -x1*iy; x1 = 0; }
    if (x2>=GSKE_Width) x2 = GSKE_Width-1;
    long start = x1+GSKE_HLineOffsets[y];
    long end = x2+GSKE_HLineOffsets[y];
    long p;
    // 2x precision for the win
    for (p=start ; p<end ; ++p)
    {
        long t= ((tx1>>17)+((ty1>>10)&0xFF00))%(64*128);
        GSKE_FrameBuffer[p] = texmap[t];
        tx1 += ix;
        ty1 += iy;
    }
}

void GSKE_DrawTriangleTexMap(Vect2D_s16* v1, Vect2D_s16* uv1, Vect2D_s16* v2, Vect2D_s16* uv2, Vect2D_s16* v3, Vect2D_s16* uv3, const u8* texmap)
{
    if ((v3->x-v1->x)*(v2->y-v1->y) > (v3->y-v1->y)*(v2->x-v1->x))
        return; // why does nobody see me ;(
            
    Vect2D_s16* p1[2]; p1[0] = v1; p1[1] = uv1;
    Vect2D_s16* p2[2]; p2[0] = v2; p2[1] = uv2;
    Vect2D_s16* p3[2]; p3[0] = v3; p3[1] = uv3;
    Vect2D_s16* t;
    
    if (p2[0]->y>p3[0]->y) { p2[0] = v3; p3[0] = v2; p2[1] = uv3; p3[1] = uv2; }
    if (p1[0]->y>p3[0]->y) { p1[0] = p3[0]; p3[0] = v1; p1[1] = p3[1]; p3[1] = uv1; }
    if (p1[0]->y>p2[0]->y) { t = p1[0]; p1[0] = p2[0]; p2[0] = t; t = p1[1]; p1[1] = p2[1]; p2[1] = t; }
    if (p3[0]->y==p1[0]->y) return;

    long ix1 = (p3[0]->x-p1[0]->x)*65536/(p3[0]->y-p1[0]->y);
    long iuv1[2]; iuv1[0] = (p3[1]->x-p1[1]->x)*65536/(p3[0]->y-p1[0]->y); iuv1[1] = (p3[1]->y-p1[1]->y)*65536/(p3[0]->y-p1[0]->y);
    
    if (p2[0]->y!=p1[0]->y)
    {
        long ix2 = (p2[0]->x-p1[0]->x)*65536/(p2[0]->y-p1[0]->y);
        long iuv2[2]; iuv2[0] = (p2[1]->x-p1[1]->x)*65536/(p2[0]->y-p1[0]->y); iuv2[1] = (p2[1]->y-p1[1]->y)*65536/(p2[0]->y-p1[0]->y);
        long dx1 = p1[0]->x*65536;
        long dxuv1[2]; dxuv1[0] = p1[1]->x*65536; dxuv1[1] = p1[1]->y*65536;
        long dx2 = dx1;
        long dxuv2[2]; dxuv2[0] = dxuv1[0]; dxuv2[1] = dxuv1[1];
        long ly;
        for (ly=p1[0]->y ; ly<p2[0]->y ; ++ly)
        {
            if (ly>=0 && ly<GSKE_Height)
                GSKE_DrawHLineTexMap(dx1/65536, dxuv1[0], dxuv1[1], dx2/65536, dxuv2[0], dxuv2[1], ly, texmap);
            dx1 += ix1; dx2 += ix2;
            dxuv1[0] += iuv1[0]; dxuv1[1] += iuv1[1];
            dxuv2[0] += iuv2[0]; dxuv2[1] += iuv2[1];
        }
    }

    if (p3[0]->y!=p2[0]->y)
    {
        long ix2 = (p3[0]->x-p2[0]->x)*65536/(p3[0]->y-p2[0]->y);
        long iuv2[2]; iuv2[0] = (p3[1]->x-p2[1]->x)*65536/(p3[0]->y-p2[0]->y); iuv2[1] = (p3[1]->y-p2[1]->y)*65536/(p3[0]->y-p2[0]->y);
        long dx1 = p1[0]->x*65536+((p2[0]->y-p1[0]->y)*ix1);
        long dxuv1[1]; dxuv1[0] = p1[1]->x*65536+((p2[0]->y-p1[0]->y)*iuv1[0]); dxuv1[1] = p1[1]->y*65536+((p2[0]->y-p1[0]->y)*iuv1[1]);
        long dx2 = p2[0]->x*65536;
        long dxuv2[2]; dxuv2[0] = p2[1]->x*65536; dxuv2[1] = p2[1]->y*65536;        
        long ly;
        for (ly=p2[0]->y ; ly<p3[0]->y ; ++ly)
        {
            if (ly>=0 && ly<GSKE_Height)
                GSKE_DrawHLineTexMap(dx1/65536, dxuv1[0], dxuv1[1], dx2/65536, dxuv2[0], dxuv2[1], ly, texmap);
            dx1 += ix1; dx2 += ix2;
            dxuv1[0] += iuv1[0]; dxuv1[1] += iuv1[1];
            dxuv2[0] += iuv2[0]; dxuv2[1] += iuv2[1];
        }
    }
}

