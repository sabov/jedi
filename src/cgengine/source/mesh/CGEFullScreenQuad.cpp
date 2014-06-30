#include "mesh/CGEFullScreenQuad.h"

namespace CGEngine {

bool CFullScreenQuad::LoadMesh(const std::string &_filename, unsigned int _postprocessingFlags)
{
    clear();

    //Create Vertex-Array
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    //Create buffers for the vertex attributes
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

    m_Entries.resize( 1 );
    m_Textures.resize( 1 );

    Vec3DVec positions;
    Vec3DVec normals;
    Vec2DVec texcoords;
    std::vector<GLuint> indices;

    unsigned int num_vertices = 4;
    unsigned int num_indices = 6;

    m_Entries[0].m_NumIndices = 6;
    m_Entries[0].m_BaseVertex = 0;
    m_Entries[0].m_BaseIndex = 0;

    positions.reserve(num_vertices);
    normals.reserve(num_vertices);
    texcoords.reserve(num_vertices);
    indices.reserve(num_indices);

    positions.push_back(Vec3(-1.0, -1.0, 0.0)); positions.push_back(Vec3(1.0, -1.0, 0.0));
    positions.push_back(Vec3(1.0, 1.0, 0.0)); positions.push_back(Vec3(-1.0, 1.0, 0.0));

    texcoords.push_back(Vec2(0.0, 0.0)); texcoords.push_back(Vec2(1.0, 0.0));
    texcoords.push_back(Vec2(1.0, 1.0)); texcoords.push_back(Vec2(0.0, 1.0));

    for (unsigned int i = 0 ; i < 4 ; i++) {
        normals.push_back( Vec3(0.0, 0.0, 1.0) );
    }
    //Index buffer
    indices.push_back(0); indices.push_back(1); indices.push_back(2);
    indices.push_back(0); indices.push_back(2); indices.push_back(3);

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

    glBindVertexArray(0);

    return true;

}

void CFullScreenQuad::VOnDraw()
{
    glBindVertexArray(m_VAO);


    glDrawElementsBaseVertex(GL_TRIANGLES, m_Entries[0].m_NumIndices, GL_UNSIGNED_INT,
                                 (void*)(sizeof(unsigned int) * m_Entries[0].m_BaseIndex), m_Entries[0].m_BaseVertex);

    glBindVertexArray(0);

}

bool CFullScreenQuad::HasProperty(CGEMeshProperty _property) const
{
    return false;
}

}   //Namespace
