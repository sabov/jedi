/*
 * This basic container stores the game world which the player is part of.
 *
 */
#pragma once

#include <ACGL/HardwareSupport/SimpleRiftController.hh>
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
#include <ACGL/OpenGL/Objects/ShaderProgram.hh>
#include "player.hh"
#include "events.hh"
#include "../audio/SimpleSound.hh"
#include "mesh/CGEMesh.h"
#include "mesh/CGEFullScreenQuad.h"
#include "math/CGETransformation.h"
#include "lights/CGEPointLight.h"
#include "lights/CGEDirectionalLight.h"
#include "lights/CGE_multiple_lights.h"
#include "process_sys/GLGProcessManager.h"
#include "process_sys/GLGExampleProcesses.h"
#include "gbuffer.hh"

class World {
public:
    World();
    ~World();

    // called once to connect the player with the Rift:
    void setPlayerCamera( ACGL::HardwareSupport::SimpleRiftController *riftControl );

    // render the world:
    void render();

    // move the player relative to the players bodys orientation:
    void movePlayer( glm::vec3 direction );

    // get the currect position in world space
    glm::vec3 getPlayerPosition();

    // returns the OpenAL compatible orientation which is the orientation on the head (ears)!
    void getPlayerOrientation( ALfloat *playerOrientation );

    // rotate the players body, negative values rotate to the left, positive to the right
    void rotatePlayer( float dYaw );

    // ducking value is between 0..1
    void duckPlayer( float duckingValue );

    void update(int time);

    void setWidthHeight(unsigned int _w, unsigned int _h);

    void DSRender();
    bool InitDS()   ;
    GLuint final_texture;

private:
    unsigned int window_width;
    unsigned int window_height;

    Player mPlayer;

    //Matrix Stack
    CGEngine::CMatrixStack  mMatrixStack;

    // The "level":
    CGEngine::CMesh mLevel  ;
    //test oject
    CGEngine::CMesh mDice   ;

    CGEngine::CFullScreenQuad mQuad;
    CGEngine::CGLTexture2D mTex;

    //using this shader since it supports textures
    ACGL::OpenGL::SharedShaderProgram     mBunnyShader;

    //Lights
    std::vector<CGEngine::CPositionalLight>     mPointLights   ;
    std::vector<CGEngine::CDirectionalLight>    mDirLights     ;

    //
    // One repeating sound as an example of how to use OpenAL:
    SimpleSound *mBeep;

    //process manager per level
    GameLogic::ProcessManagerPtr    mpProcessManager;
    GameLogic::RotationProcessPtr   mpRotProcess;

    //Deferred Shading
    bool use_direct_lighting ;
    ACGL::OpenGL::SharedShaderProgram   m_GeometryPassShader    ;
    GLint                               m_ColorTexUnitLoc       ;
    ACGL::OpenGL::SharedShaderProgram   m_PointLightPassShader  ;
    GLint   m_posTexLoc     ;
    GLint   m_colorTexLoc   ;
    GLint   m_normalTexLoc  ;
    GLint   m_screenSizeLoc ;
    GLint   m_eyeWorldPosLoc;
    //ACGL::OpenGL::SharedShaderProgram   m_DirLightPassShader    ;
    ACGL::OpenGL::SharedShaderProgram   m_NullShader            ;
    GBuffer                             m_GBuffer               ;
    CGEngine::CMesh                     m_bSphere               ;

    void DSGeometryPass();
    void DSStencilPass(unsigned int _PointLightIndex);
    void DSPointLightPass(unsigned int _PointLightIndex);
    void DSFinalPass();
    float CalcPointLightBSphere(const CGEngine::CPositionalLight& Light);
};
