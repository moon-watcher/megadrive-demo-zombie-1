#include "mesh.h"

const Vect3D_f16 GSKE_CubeVertexPositions[8] =
{
    {intToFix16(-1), intToFix16(-1), intToFix16(-1)}, {intToFix16(1), intToFix16(-1), intToFix16(-1)}, {intToFix16(1), intToFix16(1), intToFix16(-1)}, {intToFix16(-1), intToFix16(1), intToFix16(-1)},
    {intToFix16(-1), intToFix16(-1), intToFix16(1)}, {intToFix16(1), intToFix16(-1), intToFix16(1)}, {intToFix16(1), intToFix16(1), intToFix16(1)}, {intToFix16(-1), intToFix16(1), intToFix16(1)}
};
const Vect2D_s16 GSKE_CubeVertexTexCoords[8] =
{
    {intToFix16(0), intToFix16(0)}, {intToFix16(2), intToFix16(0)}, {intToFix16(2), intToFix16(2)}, {intToFix16(0), intToFix16(2)},
    {intToFix16(0), intToFix16(0)}, {intToFix16(2), intToFix16(0)}, {intToFix16(2), intToFix16(2)}, {intToFix16(0), intToFix16(2)}
};
const u16 GSKE_CubeFaceIndices[36] = { 5, 4, 7, 6, 5, 7, 2, 3, 0, 1, 2, 0, 6, 7, 3, 2, 6, 3, 1, 0, 4, 5, 1, 4, 6, 2, 1, 5, 6, 1, 3, 7, 4, 0, 3, 4 };

GSKE_Mesh* GSKE_CreateMesh()
{
    GSKE_Mesh* mesh = (GSKE_Mesh*)MEM_alloc(sizeof(GSKE_Mesh));
    memset(mesh, 0, sizeof(GSKE_Mesh));
    mesh->translation.x = FIX16(0);
    mesh->translation.y = FIX16(0);
    mesh->translation.z = FIX16(0);
    mesh->rotation.x = FIX16(0);
    mesh->rotation.y = FIX16(0);
    mesh->rotation.z = FIX16(0);
    mesh->material = 0;
    return mesh;
}

void GSKE_DestroyMesh(GSKE_Mesh* mesh)
{
    MEM_free(mesh->vertex_positions);
    if (mesh->vertex_texcoords)
        MEM_free(mesh->vertex_texcoords);
    if (mesh->vertex_colors)
        MEM_free(mesh->vertex_colors);
    MEM_free(mesh->face_indices);
    MEM_free(mesh);
}

GSKE_Mesh* GSKE_GenerateCube()
{
    GSKE_Mesh* cube = GSKE_CreateMesh();
    cube->vertex_count = 8;
    cube->vertex_positions = (Vect3D_f16*)MEM_alloc(cube->vertex_count*sizeof(Vect3D_f16));
    memcpy(cube->vertex_positions, GSKE_CubeVertexPositions, cube->vertex_count*sizeof(Vect3D_f16));
    cube->vertex_texcoords = (Vect2D_s16*)MEM_alloc(cube->vertex_count*sizeof(Vect2D_s16));
    memcpy(cube->vertex_texcoords, GSKE_CubeVertexTexCoords, cube->vertex_count*sizeof(Vect2D_s16));
    cube->vertex_colors = (u8*)MEM_alloc(cube->vertex_count);
    u8 i;
    for (i=0 ; i<cube->vertex_count ; ++i)
        cube->vertex_colors[i] = i+2;
    cube->face_count = 12;
    cube->face_indices = (u16*)MEM_alloc(cube->face_count*3*sizeof(u16));
    memcpy(cube->face_indices, GSKE_CubeFaceIndices, cube->face_count*3*sizeof(u16));
    return cube;
}
