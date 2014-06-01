#pragma once

#include <ACGL/Scene/MoveableObject.hh>
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
#include <ACGL/OpenGL/Objects/ShaderProgram.hh>
#include "PhysicsObject.hh"

/*
 * Class for the lightsaber of the player
 */

class Lightsaber : public ACGL::Scene::MoveableObject {
public:
    Lightsaber();
    ~Lightsaber();

    void render(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix);

    PhysicsObject mPhysicObject;
private:
    //The lightsaber
    ACGL::OpenGL::SharedVertexArrayObject mLightsaberGeometry;
    ACGL::OpenGL::SharedShaderProgram     mLightsaberShader;
    ACGL::OpenGL::SharedTexture2D         mLightsaberTexture;
};
