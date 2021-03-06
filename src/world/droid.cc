#include "droid.hh"
#include <ACGL/OpenGL/Creator/VertexArrayObjectCreator.hh>
#include <ACGL/OpenGL/Managers.hh>
#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>
#include <ACGL/OpenGL/Data/TextureLoadStore.hh>


using namespace std;
using namespace ACGL;
using namespace ACGL::Utils;
using namespace ACGL::OpenGL;

Droid::Droid()
{
}

Droid::~Droid()
{
    cout << "deleting droid..." << endl;
    delete cShape;
    if (mPhysicObject.rigidBody)
        delete mPhysicObject.rigidBody;
}

bool Droid::initialize(string _filename, glm::vec3 startPosition)
{
    droidStartPosition = startPosition;
    debug() << "loading droid..." << endl;
    mDroidShader   = ShaderProgramFileManager::the()->get( ShaderProgramCreator("droidShader") );
    mDroid = CGEngine::MeshPointer( new CGEngine::CMesh() );
    bool ret = mDroid->LoadMesh(_filename);
    mDroidRenderFlag = true;
    rigidflag = true;
    setPosition(startPosition);
    mPhysicObject.Init(cShape, startPosition);
    mModelMatrix = glm::scale( glm::vec3( 0.2f ) );
    mModelMatrix = glm::translate(glm::mat4(1.0), startPosition) * mModelMatrix;

    mMoveProcess = GameLogic::ProcessPointer( new Droid::MoveProcess(this) );
    /*
     * The destruction process is started as soon as a collision is registered
     */
    mDestructionProcess = boost::shared_ptr<Droid::DestructionProcess> ( new Droid::DestructionProcess(this) );

    string droidfile;
    int animVariant = rand() % 3;
    mAnimationFlag = false;
    int last_index = 0;
    for (int i = 0; i < 50; i++) {//!
        if (i < 10){
            droidfile  = "droidanim" +  std::to_string(animVariant) + "/Drone166_00000" + std::to_string(i) + ".obj";
            //droidfile  = std::string("test_anim") + std::string("/droid_anim_00000") + std::to_string(i) + ".obj";
        } else {
            droidfile = "droidanim" +  std::to_string(animVariant) + "/Drone166_0000" + std::to_string(i) + ".obj";
            //droidfile  = std::string("test_anim")  + std::string("/droid_anim_0000") + std::to_string(i) + ".obj";
        }

        mDroidanimatedGeometry.push_back( CGEngine::MeshPointer( new CGEngine::CMesh() ) );
        mDroidanimatedGeometry[last_index]->LoadMesh(droidfile);
        last_index++;
    }
    mPhysicObject.rigidBody->setUserPointer(this);
    return ret;
}

void Droid::reInit(){
    mAnimationFlag = false;
    mDroidRenderFlag = true;
    mModelMatrix = glm::scale( glm::vec3( 0.2f ) );
    mModelMatrix = glm::translate(glm::mat4(1.0), droidStartPosition) * mModelMatrix;
    setPosition(droidStartPosition);
    mPhysicObject.SetPosition(droidStartPosition);
}

void Droid::render(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix)
{
    mDroidShader->use();

    glm::mat4 modelMatrix = glm::scale( glm::vec3( 0.8f ) );

    glm::mat4 translateMatrix = glm::translate( glm::mat4(), getPosition());
    modelMatrix = translateMatrix * modelMatrix;

    mDroidShader->setUniform("uModelMatrix", modelMatrix);
    mDroidShader->setUniform("uViewMatrix", viewMatrix);
    mDroidShader->setUniform("uProjectionMatrix", projectionMatrix);
    mDroidShader->setUniform("uNormalMatrix", glm::inverseTranspose(glm::mat3(viewMatrix) * glm::mat3(modelMatrix)));

    mDroid->VOnDraw();
}

void Droid::transformPosition()
{
    glm::mat4 translateMatrix = glm::translate( glm::mat4(), getPosition() );
    mModelMatrix = translateMatrix * mModelMatrix;

    mPhysicObject.SetPosition(getPosition());
}

void Droid::baseRender(glm::vec3 moveDirection)
{
    if (mDroidRenderFlag)
    {
        mPhysicObject.SetPosition(getPosition());
        mDroid->VOnDraw();
        moveDroid(moveDirection);
    }
    else if ( mAnimationFlag )
    {
        mDroidanimatedGeometry[mDestructionProcess->animationIndex]->VOnDraw();
    }
    else
        return;
}

void Droid::moveDroid(glm::vec3 moveDirection){
    glm::mat4 translateMatrix = glm::translate( glm::mat4(), moveDirection );
    mModelMatrix = translateMatrix * mModelMatrix;
    move(moveDirection);
}

void Droid::MoveProcess::VOnInitialize()
{
    mTransform              = glm::mat4(1.0f);
    this->m_Kill            = false ;
    this->m_Active          = true ;
    this->m_InitialUpdate   = true ;
    this->m_Paused          = false ;
}

void Droid::MoveProcess::VOnUpdate(const int elapsedTime)
{
    /*
     * update "mModelMatrix" here, for example mModelMatrix = translateMatrix * mModelMatrix
     * for some translateMatrix or whatever
     */
    glm::vec3 moveDirection =glm::vec3(0.03f, 0.0f, 0.05f);
    glm::mat4 translateMatrix = glm::translate( glm::mat4(), moveDirection );
    mDroid->mModelMatrix = translateMatrix * mDroid->mModelMatrix;
    mDroid->move(moveDirection);
}

void Droid::MoveProcess::VKill()
{
    this->m_Kill            = true ;
    this->m_Active          = false ;
    this->m_InitialUpdate   = false ;
}

void Droid::DestructionProcess::VOnInitialize()
{
    animationIndex          = 0;
    this->m_Kill            = false ;
    this->m_Active          = true ;
    this->m_InitialUpdate   = true ;
    this->m_Paused          = false ;
}

void Droid::DestructionProcess::VKill()
{
    this->m_Kill            = true ;
    this->m_Active          = false ;
    this->m_InitialUpdate   = false ;
}

void Droid::DestructionProcess::VOnUpdate(const int elapsedTime)
{
    animationIndex++;
    if ( animationIndex >= (int)mDroid->mDroidanimatedGeometry.size() )
    {
        mDroid->mAnimationFlag = false;
        VKill();
    }
}
