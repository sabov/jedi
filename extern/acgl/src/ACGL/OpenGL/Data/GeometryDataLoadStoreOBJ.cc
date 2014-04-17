/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Data/GeometryDataLoadStore.hh>
#include <ACGL/Math/Math.hh>
#include <ACGL/Utils/StringHelpers.hh>

#include <fstream>
#include <string>

using namespace ACGL;
using namespace ACGL::OpenGL;
using namespace ACGL::Utils;

namespace
{
    struct IndexTuple
    {
        IndexTuple() :
        position(-1),
        texCoord(-1),
        normal(-1)
        {
        }

        int position;
        int texCoord;
        int normal;
    };

    void skipLine(std::fstream& _stream)
    {
        _stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Parses a string of space-separated numbers into a packed floating-point vector (_data) with a maximum number of _maxDimension elements
    void parseVector(const std::string& _string, int _maxDimension, int& _dimension, float* _data)
    {
        std::stringstream stream(_string);
        float temp;
        _dimension = 0;
        while(stream >> temp && _dimension < _maxDimension)
        {
            _data[_dimension] = temp;
            _dimension++;
        }
    }

    // Turns an index parameter string into a std::vector of IndexTuples, e.g.
    // "1//2 3//4 5//6" --> { IndexTuple(1, -1, 2), IndexTuple(3, -1, 4), IndexTuple(5, -1, 6) }
    std::vector<IndexTuple> parseIndices(const std::string& _string)
    {
        std::vector<IndexTuple> indices;

        std::stringstream stream(_string);
        std::string vertexString;
        while(stream >> vertexString)
        {
            std::vector<std::string> componentsString = ACGL::Utils::StringHelpers::split(vertexString, '/', false);
            IndexTuple indexTuple;

            for(int i = 0; i < std::min<int>(componentsString.size(), 3); ++i)
            {
                std::string& componentString = componentsString[i];
                int index = atoi(componentString.c_str());

                if(i == 0) indexTuple.position = index-1;
                if(i == 1) indexTuple.texCoord = index-1;
                if(i == 2) indexTuple.normal   = index-1;
            }

            indices.push_back(indexTuple);
        }

        return indices;
    }
}

namespace ACGL{
namespace OpenGL{

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                           library specific load
///////////////////////////////////////////////////////////////////////////////////////////////////

SharedGeometryData loadGeometryDataFromOBJ(const std::string& _filename, bool _computeNormals)
{
    SharedGeometryData data;

    std::fstream file(_filename.c_str(), std::ios_base::in);
    if(!file.good())
    {
        error() << "could not open file " << _filename << std::endl;
        return data;
    }

    GLenum primitiveType = GL_INVALID_ENUM;
    bool hasTexCoords = false;
    bool hasNormals = false;
    int positionDimension = 4;
    int texCoordDimension = -1;
    int normalDimension = 3;

    std::vector<glm::vec4> positionData;
    std::vector<glm::vec3> texCoordData;
    std::vector<glm::vec3> normalData;

    std::vector<IndexTuple> indices;

    std::string keyword;
    std::string parameters;
    while(file.good())
    {
        // Parse the current line

        // If the line starts with a #, it is a comment
        if(file.peek() == '#')
        {
            skipLine(file);
            continue;
        }

        // Otherwise, extract the first word and the remainder
        file >> keyword;
        std::getline(file, parameters);

        if(keyword == "v") // vertex position
        {
            glm::vec4 position;
            int dimension;
            parseVector(parameters, 4, dimension, (float*)&position);
            if(dimension < 4) position.w = 1.0;

            if(dimension < 3)
            {
                error() << "could not load OBJ: wrong vertex position dimension" << std::endl;
                return data;
            }

            positionData.push_back(position);
        }
        else if(keyword == "vt") // vertex tex coord
        {
            glm::vec3 texCoord;
            int dimension;
            parseVector(parameters, 3, dimension, (float*)&texCoord);

            if(texCoordDimension < 0)
                texCoordDimension = dimension;
            else if(texCoordDimension != dimension)
            {
                error() << "could not load OBJ: contains mixed tex coord dimensions" << std::endl;
                return data;
            }

            hasTexCoords = true;

            texCoordData.push_back(texCoord);
        }
        else if(keyword == "vn") // vertex normal
        {
            glm::vec3 normal;
            int dimension;
            parseVector(parameters, 3, dimension, (float*)&normal);

            if(dimension < 3)
            {
                error() << "could not load OBJ: wrong vertex normal dimension" << std::endl;
                return data;
            }

            hasNormals = true;

            normalData.push_back(normal);
        }
        else if(keyword == "p") // point
        {
            if(primitiveType == GL_INVALID_ENUM)
                primitiveType = GL_POINTS;
            else if(primitiveType != GL_POINTS)
            {
                error() << "could not load OBJ: contains mixed primitive types" << std::endl;
                return data;
            }

            std::vector<IndexTuple> pointIndices = parseIndices(parameters);
            // points are just added in order
            for(size_t i = 0; i < pointIndices.size(); ++i)
            {
                indices.push_back(pointIndices[i]);
            }
        }
        else if(keyword == "l") // line
        {
            if(primitiveType == GL_INVALID_ENUM)
                primitiveType = GL_LINES;
            else if(primitiveType != GL_LINES)
            {
                error() << "could not load OBJ: contains mixed primitive types" << std::endl;
                return data;
            }

            std::vector<IndexTuple> lineIndices = parseIndices(parameters);
            // add line segments for the line strip defined by the vertices
            for(size_t i = 0; i < lineIndices.size() - 1; ++i)
            {
                indices.push_back(lineIndices[i]);
                indices.push_back(lineIndices[i+1]);
            }
        }
        else if(keyword == "f") // face
        {
            if(primitiveType == GL_INVALID_ENUM)
                primitiveType = GL_TRIANGLES;
            else if(primitiveType != GL_TRIANGLES)
            {
                error() << "could not load OBJ: contains mixed primitive types" << std::endl;
                return data;
            }

            std::vector<IndexTuple> faceIndices = parseIndices(parameters);
            // triangulate the polygon defined by the indices
            for(size_t i = 1; i < faceIndices.size() - 1; ++i)
            {
                indices.push_back(faceIndices[0]);
                indices.push_back(faceIndices[i]);
                indices.push_back(faceIndices[i+1]);
            }
        }
        else if(keyword == "bevel"      || keyword == "bmat"
             || keyword == "bsp"        || keyword == "bzp"
             || keyword == "c_interp"   || keyword == "cdc"
             || keyword == "cdp"        || keyword == "con"
             || keyword == "cstype"     || keyword == "ctech"
             || keyword == "curv"       || keyword == "curv2"
             || keyword == "d_interp"   || keyword == "deg"
             || keyword == "end"        || keyword == "g"
             || keyword == "hole"       || keyword == "lod"
             || keyword == "maplib"     || keyword == "mg"
             || keyword == "mtllib"     || keyword == "o"
             || keyword == "parm"       || keyword == "res"
             || keyword == "s"          || keyword == "scrv"
             || keyword == "shadow_obj" || keyword == "sp"
             || keyword == "stech"      || keyword == "step"
             || keyword == "surf"       || keyword == "trace_obj"
             || keyword == "trim"       || keyword == "usemap"
             || keyword == "usemtl"     || keyword == "vp")
        {
            // part of the OBJ specification (i.e. non-polygonal geometry, object groups, etc.)
            // is not supported and is silently ignored
        }
        else
        {
            warning() << "unknown OBJ keyword ignored: " << keyword << std::endl;
        }
    }

    if (!hasNormals && _computeNormals) {
        // perform own per-face normal creation only if the model had no own normals!
        if(primitiveType != GL_TRIANGLES)
        {
            warning() << "computing OBJ normals is only supported for models with faces" << std::endl;
            _computeNormals = false;
        }
        else
        {
            debug() << "model has no normals, computing face normals" << std::endl;
            hasNormals = true;
        }
    } else if (hasNormals) {
        // if the model has normals defined, no face normals have to get computed
        _computeNormals = false;
    }

    // all data are read from the file. construct an ArrayBuffer from the data
    data = SharedGeometryData(new GeometryData());

    size_t abDataElements = (positionDimension + hasTexCoords * texCoordDimension + hasNormals * normalDimension) * indices.size();
    GLfloat* abData = new GLfloat[abDataElements];

    size_t pos = 0;
    for(size_t i = 0; i < indices.size(); ++i)
    {
        const glm::vec4& position = positionData[indices[i].position];
        abData[pos++] = position.x;
        abData[pos++] = position.y;
        abData[pos++] = position.z;
        abData[pos++] = position.w;

        if(hasTexCoords)
        {
            const glm::vec3& texCoord = texCoordData[indices[i].texCoord];
            for(int dim = 0; dim < texCoordDimension; ++dim)
            {
                abData[pos++] = texCoord[dim];
            }
        }

        if(hasNormals)
        {
            if(_computeNormals)
            {
                size_t triangleIndex = i / 3;
                glm::vec3 v0 = (glm::vec3)positionData[indices[3 * triangleIndex + 0].position];
                glm::vec3 v1 = (glm::vec3)positionData[indices[3 * triangleIndex + 1].position];
                glm::vec3 v2 = (glm::vec3)positionData[indices[3 * triangleIndex + 2].position];
                glm::vec3 normal = glm::cross(v1 - v0, v2 - v0);
                if (normal != glm::vec3(0))
                    normal = glm::normalize(normal);
                abData[pos++] = normal.x;
                abData[pos++] = normal.y;
                abData[pos++] = normal.z;
            }
            else
            {
                const glm::vec3& normal = normalData[indices[i].normal];
                abData[pos++] = normal.x;
                abData[pos++] = normal.y;
                abData[pos++] = normal.z;
            }
        }
    }

    size_t strideSize = 0;
    ArrayBuffer::Attribute attrPosition = { "aPosition", GL_FLOAT, positionDimension, (GLuint)0, GL_FALSE, 0, GL_FALSE };
    strideSize += positionDimension * sizeof(GLfloat);

    data->mAttributes.push_back(attrPosition);

    if(hasTexCoords)
    {
        ArrayBuffer::Attribute attrTexCoord  = { "aTexCoord", GL_FLOAT, texCoordDimension, (GLuint)strideSize, GL_FALSE, 0, GL_FALSE };
        strideSize += texCoordDimension * sizeof(GLfloat);

        data->mAttributes.push_back(attrTexCoord);
    }

    if(hasNormals)
    {
        ArrayBuffer::Attribute attrNormal = { "aNormal", GL_FLOAT, normalDimension, (GLuint)strideSize, GL_FALSE, 0, GL_FALSE };
        strideSize += normalDimension * sizeof(GLfloat);

        data->mAttributes.push_back(attrNormal);
    }

    data->setStrideSize(strideSize);
    data->setSize(abDataElements * sizeof(GLfloat));
    data->setData((GLubyte*)abData);

    return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                           library specific save
///////////////////////////////////////////////////////////////////////////////////////////////////

} // OpenGL
} // ACGL
