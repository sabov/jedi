#include "droid.hh"
#include <ACGL/OpenGL/Creator/VertexArrayObjectCreator.hh>
#include <ACGL/OpenGL/Managers.hh>
#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>
#include <ACGL/OpenGL/Data/TextureLoadStore.hh>


using namespace std;
using namespace ACGL;
using namespace ACGL::Utils;
using namespace ACGL::OpenGL;

Droid::Droid(glm::vec3 startPosition)
{
    debug() << "loading droid..." << endl;
    mDroidGeometry = VertexArrayObjectCreator("droid.obj").create();
    mDroidShader   = ShaderProgramFileManager::the()->get( ShaderProgramCreator("droidShader") );
    mDroidGeometry->setAttributeLocations( mDroidShader->getAttributeLocations() );
    mDroidRenderFlag = true;
    setPosition(startPosition);
    mPhysicObject.Init(cShape, startPosition);
    string droidfilie;
    animationFlag = 0;
    for (int i = 0; i < 40; i++) {
        if (i < 10){
           droidfilie  = "driodanim/droidanim_00000" + std::to_string(i) + ".obj";
        } else {
           droidfilie = "driodanim/droidanim_0000" + std::to_string(i) + ".obj";
        }

        mDroidanimatedGeometry.push_back(VertexArrayObjectCreator(droidfilie).create());
    }

}

Droid::~Droid()
{
    cout << "deleting droid..." << endl;
    delete cShape;
}

void Droid::render(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix)
{
    mDroidShader->use();

    glm::mat4 modelMatrix = glm::scale( glm::vec3( 0.8f ) );

    glm::mat4 translateMatrix = glm::translate( glm::mat4(),
                                                glm::vec3(getPosition().x,
                                                          getPosition().y,
                                                          getPosition().z) );
    modelMatrix = translateMatrix * modelMatrix;

    if (mDroidRenderFlag){
        mPhysicObject.SetPosition(getPosition());
    }
    mPhysicObject.rigidBody->setUserPointer(this);

    mDroidShader->setUniform("uModelMatrix", modelMatrix);
    mDroidShader->setUniform("uViewMatrix", viewMatrix);
    mDroidShader->setUniform("uProjectionMatrix", projectionMatrix);
    mDroidShader->setUniform("uNormalMatrix", glm::inverseTranspose(glm::mat3(viewMatrix) * glm::mat3(modelMatrix)));

    //animate();
    mDroidGeometry->bind();
    mDroidGeometry->draw();
}

void Droid::animate(){
    if (animationFlag < mDroidanimatedGeometry.size()-1 )
    {
        animationFlag++;
        mDroidGeometry = mDroidanimatedGeometry[animationFlag];
    }
}
