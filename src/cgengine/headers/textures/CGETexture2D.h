/****************************************************************************************

  Class for 2D-Textures with OpenGL

*****************************************************************************************/

#ifndef __GLEW_H__
    #include <GL/glew.h>
#endif
#include <boost/smart_ptr/shared_ptr.hpp>
#include "CGEQGLTextures.h"

#ifndef CGETEXTURE2D_H
#define CGETEXTURE2D_H

namespace CGEngine {

//Class Header and typedefs
class CGLTexture2D;
typedef boost::shared_ptr<CGLTexture2D> GLTexture2DPointer  ;

//Class for 2D-Textures with OpenGL
class CGLTexture2D : public CQGLTexture {
private:
    CGLTexture2D(const CGLTexture2D&) {}

protected:
    int m_Width   ;
    int m_Height  ;

public:
    CGLTexture2D() : CQGLTexture() {
        m_Width  = 0 ;
        m_Height = 0 ;
    }

    virtual ~CGLTexture2D() {
        glDeleteTextures(1, &m_TextureID);
    }

    virtual bool VLoadTexture(const std::string& _fileName) ;

    void Bind(const GLenum _ActiveTextureUnit) ;

    void SetTexParami(const GLenum _name, const GLint _param);
    void SetTexParamf(const GLenum _name, const GLfloat _param);

    //Getter-Functions
    int getWidth() const {
        return m_Width ;
    }

    int getHeight() const {
        return m_Height ;
    }

};

}   //End Namespace

#endif // CGETEXTURE2D_H
