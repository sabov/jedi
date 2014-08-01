/*
 * This basic container stores the game world which the player is part of.
 *
 */
#pragma once

#include <ACGL/HardwareSupport/SimpleRiftController.hh>
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
#include <ACGL/OpenGL/Objects/ShaderProgram.hh>
#include <ACGL/OpenGL/Objects/FrameBufferObject.hh>

#include <btBulletDynamicsCommon.h>
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
#include "process_sys/GLGProcess.h"
#include "gbuffer.hh"
#include "droid.hh"
#include "PhysicsObject.hh"

//using namespace ci;
//using namespace ci::app;
//using namespace ci::gl;

class World {
public:
    World();
    ~World();

    bool initializeWorld();

    // called once to connect the player with the Rift:
    void setPlayerCamera(ACGL::HardwareSupport::SimpleRiftController *riftControl);

    // render the world:
    void geometryRender(); //render geometry (geometry pass)

    // move the player relative to the players body orientation:
    void movePlayer(const glm::vec3 &direction);
    void DSBlurPass();

    // get the current position in world space
    glm::vec3 getPlayerPosition();

    // returns the OpenAL compatible orientation which is the orientation on the head (ears)!
    void getPlayerOrientation(ALfloat *playerOrientation);

    // rotate the players body, negative values rotate to the left, positive to the right
    void rotatePlayer(float dYaw);

    // ducking value is between 0..1
    void duckPlayer(float duckingValue);

    // player is currently using the force
    void useForcePlayer();

    // move the lightsaber of the player
    void moveLightsaber(const glm::vec3 &direction);

    // set the position of the lightsaber of the player
    void setLightsaberPosition(const glm::vec3 &movement);

    //turn on/off lightsaber
    void toggleLightsaber();

    //rotate the lightsaber, negative values rotate to the left, positive to the right
    void rotateLightsaber(float yaw, float pitch, float roll);

    //set the rotation matrix of the lightsaber
    void setRotationMatrixLightsaber(const glm::mat4 &rotation);

    //initialize bullet physics engine
    void initializeBullet();

    void update(int time);

    void setWidthHeight(unsigned int _w, unsigned int _h);

    //Deferred Shading
    void DSRender();
    bool InitDS();

    void SetRenderFBO(GLuint _fbo)
    {
        m_GBuffer.setRenderFBOName(_fbo);
    }

    void BlitCurrentDSBuffer();

    GLuint mFboScene;
    GLuint mFboBlur1;
    GLuint mFboBlur2;

private:
    unsigned int window_width;
    unsigned int window_height;

    Player mPlayer;
    std::vector<Droid> mDroids;

    //Bullet
    btDiscreteDynamicsWorld* dynamicsWorld;
    PhysicsObject *droidsPhysic;
    void updatePhysics() ;

    //Matrix Stack
    CGEngine::CMatrixStack mMatrixStack;

    // The "level":
    CGEngine::CMesh mLevel  ;

    //Lights
    std::vector<CGEngine::CPositionalLight>     mPointLights    ;
    std::vector<CGEngine::CDirectionalLight>    mDirLights      ;
    std::vector<CGEngine::CSpotLight>           mSpotLights     ;

    //One process manager per level
    GameLogic::ProcessManagerPtr mpProcessManager;

    //Deferred Shading
    ACGL::OpenGL::SharedShaderProgram m_GeometryPassShader  ;
    ACGL::OpenGL::SharedShaderProgram m_PointLightPassShader;
    ACGL::OpenGL::SharedShaderProgram m_DirLightPassShader  ;
    ACGL::OpenGL::SharedShaderProgram m_SpotLightPassShader ;
    ACGL::OpenGL::SharedShaderProgram m_NullShader          ;
    GLint m_ColorTexUnitLoc     ;
    GLint m_posTexLoc[3]        ;
    GLint m_colorTexLoc[3]      ;
    GLint m_normalTexLoc[3]     ;
    GLint m_screenSizeLoc[3]    ;
    GLint m_eyeWorldPosLoc[3]   ;

    GBuffer                     m_GBuffer   ; //Geometry buffer (manages geometry, diffuse and normal maps for DS)
    CGEngine::CMesh             m_Sphere    ; //rendering Spheres for Point Lights
    CGEngine::CFullScreenQuad   m_Quad      ; //rendering Fullscreenquad for Directional Light
    CGEngine::CMesh             m_Cone      ; //rendering Cones for Spot Lights

    void DSGeometryPass()                                               ;
    void DSStencilPass(unsigned int _PointLightIndex)                   ;
    void DSPointLightPass(unsigned int _PointLightIndex)                ;
    void DSDirectionalLightPass()                                       ;
    void DSSpotStencilPass(unsigned int _SpotLightIndex)                ;
    void DSSpotLightPass(unsigned int _SpotLightIndex)                  ;
    void DSFinalPass()                                                  ;
    float CalcPointLightBSphere(const CGEngine::CPositionalLight& Light);

    //Physics process (updates physic processes like collisions in our scene)
    friend class BulletPhysicsProcess;

    class BulletPhysicsProcess : public GameLogic::CProcess
    {
    private:
        World*  m_World ;
    public:
        BulletPhysicsProcess(World* _World) : GameLogic::CProcess()
        {
            m_World = _World;
        }
        ~BulletPhysicsProcess()
        {
            m_World = NULL;
        }

        virtual void VOnInitialize();
        virtual void VKill();
        virtual void VOnUpdate(const int elapsedTime);
    };

    typedef boost::shared_ptr<BulletPhysicsProcess> PhysicsProcessPtr   ;
    PhysicsProcessPtr m_BulletPhysicsProcess   ;
};
