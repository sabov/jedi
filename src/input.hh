/*
 * General input for game.
 * Singleton.
 */

#ifndef INPUT_H_
#define INPUT_H_

#include <ACGL/OpenGL/GL.hh> // this has to be included before glfw.h !
#include <GLFW/glfw3.h>
#include <ACGL/Math/Math.hh>
#include <ACGL/Utils/Log.hh>
#include <ACGL/HardwareSupport/GamePad.hh>
#include <ACGL/HardwareSupport/SimpleRiftController.hh>
#include "world/world.hh"

using namespace ACGL::Utils;
using namespace ACGL::HardwareSupport;

class Input {
public:
    Input(GLFWwindow* window, ACGL::HardwareSupport::SimpleRiftController *simpleRiftController);
    ~Input() {
    }
    ;
    static void mouseMoveCallback(GLFWwindow *window, double x, double y);
    static void mouseWheelCallback(GLFWwindow *window, double xoffset, double yoffset);
    static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int modifier);
    void handleGamePad();
    void handleInput();

private:
    // Rift controller
    static ACGL::HardwareSupport::SimpleRiftController *gSimpleRiftControllerInput;

    static glm::dvec2 initialPosition; // to restore the courser pos later
    static glm::dvec2 windowSize;
    static glm::dvec2 movement; // movement of mouse cursor
    static glm::dvec2 movementScale; // movement scaled depending on window size

    //mouse button states
    static bool rightMouseButtonDown;
    static bool leftMouseButtonDown;
    //mouse wheel states
    static bool mouseWheelScrollUp;
    static bool mouseWheelScrollDown;

    // Some bools to store the desired movements.
    // The player can move the character by using WASD or a gamepad.
    static bool forwardPressed;
    static bool backwardPressed;
    static bool leftPressed;
    static bool rightPressed;
    static bool rotateLeftPressed;
    static bool rotateRightPressed;
    static bool forcePressed;
    static bool controlPressed;

    // Some floats to store the analog counterparts for gamepad and
    // later the Virtualizer inputs:
    static float analogLeftRightMovement;
    static float analogForwardBackMovement;
    static float analogLeftRightRotation;
    static float analogDucking; // 0 = ducked, 1 = standing

};
#endif /* INPUT_H_ */
