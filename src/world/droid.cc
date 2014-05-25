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
}

Droid::~Droid()
{
}

void Droid::render(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix, glm::vec3 positon)
{
    mDroidShader->use();

    glm::mat4 modelMatrix = glm::scale( glm::vec3( 0.8f ) );

    glm::mat4 translateMatrix = glm::translate( glm::mat4(),
                                                glm::vec3(getPosition().x + positon.x,
                                                          getPosition().y + positon.y,
                                                          getPosition().z + positon.z ) );
    modelMatrix = translateMatrix * modelMatrix;

    mDroidShader->setUniform("uModelMatrix", modelMatrix);
    mDroidShader->setUniform("uViewMatrix", viewMatrix);
    mDroidShader->setUniform("uProjectionMatrix", projectionMatrix);
    mDroidShader->setUniform("uNormalMatrix", glm::inverseTranspose(glm::mat3(viewMatrix) * glm::mat3(modelMatrix)));

    mDroidGeometry->bind();
    mDroidGeometry->draw();
}
