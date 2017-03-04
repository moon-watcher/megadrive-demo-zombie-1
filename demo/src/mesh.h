#ifndef _GSKE_MESH_H_
#define _GSKE_MESH_H_

#include "genesis.h"

typedef struct
{
    u16 vertex_count;
    Vect3D_f16* vertex_positions;
    Vect2D_s16* vertex_texcoords;
    u8* vertex_colors;
    u16 face_count;
    u16* face_indices;
    Vect3D_f16 translation;
    Vect3D_f16 rotation;
    u8* texture;
    int material;
} GSKE_Mesh;

GSKE_Mesh* GSKE_CreateMesh();
void GSKE_DestroyMesh(GSKE_Mesh* mesh);
GSKE_Mesh* GSKE_GenerateCube();

#endif
