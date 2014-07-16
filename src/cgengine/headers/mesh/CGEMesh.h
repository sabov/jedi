
/***********************************************************************

  Class for loading and rendering meshes

***********************************************************************/

#ifndef MESH_H
#define MESH_H

#pragma once

#ifndef __GLEW_H__
    #include <GL/glew.h>
#endif

#ifdef _WIN32
    #include <assimp/Importer.hpp>
    #include <assimp/scene.h>
    #include <assimp/postprocess.h>
#else
    #include <assimp/Importer.hpp>
    #include <assimp/scene.h>
    #include <assimp/postprocess.h>
#endif
#include <boost/smart_ptr/shared_ptr.hpp>
#include <vector>
#include <string>
#include <iostream>
#include "object/CGEDrawableObject.h"
#include "math/CGEGeometry.h"
#ifndef CGETEXTURE2D_H
    #include "textures/CGETexture2D.h"
#endif

namespace CGEngine {

#define ARRAY_SIZE_IN_ELEMENTS(x) (sizeof(x)/sizeof(x[0]))

//Class Header and Typedefs
//=========================
class CMesh ;
typedef boost::shared_ptr<CMesh>    MeshPointer ;

enum CGEMeshProperty
{
    CGE_HAS_TEXCOORDS       = 1 ,
    CGE_HAS_NORMALS         = 2 ,
    CGE_HAS_TANGENTSPACE    = 4
};

enum CGEMeshProcessingFlags
{
    CGE_NO_POSTPROCESSING           = 0x0,
    CGE_COMPUTE_NORMALS             = aiProcess_GenSmoothNormals,
    CGE_COMPUTE_TANGENT_SPACE       = aiProcess_CalcTangentSpace,
    CGE_JOIN_IDENT_VERTICES         = aiProcess_JoinIdenticalVertices,
    CGE_TRIANGULATE                 = aiProcess_Triangulate,
    CGE_REMOVE_REDUNDANT_MATERIALS  = aiProcess_RemoveRedundantMaterials,
    CGE_GEN_TEXT_COORDS             = aiProcess_GenUVCoords,
    CGE_FLIP_TEX_COORDS             = aiProcess_FlipUVs,
    CGE_FIX_INFACING_NORMALS        = aiProcess_FixInfacingNormals
};

//Class for loading and rendering meshes
//======================================
class CMesh : public IDrawableObject
{
private:
    //disallow copy constructor
    CMesh(const CMesh&) {}

protected:
    GLuint m_VAO;
    GLuint m_Buffers[4];

    bool initFromScene(const aiScene* _pScene, const std::string& _filename);
    void initMesh(const aiMesh* _paiMesh,
                  Vec3DVec& _positions,
                  Vec3DVec& _normals,
                  Vec2DVec& _texCoords,
                  std::vector<unsigned int>& _indices)                      ;
    bool initMaterials(const aiScene* _pScene, const std::string& _filename);
    void setProperties(unsigned int _index, const aiMesh* _paiMesh)         ;
    void clear();

#define INDEX_BUFFER    0
#define POS_VB          1
#define NORMAL_VB       2
#define TEXCOORD_VB     3

    //MeshEntry defines a part of a mesh (if mesh is composed of several parts)
    //and provides functionality for initializing the mesh parts
    struct CMeshEntry {
        CMeshEntry() ;

        ~CMeshEntry();

        unsigned int m_NumIndices       ;
        unsigned int m_BaseVertex       ;
        unsigned int m_BaseIndex        ;
        unsigned int m_MaterialIndex    ;
        unsigned int m_MeshProperties   ;
    };

    typedef std::vector<CMeshEntry> MeshEntryVector ;

    MeshEntryVector                 m_Entries       ;
    std::vector<GLTexture2DPointer> m_Textures      ;

public:
    //Constructor
    CMesh() : IDrawableObject() {m_VAO = 0;}
    //Destructor
    virtual ~CMesh()
    {
        m_Entries.clear();;
        clear();
    }

    //Load a mesh given given by the filename or path
    bool LoadMesh(const std::string& _filename, unsigned int _postprocessingFlags = CGE_NO_POSTPROCESSING);

    //Draw Mesh
    virtual void VOnDraw();

    //Check if mesh has indicated property
    bool HasProperty(CGEMeshProperty _property) const;

};

} //End NameSpace

#endif // MESH_H
