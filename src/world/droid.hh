#pragma once

#include <ACGL/Scene/MoveableObject.hh>
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
#include <ACGL/OpenGL/Objects/ShaderProgram.hh>
#include "mesh/CGEMesh.h"
#include "process_sys/GLGProcess.h"
#include "PhysicsObject.hh"
#include <vector>

/*
 * Class for the Droid
 */
using namespace std;

class Droid : public ACGL::Scene::MoveableObject {
public:
    Droid();
    ~Droid();

    bool initialize(std::string _filename, glm::vec3 startPosition);

    void render(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix);
    void animate();
    void collide();

    glm::mat4 getModelMatrix() const { return mModelMatrix; }

    ACGL::OpenGL::SharedVertexArrayObject mDroidGeometry;

    void transformPosition() ;  //move droid, update position
    void baseRender();          //render geometry and texture

    PhysicsObject mPhysicObject;
    bool mDroidRenderFlag;

    GameLogic::ProcessPointer getMoveProcess() const { return mMoveProcess; }
    GameLogic::ProcessPointer getDestrucionProcess() const { return mDestructionProcess; }
private:
    //The droid
    ACGL::OpenGL::SharedShaderProgram     mDroidShader;
    CGEngine::CMesh                 mDroid                  ;
    std::vector<CGEngine::MeshPointer>    mDroidanimatedGeometry  ;

    glm::mat4 mModelMatrix   ;

    btCollisionShape* cShape = new btSphereShape(1);

    int animationFlag;

    //
    //Movement and Destruction Processes
    //
    class MoveProcess : public GameLogic::CProcess
    {
    public:
        MoveProcess() : GameLogic::CProcess()
        {
            mTransform = glm::mat4(1.0f);
        }
        ~MoveProcess() {}

        virtual void VOnInitialize();
        virtual void VOnUpdate(const int elapsedTim);
        virtual void VKill();

        glm::mat4 mTransform    ;
    };

    GameLogic::ProcessPointer   mMoveProcess ;

    class DestructionProcess : public GameLogic::CProcess
    {
    public:
        DestructionProcess() : GameLogic::CProcess() {}
        ~DestructionProcess() {}

        int animationIndex ;

        virtual void VOnInitialize();
        virtual void VOnUpdate(const int elapsedTime);
        virtual void VKill();
    };

    GameLogic::ProcessPointer   mDestructionProcess ;
};
