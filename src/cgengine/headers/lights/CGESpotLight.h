#ifndef CGESPOTLIGHT_H
#define CGESPOTLIGHT_H

#ifndef CGEPOINTLIGHT_H
#include "CGEPointLight.h"
#endif

namespace CGEngine {

class   CSpotLight ;
typedef boost::shared_ptr<CSpotLight>   SpotLightPtr    ;

class CSpotLight : public CPositionalLight
{
protected:
    Vec4        m_SpotDirection         ;   //Direction of Spot Light
    float       m_SpotInnerCutOff       ;   //Cosine of inner cut-off angle
    float       m_SpotOuterCutOff       ;   //Cosine of outer cut-off angle

    std::string m_ShSpotDirectionName   ;
    std::string m_ShSpotCutOffName1     ;
    std::string m_ShSpotCutOffName2     ;

public:

    CSpotLight() : CPositionalLight()
    {
        m_ShStructName          = std::string("SpotLight")      ;
        m_ShSpotDirectionName   = std::string("SpotDirection")  ;
        m_ShSpotCutOffName1     = std::string("SpotCutOff1")    ;
        m_ShSpotCutOffName2     = std::string("SpotCutOff2")    ;
        m_SpotDirection         = Vec4(0.0)                     ;
        m_SpotInnerCutOff       = 30.0f                         ;
        m_SpotOuterCutOff       = 45.0f                         ;
    }

    ~CSpotLight() {}

    //Initialize Point Light
    bool Initialize(const Vec4 &_Position,
                    const Vec4 &_SpotDirection,
                    const Vec4 &_AmbientColor,
                    const Vec4 &_DiffuseColor,
                    const Vec4 &_SpecularColor,
                    float _ConstAttenuation = 0.8f,
                    float _LinAttenuation = 0.1f,
                    float _ExpAttenuation = 0.1f,
                    float _SpotInnerCutOffAngle = 30.0f,
                    float _SpotOuterCutOffAngle = 45.0f);

    //Send Light-Information to Shader
    virtual bool VSendToShader(CShader &_shader);

    //Transform light position/direction
    void Transform(const Matrix4x4& _mat);

    Vec4 GetSpotDirection() const
    {
        return m_SpotDirection;
    }

    float GetSpotCutOff() const
    {
        return m_SpotInnerCutOff;
    }

    std::string GetSpotDirectionName() const
    {
        return m_ShSpotDirectionName;
    }

    std::string GetSpotInnerCutOffName() const
    {
        return m_ShSpotCutOffName1;
    }

    std::string GetSpotOuterCutOffName() const
    {
        return m_ShSpotCutOffName2;
    }

    void SetSpotDirectionName(std::string &_Name)
    {
        m_ShSpotDirectionName = _Name;
    }

    void SetSpotInnerCutOffName(std::string &_Name)
    {
        m_ShSpotCutOffName1 = _Name;
    }

    void SetSpotOuterCutOffName(std::string &_Name)
    {
        m_ShSpotCutOffName2 = _Name;
    }

    void SetSpotInnerCutOff(const float &_CutOff)
    {
        m_SpotInnerCutOff = cos(_CutOff);
    }

    void SetSpotOuterCutOff(const float &_CutOff)
    {
        m_SpotOuterCutOff = cos(_CutOff);
    }
};

}

#endif // CGESPOTLIGHT_H
