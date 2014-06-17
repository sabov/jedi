/*
 * Wiimote as input device for lightsaber.
 */

#ifndef WII_H_
#define WII_H_

#include "input.hh"

class Wii: public Input {
public:
    Wii(GLFWwindow* window, ACGL::HardwareSupport::SimpleRiftController *simpleRiftController, World *world);
    ~Wii() {
    }
    ;
    static void mouseMoveCallback(GLFWwindow *window, double x, double y);
    static void mouseWheelCallback(GLFWwindow *window, double xoffset, double yoffset);
    void handleInput();

private:
    static glm::dvec2 initialPosition; // to restore the courser pos later
    static glm::dvec2 movement; // movement of mouse cursor
    static glm::dvec2 movementScale; // movement scaled depending on window size

    //mouse button states
    static bool rightMouseButtonDown;
    static bool leftMouseButtonDown;
    //mouse wheel states
    static bool mouseWheelScrollUp;
    static bool mouseWheelScrollDown;
};
#endif /* WII_H_ */
