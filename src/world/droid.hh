#pragma once

#include <ACGL/Scene/MoveableObject.hh>
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
#include <ACGL/OpenGL/Objects/ShaderProgram.hh>
#include "PhysicsObject.hh"

/*
 * Class for the Droid
 */

class Droid : public ACGL::Scene::MoveableObject {
public:
    Droid();
    ~Droid();

    void render(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix, glm::vec3 positon);

    PhysicsObject mPhysicObject;
private:
    //The droid
    ACGL::OpenGL::SharedVertexArrayObject mDroidGeometry;
    ACGL::OpenGL::SharedShaderProgram     mDroidShader;
    ACGL::OpenGL::SharedTexture2D         mDroidTexture;
};
