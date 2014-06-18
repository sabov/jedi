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
    modelMatrix = translateMatrix * getRotationMatrix4() * modelMatrix;

    mLightsaberShader->setUniform("uModelMatrix", modelMatrix);
    mLightsaberShader->setUniform("uViewMatrix", viewMatrix);
    mLightsaberShader->setUniform("uProjectionMatrix", projectionMatrix);
    mLightsaberShader->setUniform("uNormalMatrix", glm::inverseTranspose(glm::mat3(viewMatrix) * glm::mat3(modelMatrix)));

    mPhysicObject.SetPosition(getPosition());
    mPhysicObject.rigidBody->setUserPointer(this);

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
    //Reset rotation for movement
    glm::mat3 tmpRotationMatrix = mRotationMatrix;
    mRotationMatrix = glm::mat3();
    ACGL::Scene::MoveableObject::move(direction);
    //Set rotation again
    mRotationMatrix = tmpRotationMatrix;

    //Check bounds
    //x-axis
    if (mPosition.x < mPlayerPosition.x - lowDistanceToPlayer.x) {
        mPosition.x = mPlayerPosition.x - lowDistanceToPlayer.x;
    } else if (mPosition.x > mPlayerPosition.x + upDistanceToPlayer.x) {
        mPosition.x = mPlayerPosition.x + upDistanceToPlayer.x;
    }
    //y-axis
    if (mPosition.y < mPlayerPosition.y + lowDistanceToPlayer.y) {
        mPosition.y = mPlayerPosition.y + lowDistanceToPlayer.y;
    } else if (mPosition.y > mPlayerPosition.y + upDistanceToPlayer.y) {
        mPosition.y = mPlayerPosition.y + upDistanceToPlayer.y;
    }
    //z-axis
    if (mPosition.z > mPlayerPosition.z - lowDistanceToPlayer.z) {
        mPosition.z = mPlayerPosition.z - lowDistanceToPlayer.z;
    } else if (mPosition.z < mPlayerPosition.z - upDistanceToPlayer.z) {
        mPosition.z = mPlayerPosition.z - upDistanceToPlayer.z;
    }
}

/*
 * Rotate lightsaber
 */
void Lightsaber::rotate(float yaw, float roll, float pitch) {
    glm::mat4 R = getRotationMatrix4();
    glm::mat4 newRot = glm::yawPitchRoll(yaw, roll, pitch);
    setRotationMatrix(R * newRot);
}
