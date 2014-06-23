/*****************************************************************
 *
 * Adapter Class for Shader-Objects to use with CGEngine
 *
 *****************************************************************/

#pragma once

#include <ACGL/OpenGL/GL.hh>
#include <GLFW/glfw3.h>
#include <ACGL/OpenGL/Objects/ShaderProgram.hh>

#ifndef SHADER_ADAPER_H
#define SHADER_ADAPER_H

namespace CGEngine {

enum CGEUniformType
{
    CGE_FLOAT, CGE_FLOAT_VEC2, CGE_FLOAT_VEC3, CGE_FLOAT_VEC4, CGE_INT, CGE_INT_VEC2, CGE_INT_VEC3, CGE_INT_VEC4,
    CGE_FLOAT_ARRAY, CGE_FLOAT_VEC2_ARRAY, CGE_FLOAT_VEC3_ARRAY, CGE_FLOAT_VEC4_ARRAY,
    CGE_INT_ARRAY, CGE_INT_VEC2_ARRAY, CGE_INT_VEC3_ARRAY, CGE_INT_VEC4_ARRAY,
    CGE_MATRIX2, CGE_MATRIX3, CGE_MATRIX4, CGE_MATRIX2_ARRAY, CGE_MATRIX3_ARRAY, CGE_MATRIX4_ARRAY
};

class CShader
{
private:
    CShader() {}
    CShader(const CShader&) {}
public:

    CShader(const ACGL::OpenGL::SharedShaderProgram& _shader_program)
    {
        shaderProgram = _shader_program;
    }

    ~CShader() {}

    ACGL::OpenGL::SharedShaderProgram shaderProgram   ;

    bool SetUniformValues(CGEUniformType _UniformType,
                          const std::string& _UniformName,
                          void* _Values,
                          const int& _NumArrayEntries = 1,
                          bool _TransposeMatrix = false)
    {
        bool ret = true;
        GLuint h = shaderProgram->getObjectName();
        GLint loc = glGetUniformLocation( shaderProgram->getObjectName(), _UniformName.c_str() );
        switch ( _UniformType )
        {
        case CGE_FLOAT:
            shaderProgram->setUniform( loc, *(static_cast<GLfloat*>(_Values)) );
            if ( glGetError() != GL_NO_ERROR )
                ret = false;
            break;
        case CGE_FLOAT_VEC4:
            shaderProgram->setUniform( loc, 4, static_cast<GLfloat*>(_Values) );
            if ( glGetError() != GL_NO_ERROR )
                ret = false;
            break;
        default:
            ret = false;
            break;
        }
        return ret;
    }


};

}   // end namespace

#endif // SHADER_ADAPER_H
