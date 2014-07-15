#include "lightsaber.hh"
#include <ACGL/OpenGL/Creator/VertexArrayObjectCreator.hh>
#include <ACGL/OpenGL/Managers.hh>
#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>
#include <ACGL/OpenGL/Data/TextureLoadStore.hh>

using namespace std;
using namespace ACGL;
using namespace ACGL::Utils;
using namespace ACGL::OpenGL;

Lightsaber::Lightsaber(const glm::vec3 &playerPosition){
    debug() << "loading lightsaber..." << endl;

    turnedOn = false;
    rayHeight = 0;
    yaw = 0;
    pitch = 0;
    roll = 0;

    mLightsaberGeometry = VertexArrayObjectCreator("lightsaber.obj").create();
    mRayGeometry = VertexArrayObjectCreator("ray.obj").create();

    mLightsaberShader = ShaderProgramFileManager::the()->get(ShaderProgramCreator("lightsaberShader"));
    mRayShader = ShaderProgramFileManager::the()->get(ShaderProgramCreator("ray"));

    mLightsaberGeometry->setAttributeLocations(mLightsaberShader->getAttributeLocations());
    mRayGeometry->setAttributeLocations(mRayShader->getAttributeLocations());

    mLightsaberTexture = loadTexture2D("lightsaber.png");

    setPlayerPosition(playerPosition);
    setPosition(glm::vec3(mPlayerPosition.x, mPlayerPosition.y + 1.0f, mPlayerPosition.z - 0.5f));
    mPhysicObject.Init(cShape, getPosition());

    /*
    mPhysicObject.rigidBody->applyTorque(btVector3(0.9, 0, 0));
    */

    /*
    btTransform tr = mPhysicObject.rigidBody->getCenterOfMassTransform();
    //tr.setIdentity();
    btQuaternion quat;
    quat.setEuler(0, 0.8, 0); //or quat.setEulerZYX depending on the ordering you want
    tr.setRotation(quat);
    mPhysicObject.rigidBody->setCenterOfMassTransform(tr);
    */

    /*
    tr.setIdentity();
    quat.setEuler(0, 0.78, 0); //or quat.setEulerZYX depending on the ordering you want
    tr.setRotation(quat);
    mPhysicObject.rigidBody->setCenterOfMassTransform(tr);
    */
}

Lightsaber::~Lightsaber() {
    cout << "deleting lightsaber..." << endl;
    delete cShape;
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

    btTransform tr;
    tr.setIdentity();
    btQuaternion quat;
    quat.setEuler(0, roll, pitch); //or quat.setEulerZYX depending on the ordering you want
    tr.setRotation(quat);
    mPhysicObject.rigidBody->setCenterOfMassTransform(tr);

    mPhysicObject.SetPosition(getPosition());
    mPhysicObject.rigidBody->setUserPointer(this);

    GLint textureUnit = 0;

    mLightsaberTexture->bind(textureUnit);
    mLightsaberShader->setUniform("uTexture", textureUnit);

    mLightsaberGeometry->bind();
    mLightsaberGeometry->draw();

    mRayShader->use();

    //Ray
    if(turnedOn) {
        modelMatrix = glm::scale(glm::vec3(0.012f, 0.0101f * rayHeight, 0.012f));
        glm::vec3 pos = getPosition();
        translateMatrix = glm::translate(glm::mat4(), pos);
        modelMatrix = translateMatrix * getRotationMatrix4() * modelMatrix;

        mRayShader->setUniform("uModelMatrix", modelMatrix);
        mRayShader->setUniform("uViewMatrix", viewMatrix);
        mRayShader->setUniform("uProjectionMatrix", projectionMatrix);
        mRayShader->setUniform("uNormalMatrix", glm::inverseTranspose(glm::mat3(viewMatrix) * glm::mat3(modelMatrix)));
        mRayShader->setUniform("uColor", glm::vec4(1.0, 1.0, 1.0, 1.0));

        glEnable(GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        mRayGeometry->bind();
        mRayGeometry->draw();

        modelMatrix = glm::scale(glm::vec3(0.022f, 0.0102f * rayHeight, 0.022f));
        pos = getPosition();
        translateMatrix = glm::translate(glm::mat4(), pos);
        modelMatrix = translateMatrix * getRotationMatrix4() * modelMatrix;

        mRayShader->setUniform("uModelMatrix", modelMatrix);
        mRayShader->setUniform("uViewMatrix", viewMatrix);
        mRayShader->setUniform("uProjectionMatrix", projectionMatrix);
        mRayShader->setUniform("uNormalMatrix", glm::inverseTranspose(glm::mat3(viewMatrix) * glm::mat3(modelMatrix)));
        mRayShader->setUniform("uColor", glm::vec4(0, 1.0f, 0, 0.2f));

        mRayGeometry->bind();
        mRayGeometry->draw();
        glDisable(GL_BLEND);
    }
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
 * Turn on/off lightsaber
 */
void Lightsaber::toggle() {
    if (turnedOn) {
        rayHeight = 0;
    } else {
        rayHeight = 1.0f;
    }
    turnedOn = !turnedOn;
}

/*
 * Rotate lightsaber
 */
void Lightsaber::rotate(float _yaw, float _roll, float _pitch) {

    yaw += _yaw;
    pitch += _pitch;
    roll += _roll;
    cout << "=========" << endl;
    cout << yaw << endl;
    cout << roll  << endl;
    cout << pitch << endl;
    cout << "=========" << endl;
    //btTransform tr;
    /*
    btTransform tr = mPhysicObject.rigidBody->getCenterOfMassTransform();
    tr.setIdentity();
    btQuaternion quat;
    quat.setEuler(0, 1.57, 0); //or quat.setEulerZYX depending on the ordering you want
    tr.setRotation(quat);
    mPhysicObject.rigidBody->setCenterOfMassTransform(tr);
    */
    //mPhysicObject.rigidBody->setRotation(tr);

    glm::mat4 R = getRotationMatrix4();
    glm::mat4 newRot = glm::yawPitchRoll(_yaw, _roll, _pitch);
    setRotationMatrix(R * newRot);
}
