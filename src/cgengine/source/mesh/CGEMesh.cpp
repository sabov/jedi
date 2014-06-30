#include "mesh/CGEMesh.h"
#include <assert.h>

namespace CGEngine {

//////////////////////////////////////////////////////////////////////////////////////////////
//  CMeshEntry
//////////////////////////////////////////////////////////////////////////////////////////////

CMesh::CMeshEntry::CMeshEntry()
{
    m_NumIndices  = 0;
    m_MeshProperties = 0 ;
}

CMesh::CMeshEntry::~CMeshEntry()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  CMesh
///////////////////////////////////////////////////////////////////////////////////////////////

bool CMesh::LoadMesh(const std::string &_filename, unsigned int _postprocessingFlags)
{
    bool return_value = false;
    clear();

    //Create Vertex-Array
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    //Create buffers for the vertex attributes
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

    Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(_filename.c_str(), _postprocessingFlags );

    if (pScene)
    {
        return_value = initFromScene(pScene, _filename);
    }
    else
    {
        std::cerr << "Error parsing " << _filename.c_str() << "Assimp: " << Importer.GetErrorString() << std::endl ;
    }

    glBindVertexArray(0);

    return return_value;
}

bool CMesh::initFromScene(const aiScene *_pScene, const std::string &_filename)
{
    m_Entries.resize( _pScene->mNumMeshes );
    m_Textures.resize( _pScene->mNumMaterials );

    Vec3DVec positions;
    Vec3DVec normals;
    Vec2DVec texcoords;
    std::vector<GLuint> indices;

    unsigned int num_vertices = 0;
    unsigned int num_indices = 0;
    for ( unsigned int i = 0; i < m_Entries.size(); i++)
    {
        m_Entries[i].m_MaterialIndex = _pScene->mMeshes[i]->mMaterialIndex;
        m_Entries[i].m_NumIndices = _pScene->mMeshes[i]->mNumFaces * 3;
        m_Entries[i].m_BaseVertex = num_vertices;
        m_Entries[i].m_BaseIndex = num_indices;

        num_vertices += _pScene->mMeshes[i]->mNumVertices;
        num_indices += m_Entries[i].m_NumIndices;
    }

    positions.reserve(num_vertices);
    normals.reserve(num_vertices);
    texcoords.reserve(num_vertices);
    indices.reserve(num_indices);

    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++)
    {
        const aiMesh* paiMesh = _pScene->mMeshes[i];
        initMesh(paiMesh, positions, normals, texcoords, indices);
        setProperties(i, paiMesh);
    }

    if ( !initMaterials(_pScene, _filename) )
        return false;

    // Generate and populate the buffers with vertex attributes and the indices
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords[0]) * texcoords.size(), &texcoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    return (glGetError() == GL_NO_ERROR);
}



void CMesh::initMesh(const aiMesh* _paiMesh, Vec3DVec& _positions, Vec3DVec& _normals, Vec2DVec &_texCoords, std::vector<unsigned int> &_indices)
{
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int i = 0 ; i < _paiMesh->mNumVertices ; i++) {
        const aiVector3D* pPos      = &(_paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = _paiMesh->HasNormals() ? &(_paiMesh->mNormals[i]) : &Zero3D;
        const aiVector3D* pTexCoord = _paiMesh->HasTextureCoords(0) ? &(_paiMesh->mTextureCoords[0][i]) : &Zero3D;

        _positions.push_back( Vec3(pPos->x, pPos->y, pPos->z) );
        _normals.push_back( Vec3(pNormal->x, pNormal->y, pNormal->z) );
        _texCoords.push_back( Vec2(pTexCoord->x, pTexCoord->y ) );
    }
    //Index buffer
    for (unsigned int i = 0 ; i < _paiMesh->mNumFaces ; i++) {
        const aiFace& Face = _paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        _indices.push_back(Face.mIndices[0]);
        _indices.push_back(Face.mIndices[1]);
        _indices.push_back(Face.mIndices[2]);
    }
}

void CMesh::setProperties(unsigned int _index, const aiMesh *_paiMesh)
{
    if ( _paiMesh->HasTextureCoords(0) )
    {
        m_Entries[_index].m_MeshProperties |= CGE_HAS_TEXCOORDS ;
    }
    if ( _paiMesh->HasNormals() )
    {
        m_Entries[_index].m_MeshProperties |= CGE_HAS_NORMALS ;
    }
    if ( _paiMesh->HasTangentsAndBitangents() )
    {
        m_Entries[_index].m_MeshProperties |= CGE_HAS_TANGENTSPACE ;
    }

}

void CMesh::clear()
{
    m_Textures.clear();

    if (m_Buffers[0] != 0)
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

    if (m_VAO != 0)
    {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}

/*
 * Still a lot to do here (for different material types and textures...)
 */
bool CMesh::initMaterials(const aiScene *_pScene, const std::string &_filename)
{
    bool return_value = true;

    int pos = _filename.find_last_of("\\/");
    std::string current_dir = _filename.substr(0, pos);

    // Initialize the materials
    for (unsigned int i = 0 ; i < _pScene->mNumMaterials ; i++)
    {
        const aiMaterial* pMaterial = _pScene->mMaterials[i];

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString aiPath;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiPath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
            {
                std::string path(aiPath.data);
#ifdef _WIN32
                std::string slash("\\");
#else
                std::string slash("/");
#endif
                size_t p = path.find("..\\");
                size_t len = 3;
                if ( p != std::string::npos )
                {
                    path.replace(p, len, "");
                }
                std::string fullPath = current_dir + slash + path;
#ifndef _WIN32
                p = fullPath.find_first_of("\\");
                while ( p != std::string::npos )
                {
                    fullPath.replace(p, 1, slash);
                    p = fullPath.find_first_of("\\");
                } ;
#endif

                GLTexture2DPointer tex_ptr(new CGLTexture2D);

                if ( !tex_ptr->VLoadTexture(fullPath) )
                {
                    std::cout << "Error Loading Texture: " << "For Mesh Object Nr. " << getObjectID() << std::endl;
                    m_Textures[i] = GLTexture2DPointer() ;
                    return_value = false;
                }
                else
                {
                    m_Textures[i] = tex_ptr ;
                }
            }
        }

    }

    return return_value;

}

void CMesh::VOnDraw()
{
    glBindVertexArray(m_VAO);

    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {

        const unsigned int MaterialIndex = m_Entries[i].m_MaterialIndex;

        if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex])
        {
            m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
        }

        glDrawElementsBaseVertex(GL_TRIANGLES, m_Entries[i].m_NumIndices, GL_UNSIGNED_INT,
                                 (void*)(sizeof(unsigned int) * m_Entries[i].m_BaseIndex), m_Entries[i].m_BaseVertex);
    }

    glBindVertexArray(0);
}

bool CMesh::HasProperty(CGEMeshProperty _property) const
{
    unsigned int a = 0x0 | _property ;
    bool return_value = true;

    for (unsigned int i = 0 ; i < m_Entries.size(); ++i)
    {
        unsigned int b = m_Entries[i].m_MeshProperties & a ;
        if ( (b >> (_property-1)) != 1 )
            return_value = false;
    }

    return return_value;
}















}   //End Namespace
