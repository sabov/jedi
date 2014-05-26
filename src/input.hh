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
    static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int modifier);
    void handleGamePad();
    void handleInput();

private:
    // Rift controller
    static ACGL::HardwareSupport::SimpleRiftController *gSimpleRiftControllerInput;

    // Some bools to store the desired movements.
    // The player can move the character by using WASD or a gamepad.
    static bool sForwardPressed;
    static bool sBackwardPressed;
    static bool sLeftPressed;
    static bool sRightPressed;
    static bool sRotateLeftPressed;
    static bool sRotateRightPressed;
    static bool sForcePressed;

    // Some floats to store the analog counterparts for gamepad and
    // later the Virtualizer inputs:
    static float gAnalogLeftRightMovement;
    static float gAnalogForwardBackMovement;
    static float gAnalogLeftRightRotation;
    static float gAnalogDucking; // 0 = ducked, 1 = standing

};
#endif /* INPUT_H_ */
