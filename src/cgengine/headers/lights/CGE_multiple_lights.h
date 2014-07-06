#ifndef CGE_MULTIPLE_LIGHTS_H
#define CGE_MULTIPLE_LIGHTS_H

#ifndef CGELIGHT_H
    #include "CGELight.h"
#endif
#ifndef CGEDIRECTIONALLIGHT_H
    #include "CGEDirectionalLight.h"
#endif
#ifndef CGESPOTLIGHT_H
    #include "CGESpotLight.h"
#endif
#ifndef CGEPOINTLIGHT_H
    #include "CGEPointLight.h"
#endif
#ifndef CGESHADER_H
    #ifdef _USE_CGE_SHADER
        #include "CGEShader/CGEShader.h"
    #else
        #include "sh_adapt/shader_adapter.h"
    #endif
#endif

#include <string>
#include <sstream>
#include <cassert>

//For light import
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace CGEngine {

/*
  Send the arrays of lights given in the containers to the specified shader. The function expects that each of
  the directional lights and the positional lights have the same structure name in the shader
  (given by "GetStructName()"), and that
  each struct in the shader only needs to be addressed by adding the index suffix "[index]". The function makes no
  changes to the fields of the light structs on the software side.
*/
template <class DirLContainer, class PosLContainer>
bool SendLightsToShaderConst(CShader& _shader,
                        DirLContainer& _directionalLights,
                        PosLContainer& _positionalLights)
{
    int num_dir_lights = (int)_directionalLights.size();
    int num_pt_lights = (int)_positionalLights.size();
    std::string brackets[2] = {std::string("["), std::string("]")};
    //Send directional lights to shader
    if ( num_dir_lights > 0 )
    {
        int i = 0;
        typename DirLContainer::iterator it_begin = _directionalLights.begin();
        typename DirLContainer::iterator it_end = _directionalLights.end();
        std::string struct_name = it_begin->GetStructName(); //original struct name
        for ( typename DirLContainer::iterator it = it_begin ; it != it_end ; ++it, ++i )
        {
            //Adjust Name of light-struct, so that it's found in the shader
            //Note: std::ostringstream is used to convert numerical values to strings
            it->SetStructName( struct_name + brackets[0] + static_cast<std::ostringstream*>(&(std::ostringstream() << i))->str() + brackets[1] );
            it->VSendToShader( _shader );
            it->SetStructName( struct_name ); //set struct-name back to original struct-name
        }
    }
    //Send point-lights to shader
    if ( num_pt_lights > 0 )
    {
        int i = 0;
        typename PosLContainer::iterator it_begin = _positionalLights.begin();
        typename PosLContainer::iterator it_end = _positionalLights.end();
        std::string struct_name = it_begin->GetStructName();
        for ( typename PosLContainer::iterator it = it_begin ; it != it_end ; ++it, ++i )
        {
            it->SetStructName( struct_name + brackets[0] + static_cast<std::ostringstream*>(&(std::ostringstream() << i))->str() + brackets[1] );
            it->VSendToShader( _shader );
            it->SetStructName( struct_name );
        }
    }

    return true;
}

/*
 * Load lights from a given file, normally a scene file (*.dae, *.blend, etc).
 * Uses Assimp-Library to load a scene in which the lights are contained.
 * It distinguishes between different light sources and fills the arrays correspondingly.
 * All previous data in the arrays is lost.
 * returns true if all lights could be loaded correctly.
 */
template <class DirLContainer, class PosLContainer, class SpotLContainer>
bool LoadLightsFromFile(std::string _FileName,
                        DirLContainer& _directionalLights,
                        PosLContainer& _positionalLights,
                        SpotLContainer& _spotLights)
{
    bool ret = true;

    Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(_FileName.c_str(), 0);

    aiNode* root = pScene->mRootNode;
    aiMatrix4x4 root_m = root->mTransformation;

    if (pScene)
    {
        if ( !pScene->HasLights() )
            return false;

        _directionalLights.clear();
        _positionalLights.clear();
        _spotLights.clear();
        unsigned int numLights = pScene->mNumLights;
        aiColor3D   ambient;
        aiColor3D   diffuse;
        aiColor3D   specular;
        for (unsigned int i = 0; i < numLights; ++i)
        {
            aiLight* paiLight = pScene->mLights[i] ;
            std::string light_name( paiLight->mName.C_Str() );

            aiNode* child = root->mChildren[i]; //This probably only works as long as the scene does not contain other things than lights
            aiMatrix4x4 m = root_m * child->mTransformation;
            std::string child_name( child->mName.C_Str() );

            assert( light_name.compare( child_name ) == 0 ) ;

            switch ( paiLight->mType )
            {
            case aiLightSource_DIRECTIONAL:
            {
                CDirectionalLight dirlight;

                aiVector3D  direction   = paiLight->mDirection      ;
                ambient     = paiLight->mColorAmbient   ;
                diffuse     = paiLight->mColorDiffuse   ;
                specular    = paiLight->mColorSpecular  ;

                dirlight.Initialize(
                            Vec4(direction[0], direction[1], direction[2], 0.0),
                            Vec4(ambient[0], ambient[1], ambient[2], 1.0),
                            Vec4(diffuse[0], diffuse[1], diffuse[2], 1.0),
                            Vec4(specular[0], specular[1], specular[2], 1.0)
                            );

                _directionalLights.push_back( dirlight );

                ret = ret && true;
                break;
            }
            case aiLightSource_POINT:
            {
                CPositionalLight ptlight;

                aiVector3D  position    = m * paiLight->mPosition       ;
                ambient     = paiLight->mColorAmbient   ;
                diffuse     = paiLight->mColorDiffuse   ;
                specular    = paiLight->mColorSpecular  ;
                float       const_att   = paiLight->mAttenuationConstant    ;
                float       lin_att     = paiLight->mAttenuationLinear      ;
                float       exp_att     = paiLight->mAttenuationQuadratic   ;

                ptlight.Initialize(
                            Vec4(position[0], position[1], position[2], 1.0),
                            Vec4(ambient[0], ambient[1], ambient[2], 1.0),
                            Vec4(diffuse[0], diffuse[1], diffuse[2], 1.0),
                            Vec4(specular[0], specular[1], specular[2], 1.0),
                            const_att, lin_att, exp_att
                            );

                _positionalLights.push_back( ptlight );

                ret = ret && true;
                break;
            }
            case aiLightSource_SPOT:
            {
                CSpotLight sptlight;
                aiVector3D  position    = m * paiLight->mPosition       ;
                aiVector3D  direction   = m * paiLight->mDirection      ;
                ambient     = paiLight->mColorAmbient   ;
                diffuse     = paiLight->mColorDiffuse   ;
                specular    = paiLight->mColorSpecular  ;
                float       const_att   = paiLight->mAttenuationConstant    ;
                float       lin_att     = paiLight->mAttenuationLinear      ;
                float       exp_att     = paiLight->mAttenuationQuadratic   ;
                float       cut_off1    = paiLight->mAngleInnerCone         ;
                float       cut_off2    = paiLight->mAngleOuterCone         ;

                sptlight.Initialize(
                        Vec4(position[0], position[1], position[2], 1.0),
                        Vec4(direction[0], direction[1], direction[2], 0.0),
                        Vec4(ambient[0], ambient[1], ambient[2], 1.0),
                        Vec4(diffuse[0], diffuse[1], diffuse[2], 1.0),
                        Vec4(specular[0], specular[1], specular[2], 1.0),
                        const_att, lin_att, exp_att, cut_off1, cut_off2);

                _spotLights.push_back( sptlight );

                ret = ret && true;
                break;
            }
            case aiLightSource_UNDEFINED:
            default:
            {
                std::cerr << "Light Importer: Error occured while loading " << _FileName.c_str() << ". Encountered unknown light type or error!" << std::endl ;
                ret = ret && false;
                break;
            }
            }
        }

    }
    else
    {
        std::cerr << "Light Importer: Error parsing " << _FileName.c_str() << " Assimp: " << Importer.GetErrorString() << std::endl ;
        ret = false;
    }

    if ( !ret )
        std::cerr << "Light Importer Error: Some lights were not imported correctly." << std::endl;

    return ret;
}

}   //End Namespace

#endif // CGE_MULTIPLE_LIGHTS_H
