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
    debug() << "loading droid..." << endl;
    mDroidGeometry = VertexArrayObjectCreator("droid.obj").create();
    mDroidShader   = ShaderProgramFileManager::the()->get( ShaderProgramCreator("droidShader") );
    mDroidGeometry->setAttributeLocations( mDroidShader->getAttributeLocations() );
    mDroidRenderFlag = true;
    //setPosition(startPosition);
    //mPhysicObject.Init(cShape, startPosition);
    string droidfilie;
    int animVariant = rand() % 3;
    animationFlag = 0;
    for (int i = 0; i < 50; i++) {
        if (i < 10){
           droidfilie  = "droidanim" +  std::to_string(animVariant) + "/Drone166_00000" + std::to_string(i) + ".obj";
        } else {
           droidfilie = "droidanim" +  std::to_string(animVariant) + "/Drone166_0000" + std::to_string(i) + ".obj";
        }

        //mDroidanimatedGeometry.push_back(VertexArrayObjectCreator(droidfilie).create());
    }

}

Droid::~Droid()
{
    cout << "deleting droid..." << endl;
    delete cShape;
}

bool Droid::initialize(string _filename, glm::vec3 startPosition)
{
    debug() << "loading droid..." << endl;
    mDroidShader   = ShaderProgramFileManager::the()->get( ShaderProgramCreator("droidShader") );
    bool ret = mDroid.LoadMesh(_filename);
    mDroidRenderFlag = true;
    setPosition(startPosition);
    mPhysicObject.Init(cShape, startPosition);
    mModelMatrix = glm::scale( glm::vec3( 0.8f ) );
    mModelMatrix = glm::translate(glm::mat4(1.0), startPosition) * mModelMatrix;

    mMoveProcess = GameLogic::ProcessPointer( new Droid::MoveProcess() );
    /*
     * The destruction process is started as soon as a collision is registered,
     * i.e. see world.cc, line 167
     */
    mDestructionProcess = GameLogic::ProcessPointer ( new Droid::DestructionProcess() );

    string droidfile;
    int animVariant = rand() % 3;
    animationFlag = 0;
    int last_index = 0;
    for (int i = 0; i < 50; i++) {
        if (i < 10){
           droidfile  = "droidanim" +  std::to_string(animVariant) + "/Drone166_00000" + std::to_string(i) + ".obj";
        } else {
           droidfile = "droidanim" +  std::to_string(animVariant) + "/Drone166_0000" + std::to_string(i) + ".obj";
        }

        mDroidanimatedGeometry.push_back( CGEngine::MeshPointer( new CGEngine::CMesh() ) );
        mDroidanimatedGeometry[last_index]->LoadMesh(droidfile);
        last_index++;
    }

    return ret;
}

void Droid::render(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix)
{
    mDroidShader->use();

    glm::mat4 modelMatrix = glm::scale( glm::vec3( 0.8f ) );

    glm::mat4 translateMatrix = glm::translate( glm::mat4(), getPosition());
    modelMatrix = translateMatrix * modelMatrix;

    if (mDroidRenderFlag){
        mPhysicObject.SetPosition(getPosition());
    }else{
        setPosition(mPhysicObject.GetPosition());
    }
    mPhysicObject.rigidBody->setUserPointer(this);

    mDroidShader->setUniform("uModelMatrix", modelMatrix);
    mDroidShader->setUniform("uViewMatrix", viewMatrix);
    mDroidShader->setUniform("uProjectionMatrix", projectionMatrix);
    mDroidShader->setUniform("uNormalMatrix", glm::inverseTranspose(glm::mat3(viewMatrix) * glm::mat3(modelMatrix)));

    mDroid.VOnDraw();
}

void Droid::transformPosition()
{
    glm::mat4 translateMatrix = glm::translate( glm::mat4(), getPosition() );
    mModelMatrix = translateMatrix * mModelMatrix;

    mPhysicObject.SetPosition(getPosition());
    mPhysicObject.rigidBody->setUserPointer(this);
}

void Droid::baseRender()
{
    if (mDroidRenderFlag){
        mPhysicObject.SetPosition(getPosition());
    }else{
        setPosition(mPhysicObject.GetPosition());
    }
    mPhysicObject.rigidBody->setUserPointer(this);
    mDroid.VOnDraw();
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
    /*
     * render the different meshes for the animation: mDroidanimatedGeometry[animationIndex]->VOnDraw()
     * the variable animationIndex is the same as animationflag, it should be increased after each call of this function,
     * or after a certain time period.
     * If animationIndex is greater than 50, the process should be killed ( call VKill() ), or we will get errors.
     */
}

void Droid::animate(){
    /*
    if (animationFlag < mDroidanimatedGeometry.size()-1 )
    {
        animationFlag++;
        mDroidGeometry = mDroidanimatedGeometry[animationFlag];
        mDroidGeometry->setAttributeLocations( mDroidShader->getAttributeLocations() );
    }
    */
}

void Droid::collide(){
    animate();
}
