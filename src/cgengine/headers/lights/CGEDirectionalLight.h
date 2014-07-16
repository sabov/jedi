/***********************************************************************************************************

  Class for directional light

***********************************************************************************************************/

#ifndef CGEDIRECTIONALLIGHT_H
#define CGEDIRECTIONALLIGHT_H

#ifndef CGELIGHT_H
#include "CGELight.h"
#endif

namespace CGEngine {

class CDirectionalLight ;
typedef boost::shared_ptr<CDirectionalLight>    DirectionalLightPointer ;

class CDirectionalLight : public CLight
{
protected:
    std::string     m_ShDirectionName   ;   //Is the name of the field of the uniform struct-variable in the GLSL-Shader

public:

    CDirectionalLight() : CLight(), m_ShDirectionName("Direction")
    {
        m_ShStructName  = std::string("DirectionalLight")   ;
        m_ShAmbientName = std::string("AmbientColor")       ;
        m_ShDiffuseName = std::string("DiffuseColor")       ;
        m_ShSpecName    = std::string("SpecularColor")      ;
    }

    virtual ~CDirectionalLight() {}

    bool Initialize(const Vec4 &_Position, const Vec4 &_AmbientColor, const Vec4 &_DiffuseColor, const Vec4 &_SpecularColor);


    //Send Light-Information to Shader
    virtual bool VSendToShader(CShader &_shader);

    //Sets the name of the Direction-Field in the Directional Light-Structure variable in the Shader
    void SetDirectionName(std::string& _DirectionName)
    {
        m_ShDirectionName = _DirectionName;
    }

    //Gets the name of the Directional Light-Structure variable in the Shader
    std::string GetDirectionName()
    {
        return m_ShDirectionName ;
    }

};

} //End Namespace

#endif // CGEDIRECTIONALLIGHT_H
