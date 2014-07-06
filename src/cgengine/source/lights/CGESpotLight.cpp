#include "lights/CGESpotLight.h"


bool CGEngine::CSpotLight::Initialize(const CGEngine::Vec4 &_Position,
                                      const CGEngine::Vec4 &_SpotDirection,
                                      const CGEngine::Vec4 &_AmbientColor,
                                      const CGEngine::Vec4 &_DiffuseColor,
                                      const CGEngine::Vec4 &_SpecularColor,
                                      float _ConstAttenuation,
                                      float _LinAttenuation,
                                      float _ExpAttenuation,
                                      float _SpotInnerCutOffAngle,
                                      float _SpotOuterCutOffAngle)
{
    bool b              = CLight::Initialize(_Position, _AmbientColor, _DiffuseColor, _SpecularColor);
    m_Position[3]       = 1.0f;
    m_SpotDirection     = _SpotDirection;
    m_SpotDirection[3]  = 0.0f;
    m_const_att         = _ConstAttenuation;
    m_lin_att           = _LinAttenuation;
    m_exp_att           = _ExpAttenuation;
    m_SpotInnerCutOff   = cos( _SpotInnerCutOffAngle );
    m_SpotOuterCutOff   = cos( _SpotOuterCutOffAngle );
    return b;
}

bool CGEngine::CSpotLight::VSendToShader(CGEngine::CShader &_shader)
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
    std::string spotdir     = m_ShStructName + point + m_ShSpotDirectionName;
    std::string spotcutoff1 = m_ShStructName + point + m_ShSpotCutOffName1  ;
    std::string spotcutoff2 = m_ShStructName + point + m_ShSpotCutOffName2  ;
    //Send values to the shader (shader checks itself for validity and activity)
    success = _shader.SetUniformValues(CGE_FLOAT_VEC4,  direction,  (void*)glm::value_ptr(m_Position)     );
    success = _shader.SetUniformValues(CGE_FLOAT_VEC4,  ambient,    (void*)glm::value_ptr(m_AmbientColor) ) && success;
    success = _shader.SetUniformValues(CGE_FLOAT_VEC4,  diffuse,    (void*)glm::value_ptr(m_DiffuseColor) ) && success;
    success = _shader.SetUniformValues(CGE_FLOAT_VEC4,  specular,   (void*)glm::value_ptr(m_SpecularColor)) && success;
    success = _shader.SetUniformValues(CGE_FLOAT,       constatt,   (void*)&m_const_att      ) && success;
    success = _shader.SetUniformValues(CGE_FLOAT,       linatt,     (void*)&m_lin_att        ) && success;
    success = _shader.SetUniformValues(CGE_FLOAT,       expatt,     (void*)&m_exp_att        ) && success;
    success = _shader.SetUniformValues(CGE_FLOAT_VEC4,  spotdir,    (void*)glm::value_ptr(m_SpotDirection)) && success;
    success = _shader.SetUniformValues(CGE_FLOAT,       spotcutoff1,(void*)&m_SpotInnerCutOff) && success;
    success = _shader.SetUniformValues(CGE_FLOAT,       spotcutoff2,(void*)&m_SpotOuterCutOff) && success;
    return success;
}

void CGEngine::CSpotLight::Transform(const CGEngine::Matrix4x4 &_mat)
{
    m_Position      = _mat * m_Position;
    m_SpotDirection = _mat * m_SpotDirection;
}
