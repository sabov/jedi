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

    static glm::dvec2 movement; // movement of mouse cursor
    static glm::dvec2 movementScale; // movement scaled depending on window size

    //Wii
    static int LED_MAP[4];
    static bool reloadWiimotes;
    static CWii wii; // Defaults to 4 remotes
    static CWiimote* wiimote;
};
#endif /* WII_H_ */
