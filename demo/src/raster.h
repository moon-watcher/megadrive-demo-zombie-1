#ifndef _GSKE_RASTER_H_
#define _GSKE_RASTER_H_

#include "genesis.h"

inline void GSKE_DrawHLineFlat(long x1, long x2, long y, u8 color);
void GSKE_DrawTriangleFlat(Vect2D_s16* v1, Vect2D_s16* v2, Vect2D_s16* v3, u8 color);
inline void GSKE_DrawHLineGouraud(long x1, long c1, long x2, long c2, long y);
void GSKE_DrawTriangleGouraud(Vect2D_s16* v1, u8 c1, Vect2D_s16* v2, u8 c2, Vect2D_s16* v3, u8 c3);
inline void GSKE_DrawHLineTexMap(long x1, long tx1, long ty1, long x2, long tx2, long ty2, long y, const u8* texmap);
void GSKE_DrawTriangleTexMap(Vect2D_s16* v1, Vect2D_s16* uv1, Vect2D_s16* v2, Vect2D_s16* uv2, Vect2D_s16* v3, Vect2D_s16* uv3, const u8* texmap);

#endif
