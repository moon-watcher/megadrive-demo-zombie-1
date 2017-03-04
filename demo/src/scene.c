#include "scene.h"
#include "raster.h"

GSKE_SceneS GSKE_Scene;

void GSKE_CreateScene()
{
    memset(GSKE_Scene.meshes, 0, sizeof(GSKE_Mesh*)*GSKE_MAX_MESH);
    GSKE_Scene.mesh_count = 0;
    GSKE_Scene.camera.translation.x = FIX16(0);
    GSKE_Scene.camera.translation.y = FIX16(0);
    GSKE_Scene.camera.translation.z = FIX16(0);
    GSKE_Scene.camera.translation.x = FIX16(0);
    GSKE_Scene.camera.translation.y = FIX16(0);
    GSKE_Scene.camera.translation.z = FIX16(0);
    GSKE_Scene.maxFov = 0;
}

void GSKE_DestroyScene()
{
    u16 i;
    for (i=0 ; i<GSKE_Scene.mesh_count ; ++i)
        GSKE_RemoveMesh(i);
    GSKE_CreateScene();   
}


void quick_sort(int deb, int fin)
{
  int i=deb;
  int j=fin;
  int milieu=GSKE_Scene.face_norms[GSKE_Scene.face_indices[(deb+fin)/2]];
  int temp;

  while(i<=j)
  {
    while(GSKE_Scene.face_norms[GSKE_Scene.face_indices[i]]>milieu && i<j) i++;
    while(GSKE_Scene.face_norms[GSKE_Scene.face_indices[j]]<milieu && j>0) j--;

    if(i<=j)
    {
      temp=GSKE_Scene.face_indices[i];
      GSKE_Scene.face_indices[i]=GSKE_Scene.face_indices[j];
      GSKE_Scene.face_indices[j]=temp;
      i++;
      j--;
    }
  }

  if(i<fin) quick_sort(i,fin);
  if(deb<j) quick_sort(deb,j);
}

