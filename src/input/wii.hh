/*
 * Wiimote as input device for lightsaber.
 */

#ifndef WII_H_
#define WII_H_

#include "input.hh"
#include "wiicpp.h"

class Wii: public Input {
public:
    Wii(GLFWwindow* window, ACGL::HardwareSupport::SimpleRiftController *simpleRiftController, World *world);
    ~Wii();
    void handleInput();

private:
    void handleEvent(CWiimote &wm);
    void handleStatus(CWiimote &wm);

    // movement of wiimote
    static glm::vec3 movement;
    static glm::vec3 acceleration;
    // rotation of wiimote
    static float yaw;
    static float roll;
    static float pitch;

    //Wii
    static int LED_MAP[4];
    static bool reloadWiimotes;
    static CWii wii; // Defaults to 4 wiimotes
    static CWiimote* wiimote;

    static double timeLast;
    static double timePassed;

    static const float SMOOTH_FACTOR_GYRO;
    static const float SMOOTH_FACTOR_ACCEL;
};
#endif /* WII_H_ */
