#include "textures/CGETexture2D.h"
#include <QString>

namespace CGEngine {

bool CGLTexture2D::VLoadTexture(const std::string& _fileName) {
    QImage tmp, img ;

    // Bild aus Datei laden
    if ( !tmp.load(QString( _fileName.c_str() )) )
       return false ;

   // QImage in OpenGL-Format wandeln (umdrehen und Farben tauschen)
   img = QGLWidget::convertToGLFormat(tmp);

   m_Width = img.width() ;
   m_Height = img.height() ;

   //Textur auf Grafikkarte hochladen
   glGenTextures(1, &m_TextureID);

   glBindTexture(GL_TEXTURE_2D, m_TextureID);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(),
               img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
               img.bits());
   glBindTexture(GL_TEXTURE_2D, 0);

   return true ;
}

void CGLTexture2D::SetTexParami(const GLenum _name, const GLint _param) {
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexParameteri(GL_TEXTURE_2D, _name, _param);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void CGLTexture2D::SetTexParamf(const GLenum _name, const GLfloat _param) {
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexParameteri(GL_TEXTURE_2D, _name, _param);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void CGLTexture2D::Bind(const GLenum _ActiveTextureUnit) {
    glActiveTexture(_ActiveTextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_TextureID) ;

}

}   //End Namespace
