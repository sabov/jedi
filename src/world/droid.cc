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
    //mLightsaberTexture  = loadTexture2D( "lightsaber.png" );
}

Droid::~Droid()
{
}

void Droid::render(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix)
{
    mDroidShader->use();

    glm::mat4 modelMatrix = glm::scale( glm::vec3( 0.01f ) );
    glm::mat4 translateMatrix = glm::translate( glm::mat4(), glm::vec3(getPosition().x, 1.0f, getPosition().z-0.5f ) );
    modelMatrix = translateMatrix * modelMatrix;

    mDroidShader->setUniform("uModelMatrix", modelMatrix);
    mDroidShader->setUniform("uViewMatrix", viewMatrix);
    mDroidShader->setUniform("uProjectionMatrix", projectionMatrix);
    mDroidShader->setUniform("uNormalMatrix", glm::inverseTranspose(glm::mat3(viewMatrix) * glm::mat3(modelMatrix)));

    //GLint textureUnit = 0;
    //mLightsaberTexture->bind( textureUnit );
    //mLightsaberShader->setUniform( "uTexture", textureUnit );

    mDroidGeometry->bind();
    mDroidGeometry->draw();
}
