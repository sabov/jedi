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
    ~Wii() {};
    static void mouseMoveCallback(GLFWwindow *window, double x, double y);
    static void mouseWheelCallback(GLFWwindow *window, double xoffset, double yoffset);
    void handleInput();
    void main();
private:
    void handleEvent(CWiimote &wm);
    void handleStatus(CWiimote &wm);
    void handleDisconnect(CWiimote &wm);
    void handleReadData(CWiimote &wm);
    void handleNunchukInserted(CWiimote &wm);
    void handleClassicInserted(CWiimote &wm);
    void handleGH3Inserted(CWiimote &wm);

    static glm::dvec2 initialPosition; // to restore the courser pos later
    static glm::dvec2 movement; // movement of mouse cursor
    static glm::dvec2 movementScale; // movement scaled depending on window size

    //mouse button states
    static bool rightMouseButtonDown;
    static bool leftMouseButtonDown;
    //mouse wheel states
    static bool mouseWheelScrollUp;
    static bool mouseWheelScrollDown;

    static int LED_MAP[4];
};
#endif /* WII_H_ */
