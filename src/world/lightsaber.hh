#pragma once

#include <ACGL/Scene/MoveableObject.hh>
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
#include <ACGL/OpenGL/Objects/ShaderProgram.hh>
#include "PhysicsObject.hh"

/*
 * Class for the lightsaber of the player
 */

class Lightsaber: public ACGL::Scene::MoveableObject {
public:
    Lightsaber(const glm::vec3 &playerPosition);
    ~Lightsaber();

    void setPlayerPosition(const glm::vec3 &playerPosition) {
        mPlayerPosition = playerPosition;
    }

    void render(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);

    void move(const glm::vec3 &direction); //Overwrite of parent method

    void setPosition(const glm::vec3 &position);

    void rotate(float yaw, float pitch, float roll);

    void toggle();

    PhysicsObject mPhysicObject;

private:
    // Current position of player
    glm::vec3 mPlayerPosition;

    //The lightsaber
    ACGL::OpenGL::SharedVertexArrayObject mLightsaberGeometry;
    ACGL::OpenGL::SharedVertexArrayObject mRayGeometry;

    ACGL::OpenGL::SharedShaderProgram mLightsaberShader;
    ACGL::OpenGL::SharedShaderProgram mRayShader;

    ACGL::OpenGL::SharedTexture2D mLightsaberTexture;

    //Bounds for distance of lightsaber from player
    const glm::vec3 upDistanceToPlayer = glm::vec3(0.5f, 1.5f, 0.8f);
    const glm::vec3 lowDistanceToPlayer = glm::vec3(0.5f, 0.5f, 0.2f);

    bool turnedOn;
    float rayHeight;

    float yaw;
    float pitch;
    float roll;

    btCollisionShape* cShape = new btCylinderShape(btVector3(0.1, 1, 0.1));
};
