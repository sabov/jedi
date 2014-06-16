/*
 * This basic container stores the game world which the player is part of.
 *
 */
#pragma once

#include <ACGL/HardwareSupport/SimpleRiftController.hh>
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
#include <ACGL/OpenGL/Objects/ShaderProgram.hh>
#include "player.hh"
#include "../audio/SimpleSound.hh"
#include "mesh/CGEMesh.h"
#include "math/CGETransformation.h"

class World {
public:
    World();
    ~World();

    // called once to connect the player with the Rift:
    void setPlayerCamera(ACGL::HardwareSupport::SimpleRiftController *riftControl);

    // render the world:
    void render();

    // move the player relative to the players body orientation:
    void movePlayer(const glm::vec3 &direction);

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

    //rotate the lightsaber, negative values rotate to the left, positive to the right
    void rotateLightsaber(float dYaw, float dRoll, float dPitch);

private:
    Player mPlayer;

    //Matrix Stack
    CGEngine::CMatrixStack  mMatrixStack;

    // The "level":
    CGEngine::CMesh mLevel ;

    //using this shader since it supports textures
    ACGL::OpenGL::SharedShaderProgram mBunnyShader;
    
    // One repeating sound as an example of how to use OpenAL:
    SimpleSound *mBeep;
};
