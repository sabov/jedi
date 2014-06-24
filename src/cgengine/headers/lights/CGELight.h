/****************************************************************************

  Base-Class for GL-Lights manipulations

  **************************************************************************/

#ifndef CGELIGHT_H
#define CGELIGHT_H

#ifndef __GLEW_H__
    #include <GL/glew.h>
#endif
#include <boost/smart_ptr/shared_ptr.hpp>
#include <vector>
#include <string>
#include "object/CGEObject.h"
#include "math/CGEGeometry.h"


#ifndef CGESHADER_H
    #ifdef _USE_CGE_SHADER
        #include "CGEShader/CGEShader.h"
    #else
        #include "sh_adapt/shader_adapter.h"
    #endif
#endif


namespace CGEngine {

//Class Header and Typedefs
class CLight    ;
typedef boost::shared_ptr<CLight>   LightPointer    ;

//Main Class
class CLight : public ICGEObject
{
protected:
    bool    m_Enabled       ;
    bool    m_Initialized   ;
    Vec4    m_Position      ;   //Position for Positional Light / Direction for Directional Light
    Vec4    m_AmbientColor  ;
    Vec4    m_DiffuseColor  ;
    Vec4    m_SpecularColor ;

    //Shader-related stuff
    std::string     m_ShStructName      ;   //Is the name of the uniform struct-variable in the GLSL-Shader
    std::string     m_ShAmbientName     ;   //Is the name of the field of the uniform struct-variable in the GLSL-Shader
    std::string     m_ShDiffuseName     ;   //Is the name of the field of the uniform struct-variable in the GLSL-Shader
    std::string     m_ShSpecName        ;   //Is the name of the field of the uniform struct-variable in the GLSL-Shader

public:
    CLight() ;
    virtual ~CLight() ;

    //Initialize Light (enables current Light)
    bool Initialize(const Vec4& _Position, const Vec4& _AmbientColor,
                    const Vec4& _DiffuseColor, const Vec4& _SpecularColor) ;

    //Enable current light
    void Enable()
    {
        m_Enabled = true ;
    }

    //Disable current light
    void Disable()
    {
        m_Enabled = false;
    }

    //Get light position as array of floats
    GLfloat* getPositionToArray() const;

    //Get position/direction of point light/directional light
    Vec4 getPosition() const
    {
        return m_Position;
    }

    //Get position of point light
    Vec3 getPositionV3() const
    {
        return Vec3(m_Position[0], m_Position[1], m_Position[2]);
    }

    //Get Ambient Lightcolor
    Vec4 getAmbientColor() const
    {
        return m_AmbientColor;
    }
    //Get Diffuse Lightcolor
    Vec4 getDiffuseColor() const
    {
        return m_DiffuseColor;
    }
    //Get Specular Lightcolor
    Vec4 getSpecularColor() const
    {
        return m_SpecularColor;
    }
    //Is this light a point light?
    bool isPointLight() const
    {
        return ( m_Position[3] == 1.0f );
    }
    //Is this light a directional light?
    bool isDirectionalLight() const
    {
        return ( m_Position[3] == 0.0f );
    }

    //Set new position/direction of point light/directional light
    void setPosition(const Vec4& _newPosition)
    {
        m_Position = _newPosition;
    }
    //Set Ambient Colorlight
    void setAmbientColor(const Vec4& _color)
    {
        m_AmbientColor = _color;
    }
    //Set Diffuse Colorlight
    void setDiffuseColor(const Vec4& _color)
    {
        m_DiffuseColor = _color;
    }
    //Set Specular Colorlight
    void setSpecularColor(const Vec4& _color)
    {
        m_SpecularColor = _color;
    }
    //Sets the name of the Light-Structure variable in the Shader
    void SetStructName(std::string& _structName)
    {
        m_ShStructName = _structName;
    }
    //Sets the name of the AmbientColor-Field in the Light-Structure variable in the Shader
    void SetAmbientName(std::string& _ambName)
    {
        m_ShAmbientName = _ambName;
    }
    //Sets the name of the DiffuseColor-Field in the Light-Structure variable in the Shader
    void SetDiffuseName(std::string& _diffName)
    {
        m_ShDiffuseName = _diffName;
    }
    //Sets the name of the SpecularColor-Field in the Light-Structure variable in the Shader
    void SetSpecularName(std::string& _specName)
    {
        m_ShSpecName = _specName;
    }

    //Gets the name of the Light-Structure variable in the Shader
    std::string GetStructName()
    {
        return m_ShStructName ;
    }
    //Gets the name of the AmbientColor-Field in the Light-Structure variable in the Shader
    std::string GetAmbientName()
    {
        return m_ShAmbientName ;
    }
    //Gets the name of the DiffuseColor-Field in the Light-Structure variable in the Shader
    std::string GetDiffuseName()
    {
        return m_ShDiffuseName;
    }
    //Gets the name of the SpecularColor-Field in the Light-Structure variable in the Shader
    std::string GetSpecularName()
    {
        return m_ShSpecName ;
    }
    //Transform light position
    void Transform(const Matrix4x4& _mat);


    //Send Light-Information to Shader
    virtual bool VSendToShader(CShader& _shader) = 0;


};


}//End Namespace

#endif // CGELIGHT_H