void GSKE_UpdateScene()
{
    u16 vertex_count = 0;
    u16 face_count = 0;
    u16 i,j;
    for (i=0 ; i<GSKE_Scene.mesh_count ; ++i)
    {
        if (vertex_count+GSKE_Scene.meshes[i]->vertex_count<GSKE_MAX_VERTEX)
        {
            M3D_setTransMat3D(&GSKE_Scene.meshes[i]->translation);
            M3D_setRotMat3D(&GSKE_Scene.meshes[i]->rotation);
            M3D_transform3D(GSKE_Scene.meshes[i]->vertex_positions, GSKE_Scene.ws_vertices+vertex_count, GSKE_Scene.meshes[i]->vertex_count);
            M3D_setTransMat3D(&GSKE_Scene.camera.translation);
            M3D_setRotMat3D(&GSKE_Scene.camera.rotation);
            M3D_transform3D(GSKE_Scene.ws_vertices+vertex_count, GSKE_Scene.cs_vertices+vertex_count, GSKE_Scene.meshes[i]->vertex_count);
                        
            for (j=0 ; j<GSKE_Scene.meshes[i]->face_count ; ++j)
            {
                GSKE_Scene.face_norms[face_count] = GSKE_Scene.cs_vertices[vertex_count+GSKE_Scene.meshes[i]->face_indices[j*3]].z
                    + GSKE_Scene.cs_vertices[vertex_count+GSKE_Scene.meshes[i]->face_indices[j*3+1]].z
                    + GSKE_Scene.cs_vertices[vertex_count+GSKE_Scene.meshes[i]->face_indices[j*3+2]].z;
                GSKE_Scene.face_indices[face_count] = face_count;
                ++face_count;
            }
            
            quick_sort(0, face_count-1);
            
            vertex_count += GSKE_Scene.meshes[i]->vertex_count;
        }
    }

    if (GSKE_Scene.maxFov)
    {
        double ofx = BMP_WIDTH/2.;
        double ofy = BMP_HEIGHT/2.;
        for (i=0;i<vertex_count;i++)
        {
            double depth = (double)GSKE_Scene.cs_vertices[i].z*4.1;
            if (depth!=0)
            {
                GSKE_Scene.vp_vertices[i].x=(double)(GSKE_Scene.cs_vertices[i].x<<8)/depth+ofx;
                GSKE_Scene.vp_vertices[i].y=(double)(GSKE_Scene.cs_vertices[i].y<<8)/depth+ofy;
            }
        }
    }
    else
    {
        double ofx = BMP_WIDTH/2.;
        double ofy = BMP_HEIGHT/2.;
        for (i=0;i<vertex_count;i++)
        {
            double depth = (double)GSKE_Scene.cs_vertices[i].z*1.0;
            if (depth!=0)
            {
                GSKE_Scene.vp_vertices[i].x=(double)(GSKE_Scene.cs_vertices[i].x*ofx)/depth+ofx;
                GSKE_Scene.vp_vertices[i].y=(double)(GSKE_Scene.cs_vertices[i].y*ofx*1.7)/depth+ofy;
            }
        }
    }
    
    u16 giv = 0;
    for (i=0 ; i<GSKE_Scene.mesh_count && giv<vertex_count; ++i)
    {
        GSKE_Mesh* mesh = GSKE_Scene.meshes[i];
        u16 f;
        for (f=0 ; f<mesh->face_count ; ++f)
        {
            u16 iv[3];
            u16 sf = GSKE_Scene.face_indices[f];
            iv[0] = mesh->face_indices[sf*3];
            iv[1] = mesh->face_indices[sf*3+1];
            iv[2] = mesh->face_indices[sf*3+2];
                      
            if (GSKE_Scene.cs_vertices[iv[0]].z<=0 || GSKE_Scene.cs_vertices[iv[1]].z<=0 || GSKE_Scene.cs_vertices[iv[2]].z<=0)
                continue;
                
            if (mesh->material==2) // TexMap
            {
                GSKE_DrawTriangleTexMap(
                    GSKE_Scene.vp_vertices+iv[0]+giv, mesh->vertex_texcoords+iv[0],
                    GSKE_Scene.vp_vertices+iv[1]+giv, mesh->vertex_texcoords+iv[1],
                    GSKE_Scene.vp_vertices+iv[2]+giv, mesh->vertex_texcoords+iv[2], mesh->texture);
            }
            else if (mesh->material==1) // Gouraud
            {
                GSKE_DrawTriangleGouraud(
                    GSKE_Scene.vp_vertices+iv[0]+giv, mesh->vertex_colors[iv[0]],
                    GSKE_Scene.vp_vertices+iv[1]+giv, mesh->vertex_colors[iv[1]],
                    GSKE_Scene.vp_vertices+iv[2]+giv, mesh->vertex_colors[iv[2]]);
            }
            else // Flat
            {
                GSKE_DrawTriangleFlat(
                    GSKE_Scene.vp_vertices+iv[0]+giv,
                    GSKE_Scene.vp_vertices+iv[1]+giv,
                    GSKE_Scene.vp_vertices+iv[2]+giv, mesh->vertex_colors[iv[2]]);
            }        
        }
        
        giv += mesh->vertex_count;
    }
}

u16 GSKE_AddMesh(GSKE_Mesh* mesh)
{
    if (GSKE_Scene.mesh_count<GSKE_MAX_MESH)
    {
        GSKE_Scene.meshes[GSKE_Scene.mesh_count++] = mesh;
        return GSKE_Scene.mesh_count-1;
    }
    return -1;
}

void GSKE_RemoveMesh(u16 mesh)
{
    if (mesh<GSKE_Scene.mesh_count)
    {
        GSKE_DestroyMesh(GSKE_Scene.meshes[mesh]);
        GSKE_Scene.meshes[mesh] = 0;
        int i;
        for (i=mesh ; i<GSKE_Scene.mesh_count-1 ; ++i)
            GSKE_Scene.meshes[i] = GSKE_Scene.meshes[i+1];
        --GSKE_Scene.mesh_count;        
    }
}


