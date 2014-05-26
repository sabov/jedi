#include "lightsaber.hh"
#include <ACGL/OpenGL/Creator/VertexArrayObjectCreator.hh>
#include <ACGL/OpenGL/Managers.hh>
#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>
#include <ACGL/OpenGL/Data/TextureLoadStore.hh>

using namespace std;
using namespace ACGL;
using namespace ACGL::Utils;
using namespace ACGL::OpenGL;

Lightsaber::Lightsaber(const glm::vec3 &playerPosition) {
    debug() << "loading lightsaber..." << endl;
    mLightsaberGeometry = VertexArrayObjectCreator("lightsaber.obj").create();
    mLightsaberShader = ShaderProgramFileManager::the()->get(ShaderProgramCreator("lightsaberShader"));
    mLightsaberGeometry->setAttributeLocations(mLightsaberShader->getAttributeLocations());
    mLightsaberTexture = loadTexture2D("lightsaber.png");

    setPlayerPosition(playerPosition);
    setPosition(glm::vec3(mPlayerPosition.x, mPlayerPosition.y + 1.0f, mPlayerPosition.z - 0.5f));
}

Lightsaber::~Lightsaber() {
}

void Lightsaber::render(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix) {
    mLightsaberShader->use();

    glm::mat4 modelMatrix = glm::scale(glm::vec3(0.01f));
    glm::mat4 translateMatrix = glm::translate(glm::mat4(), getPosition());
    modelMatrix = translateMatrix * modelMatrix;

    mLightsaberShader->setUniform("uModelMatrix", modelMatrix);
    mLightsaberShader->setUniform("uViewMatrix", viewMatrix);
    mLightsaberShader->setUniform("uProjectionMatrix", projectionMatrix);
    mLightsaberShader->setUniform("uNormalMatrix", glm::inverseTranspose(glm::mat3(viewMatrix) * glm::mat3(modelMatrix)));

    GLint textureUnit = 0;
    mLightsaberTexture->bind(textureUnit);
    mLightsaberShader->setUniform("uTexture", textureUnit);

    mLightsaberGeometry->bind();
    mLightsaberGeometry->draw();
}

/*
 * Move lightsaber in space
 */
void Lightsaber::move(const glm::vec3 &direction) {
    ACGL::Scene::MoveableObject::move(direction);

    if (mPosition.x < mPlayerPosition.x - lowDistanceToPlayer.x) {
        mPosition.x = mPlayerPosition.x - lowDistanceToPlayer.x;
    } else if (mPosition.x > mPlayerPosition.x + upDistanceToPlayer.x) {
        mPosition.x = mPlayerPosition.x + upDistanceToPlayer.x;
    }

    if (mPosition.y < mPlayerPosition.y + lowDistanceToPlayer.y) {
        mPosition.y = mPlayerPosition.y + lowDistanceToPlayer.y;
    } else if (mPosition.y > mPlayerPosition.y + upDistanceToPlayer.y) {
        mPosition.y = mPlayerPosition.y + upDistanceToPlayer.y;
    }
}