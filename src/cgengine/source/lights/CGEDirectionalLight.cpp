#include "lights/CGEDirectionalLight.h"

namespace CGEngine {

bool CDirectionalLight::Initialize(const Vec4 &_Position, const Vec4 &_AmbientColor, const Vec4 &_DiffuseColor, const Vec4 &_SpecularColor)
{
    bool b = CLight::Initialize(_Position, _AmbientColor, _DiffuseColor, _SpecularColor);
    m_Position[3] = 0.0f;
    return b;
}


bool CDirectionalLight::VSendToShader(CShader &_shader)
{
    if (!m_Enabled) return false;
    bool success = false;
    std::string point(".");
    //Determine complete uniform names to structure
    std::string direction   = m_ShStructName + point + m_ShDirectionName    ;
    std::string ambient     = m_ShStructName + point + m_ShAmbientName      ;
    std::string diffuse     = m_ShStructName + point + m_ShDiffuseName      ;
    std::string specular    = m_ShStructName + point + m_ShSpecName         ;
    //Send values to the shader (shader checks itself for validity and activity)
    success = _shader.SetUniformValues(CGE_FLOAT_VEC4, direction, (void*)glm::value_ptr(m_Position)     );
    success = _shader.SetUniformValues(CGE_FLOAT_VEC4, ambient,   (void*)glm::value_ptr(m_AmbientColor) ) && success;
    success = _shader.SetUniformValues(CGE_FLOAT_VEC4, diffuse,   (void*)glm::value_ptr(m_DiffuseColor) ) && success;
    success = _shader.SetUniformValues(CGE_FLOAT_VEC4, specular,  (void*)glm::value_ptr(m_SpecularColor)) && success;
    return success;
}



}   //end Namespace
