#include "player.hh"

using namespace std;
using namespace ACGL::Scene;

Player::Player() :
        mLightsaber(getPosition()) {
    mPhysicObject.Init(cShape, getPosition());
    mPhysicObject.rigidBody->setUserPointer(this);
}

Player::~Player() {
    delete cShape;
}

void Player::setCamera(ACGL::Scene::SharedHMDCamera sHMDCamera) {
    mHMDCamera = sHMDCamera;
    setPosition(glm::vec3(0.0f, 0.0f, 0.0f)); // feet position
    duck(1.0f); // standing up
}

glm::mat4 Player::getHMDViewMatrix() {
    glm::mat4 playerInTheWorld = this->getModelMatrix();
    glm::mat4 headOnBody = mHMDCamera->getViewMatrix();

    float heightOfEyes = mHeight - 0.12f; // estimated distance eyes to head
    headOnBody = glm::translate(headOnBody, glm::vec3(0.0f, -heightOfEyes, 0.0f));

    return headOnBody * playerInTheWorld;
}

glm::mat4 Player::getProjectionMatrix() {
    return mHMDCamera->getProjectionMatrix();
}

void Player::duck(float duckingValue) {
    duckingValue = min(duckingValue, 1.0f);
    duckingValue = max(duckingValue, 0.0f);

    // values are rough estimates, need to be measured correctly
    // for the Virtualizer
    //        |----- legs ------------| + upper body
    mHeight = 0.3f + duckingValue * 0.70f + 0.87f;
}

void Player::useForce() {
    //TODO do something useful
}

bool Player::VHandleEvent(const GameLogic::IEventData &_event)
{
    return true;
}
