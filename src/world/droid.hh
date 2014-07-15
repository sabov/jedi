#pragma once

#include <ACGL/Scene/MoveableObject.hh>
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
#include <ACGL/OpenGL/Objects/ShaderProgram.hh>
#include "PhysicsObject.hh"
#include <vector>

/*
 * Class for the Droid
 */
using namespace std;

class Droid : public ACGL::Scene::MoveableObject {
public:
    Droid(glm::vec3 startPosition);
    ~Droid();

    void render(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix);
    void animate();
    void collide();

    PhysicsObject mPhysicObject;
    bool mDroidRenderFlag;
private:
    //The droid
    ACGL::OpenGL::SharedVertexArrayObject mDroidGeometry;
    vector<ACGL::OpenGL::SharedVertexArrayObject> mDroidanimatedGeometry;

    ACGL::OpenGL::SharedShaderProgram     mDroidShader;
    ACGL::OpenGL::SharedTexture2D         mDroidTexture;

    btCollisionShape* cShape = new btSphereShape(1);
    int animationFlag;
};
