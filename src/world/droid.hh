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
class Droid;
typedef boost::shared_ptr<Droid> DroidPtr;

class Droid : public ACGL::Scene::MoveableObject {
public:
    Droid();
    ~Droid();

    friend class MoveProcess;
    friend class DestructionProcess;

    bool initialize(std::string _filename, glm::vec3 startPosition);
    void reInit();

    void render(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix);

    glm::mat4 getModelMatrix() const { return mModelMatrix; }

    void transformPosition() ;  //move droid, update position
    void baseRender(glm::vec3 moveDirection);          //render geometry and texture
    void moveDroid(glm::vec3 movedirection);

    PhysicsObject mPhysicObject;

    void setRenderFlag(bool _b) { mDroidRenderFlag = _b; }
    void setAnimationFlag(bool _b) { mAnimationFlag = _b; }


    GameLogic::ProcessPointer getMoveProcess() const { return mMoveProcess; }
    GameLogic::ProcessPointer getDestrucionProcess() const { return mDestructionProcess; }

    glm::vec3 moveDirection;
    glm::vec3 droidStartPosition;
    bool rigidflag;
private:
    //The droid
    ACGL::OpenGL::SharedShaderProgram       mDroidShader            ;
    CGEngine::MeshPointer                         mDroid                  ;
    std::vector<CGEngine::MeshPointer>      mDroidanimatedGeometry  ;

    glm::mat4 mModelMatrix   ;

    btCollisionShape* cShape = new btSphereShape(0.3);

    bool mAnimationFlag;
    bool mDroidRenderFlag;

    //
    //Movement and Destruction Processes
    //
    class MoveProcess : public GameLogic::CProcess
    {
    private:
        Droid* mDroid;
    public:
        MoveProcess(Droid* droid) : GameLogic::CProcess()
        {
            mDroid = droid;
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
    private:
        Droid* mDroid;
    public:
        DestructionProcess() : GameLogic::CProcess() {}
        DestructionProcess(Droid* droid) {mDroid = droid;}
        ~DestructionProcess() {}

        int animationIndex ;

        virtual void VOnInitialize();
        virtual void VOnUpdate(const int elapsedTime);
        virtual void VKill();
    };

    boost::shared_ptr<DestructionProcess>   mDestructionProcess ;
};
