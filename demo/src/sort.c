#include "sort.h"

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
