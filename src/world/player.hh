#pragma once

#include <ACGL/Scene/MoveableObject.hh>
#include <ACGL/Scene/HMDCamera.hh>
#include "lightsaber.hh"

/*
 * A player can be moved around, the position and orientation are the
 * values of the players feet. The getHMDViewMatrix() gets the matrix for rendering.
 * mHeight is storing the current (depending on ducking state) height.
 * The position/orientation is given in world coordinates.
 *
 * The player has a HMDCamera which has a position and orientation relative to
 * the position/orientation of the Player.
 *
 * Without additional tracking the HMDCamera is positioned at a static translation
 * and can only rotate (look around). Additional tracking can move the head around
 * relative to the bodies center.
 */

class Player: public ACGL::Scene::MoveableObject {
public:
    Player();
    ~Player();

    // connect to the HMD
    void setCamera(ACGL::Scene::SharedHMDCamera sHMDCamera);

    // get the view matrix for rendering (of the HMD controlled eye positions)
    // this is a combination of the player position and the
    glm::mat4 getHMDViewMatrix();

    // projection matrix for rendering, the FoV is controlled by the HMD in case there is one
    glm::mat4 getProjectionMatrix();

    // returns the camera as a shared pointer to let others get/set camera parameters
    ACGL::Scene::SharedHMDCamera getCamera() {
        return mHMDCamera;
    }

    // 1.0: not ducked, 0.0: fully ducked
    void duck(float duckingValue);

    // player uses the force
    void useForce();

    Lightsaber mLightsaber;
private:
    ACGL::Scene::SharedHMDCamera mHMDCamera;
    float mHeight;

    //game related stuff
    int     mPoints ;
    float   mLifeBar;
};
