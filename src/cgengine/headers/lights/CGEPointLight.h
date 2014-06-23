/***********************************************************************************************************

  Class for positional light

***********************************************************************************************************/

#ifndef CGEPOINTLIGHT_H
#define CGEPOINTLIGHT_H

#ifndef CGELIGHT_H
#include "CGELight.h"
#endif

namespace CGEngine {

class CPositionalLight   ;
typedef boost::shared_ptr<CPositionalLight>  CPositionalLightPointer   ;

class CPositionalLight : public CLight
{
protected:
    std::string m_ShPositionName    ;   //Is the name of the field of the uniform struct-variable in the GLSL-Shader
    std::string m_ShConstAttribName ;   //Is the name of the field of the uniform struct-variable in the GLSL-Shader
    std::string m_ShLinAttribName   ;   //Is the name of the field of the uniform struct-variable in the GLSL-Shader
    std::string m_ShExpAttribName   ;   //Is the name of the field of the uniform struct-variable in the GLSL-Shader
    float       m_const_att         ;   //Constant attenuation
    float       m_lin_att           ;   //Linear attenuation
    float       m_exp_att           ;   //Exponential attenuation (also called quadratic attenuation)

public:

    CPositionalLight() : CLight(),
        m_ShPositionName("Position"),
        m_ShConstAttribName("ConstantAttenuation"),
        m_ShLinAttribName("LinearAttenuation"),
        m_ShExpAttribName("ExponentialAttenuation"),
        m_const_att(0.8f), m_lin_att(0.1f), m_exp_att(0.1f)
    {
        m_ShStructName  = std::string("PositionalLight")    ;
        m_ShAmbientName = std::string("AmbientColor")       ;
        m_ShDiffuseName = std::string("DiffuseColor")       ;
        m_ShSpecName    = std::string("SpecularColor")      ;
    }

    bool Initialize(const Vec4 &_Position, const Vec4 &_AmbientColor, const Vec4 &_DiffuseColor, const Vec4 &_SpecularColor,
                    float _ConstAttenuation = 0.8f, float _LinAttenuation = 0.1f, float _ExpAttenuation = 0.1f);

    virtual ~CPositionalLight() {}


    //Send Light-Information to Shader
    virtual bool VSendToShader(CShader &_shader);


    //Sets the name of the Position-Field in the Positional-Light-Structure variable in the Shader
    void SetPositionName(std::string& _PositionName)
    {
        m_ShPositionName = _PositionName;
    }
    //Sets the name of the Const.Att. -Field in the Positional-Light-Structure variable in the Shader
    void SetConstAttenuationName(std::string& _ConstAttName)
    {
        m_ShConstAttribName = _ConstAttName;
    }
    //Sets the name of the Lin.Att. -Field in the Positional-Light-Structure variable in the Shader
    void SetLinAttenuationName(std::string& _LinAttName)
    {
        m_ShLinAttribName = _LinAttName;
    }
    //Sets the name of the Exp.Att. -Field in the Positional-Light-Structure variable in the Shader
    void SetExpAttenuationName(std::string& _ExpAttName)
    {
        m_ShExpAttribName = _ExpAttName;
    }
    //Sets constant attenuation factor
    void SetConstantAttenuation(const float& _constAttenuation)
    {
        m_const_att = _constAttenuation;
    }
    //Sets linear attenuation factor
    void SetLinearAttenuation(const float& _linearAttenuation)
    {
        m_lin_att = _linearAttenuation;
    }
    //Sets exponential attenuation factor
    void SetExponentialAttenuation(const float& _expAttenuation)
    {
        m_exp_att = _expAttenuation;
    }

    //Gets the name of the Positional Light-Structure variable in the Shader
    std::string GetPositionName()
    {
        return m_ShPositionName ;
    }
    //Gets the name of the Const. Att. -Field in the Light-Structure variable in the Shader
    std::string GetConstantAttenuationName()
    {
        return m_ShConstAttribName ;
    }
    //Gets the name of the Lin. Att. -Field in the Light-Structure variable in the Shader
    std::string GetLinearAttenuationName()
    {
        return m_ShLinAttribName ;
    }
    //Gets the name of the Exp. Att. -Field in the Light-Structure variable in the Shader
    std::string GetExponentialAttenuationName()
    {
        return m_ShExpAttribName ;
    }

    float GetLinAttenuation() const
    {
        return m_lin_att;
    }

    float GetConstAttenuation() const
    {
        return m_const_att;
    }

    float GetExpAttenuation() const
    {
        return m_exp_att;
    }

};

}   //End Namespace

#endif // CGEPOINTLIGHT_H
