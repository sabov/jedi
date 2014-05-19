#include "CGEMesh.h"
#include <assert.h>

namespace CGEngine {

//////////////////////////////////////////////////////////////////////////////////////////////
//  CMeshEntry
//////////////////////////////////////////////////////////////////////////////////////////////

CMesh::CMeshEntry::CMeshEntry()
{
    m_VBO = 0;
    m_IBO = 0;
    m_NumIndices  = 0;
    m_MeshProperties = 0 ;
}

CMesh::CMeshEntry::~CMeshEntry()
{
    if (m_VBO != 0)
    {
        glDeleteBuffers(1, &m_VBO);
    }

    if (m_IBO != 0)
    {
        glDeleteBuffers(1, &m_IBO);
    }
}

void CMesh::CMeshEntry::Init(const std::vector<GLVertex3f>& _vertices,
                      const std::vector<unsigned int>& _indices)
{
    m_NumIndices = _indices.size();

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLVertex3f) * _vertices.size(), &_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_NumIndices, &_indices[0], GL_STATIC_DRAW);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  CMesh
///////////////////////////////////////////////////////////////////////////////////////////////

bool CMesh::LoadMesh(const std::string &_filename, unsigned int _postprocessingFlags)
{
    bool return_value = false;
    Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(_filename.c_str(), _postprocessingFlags );

    if (pScene)
    {
        return_value = initFromScene(pScene, _filename);
    }
    else
    {
        std::cout << "Error parsing " << _filename.c_str() << "Assimp: " << Importer.GetErrorString() << std::endl ;
    }

    return return_value;
}

bool CMesh::initFromScene(const aiScene *_pScene, const std::string &_filename)
{
    m_Entries.resize( _pScene->mNumMeshes );
    m_Textures.resize( _pScene->mNumMaterials );

    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++)
    {
        const aiMesh* paiMesh = _pScene->mMeshes[i];
        initMesh(i, paiMesh);
    }

    return initMaterials(_pScene, _filename);
}



void CMesh::initMesh(unsigned int _index, const aiMesh *_paiMesh)
{
    m_Entries[_index].m_MaterialIndex = _paiMesh->mMaterialIndex;
    setProperties(_index, _paiMesh);

    std::vector<GLVertex3f> Vertices;
    std::vector<unsigned int> Indices;

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int i = 0 ; i < _paiMesh->mNumVertices ; i++) {
        const aiVector3D* pPos      = &(_paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = _paiMesh->HasNormals() ? &(_paiMesh->mNormals[i]) : &Zero3D;
        const aiVector3D* pTexCoord = _paiMesh->HasTextureCoords(0) ? &(_paiMesh->mTextureCoords[0][i]) : &Zero3D;

        GLVertex3f v(   GLPoint3f( pPos->x, pPos->y, pPos->z ),
                        GLPoint2f( pTexCoord->x, pTexCoord->y ),
                        GLPoint3f( pNormal->x, pNormal->y, pNormal->z ));

        Vertices.push_back(v);
    }

    for (unsigned int i = 0 ; i < _paiMesh->mNumFaces ; i++) {
        const aiFace& Face = _paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }

    m_Entries[_index].Init(Vertices, Indices);
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
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
#ifdef _WIN32
                std::string slash("\\");
#else
                std::string slash("/");
#endif
                std::string fullPath = current_dir + slash + Path.data;

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
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].m_VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex3f), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLVertex3f), (const GLvoid*)(3 * sizeof(GL_FLOAT)));
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex3f), (const GLvoid*)(5 * sizeof(GL_FLOAT)));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].m_IBO);

        const unsigned int MaterialIndex = m_Entries[i].m_MaterialIndex;

        if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex])
        {
            m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
        }

        glDrawElements(GL_TRIANGLES, m_Entries[i].m_NumIndices, GL_UNSIGNED_INT, 0);
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
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
