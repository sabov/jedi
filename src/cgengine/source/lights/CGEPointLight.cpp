#include "lights/CGEPointLight.h"

namespace CGEngine {

bool CPositionalLight::Initialize(const Vec4 &_Position, const Vec4 &_AmbientColor, const Vec4 &_DiffuseColor, const Vec4 &_SpecularColor,
                                  float _ConstAttenuation, float _LinAttenuation, float _ExpAttenuation)
{
    bool b = CLight::Initialize(_Position, _AmbientColor, _DiffuseColor, _SpecularColor);
    m_Position[3] = 1.0f;
    m_const_att = _ConstAttenuation;
    m_lin_att = _LinAttenuation;
    m_exp_att = _ExpAttenuation;
    return b;
}


bool CPositionalLight::VSendToShader(CShader &_shader)
{
    if (!m_Enabled) return false;
    bool success = false;
    std::string point(".");
    //Determine complete uniform names to structure
    std::string direction   = m_ShStructName + point + m_ShPositionName     ;
    std::string ambient     = m_ShStructName + point + m_ShAmbientName      ;
    std::string diffuse     = m_ShStructName + point + m_ShDiffuseName      ;
    std::string specular    = m_ShStructName + point + m_ShSpecName         ;
    std::string constatt    = m_ShStructName + point + m_ShConstAttribName  ;
    std::string linatt      = m_ShStructName + point + m_ShLinAttribName    ;
    std::string expatt      = m_ShStructName + point + m_ShExpAttribName    ;
    //Send values to the shader (shader checks itself for validity and activity)
    success = _shader.SetUniformValues(CGE_FLOAT_VEC4,  direction,  (void*)glm::value_ptr(m_Position)     );
    success = _shader.SetUniformValues(CGE_FLOAT_VEC4,  ambient,    (void*)glm::value_ptr(m_AmbientColor) ) && success;
    success = _shader.SetUniformValues(CGE_FLOAT_VEC4,  diffuse,    (void*)glm::value_ptr(m_DiffuseColor) ) && success;
    success = _shader.SetUniformValues(CGE_FLOAT_VEC4,  specular,   (void*)glm::value_ptr(m_SpecularColor)) && success;
    success = _shader.SetUniformValues(CGE_FLOAT,       constatt,   (void*)&m_const_att      ) && success;
    success = _shader.SetUniformValues(CGE_FLOAT,       linatt,     (void*)&m_lin_att        ) && success;
    success = _shader.SetUniformValues(CGE_FLOAT,       expatt,     (void*)&m_exp_att        ) && success;
    return success;
}

}   //End Namespace
