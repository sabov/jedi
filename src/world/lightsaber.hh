#pragma once

#include <ACGL/Scene/MoveableObject.hh>
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
#include <ACGL/OpenGL/Objects/ShaderProgram.hh>

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

    void render(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix);

    void move(const glm::vec3 &direction); //Overwrite of parent method

    void rotate(float yaw, float roll, float pitch);
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
};
