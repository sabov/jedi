#include "lights/CGELight.h"

namespace CGEngine {

CLight::CLight()
{
    m_Enabled       = false;
    m_Initialized   = false;
    //Position
    m_Position      = Vec4(0.0);
    //Ambient
    m_AmbientColor  = Vec4(1.0);
    //Diffuse
    m_DiffuseColor  = Vec4(1.0);
    //Ambient
    m_SpecularColor = Vec4(1.0);
}

CLight::~CLight()
{
}

bool CLight::Initialize(const Vec4 &_Position, const Vec4 &_AmbientColor, const Vec4 &_DiffuseColor, const Vec4 &_SpecularColor)
{
    m_Position      = _Position     ;
    m_AmbientColor  = _AmbientColor ;
    m_DiffuseColor  = _DiffuseColor ;
    m_SpecularColor = _SpecularColor;
    m_Enabled       = true          ;
    m_Initialized   = true          ;
    return true;
}

GLfloat *CLight::getPositionToArray() const
{
    GLfloat* array = new GLfloat[4];
    array[0] = m_Position[0] ; array[1] = m_Position[1] ;
    array[2] = m_Position[2] ; array[3] = m_Position[3] ;
    return array;
}

void CLight::Transform(const Matrix4x4 &_mat)
{
    m_Position = _mat * m_Position;
}

}   //End Namespace


