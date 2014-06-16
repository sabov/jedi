/**************************************************************************************************

  Abstract Base-Class for Textures with QT and OpenGL

***************************************************************************************************/

#ifndef __GLEW_H__
    #include <GL/glew.h>
#endif
#include <string>
#include <QtOpenGL/QGLWidget>
#include <QImage>
#include "object/CGEObject.h"

#ifndef CGEQGLTEXTURES_H
#define CGEQGLTEXTURES_H

namespace CGEngine {

class CQGLTexture : public ICGEObject {
protected:
    GLuint m_TextureID ;

    CQGLTexture() : ICGEObject()
    {
        m_TextureID = 0;
    }

public:
    //Overloadables
    virtual ~CQGLTexture() {}

    virtual bool VLoadTexture(const std::string& _fileName) = 0 ;

    //Getter-Functions

    GLuint getTextureId() const
    {
        return m_TextureID ;
    }

};

}   //End Namespace

#endif // CGEQGLTEXTURES_H
