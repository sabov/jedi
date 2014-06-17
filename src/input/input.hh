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
#include "../world/world.hh"

using namespace ACGL::Utils;
using namespace ACGL::HardwareSupport;

class Input {
    friend class Mouse;
    friend class Wii;
public:
    Input(GLFWwindow* window, ACGL::HardwareSupport::SimpleRiftController *simpleRiftController, World *world);
    ~Input() {
    }
    ;
    static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int modifier);
    void handleGamePad();
    void handleInput();

private:
    static World *world;

    // Rift controller
    static ACGL::HardwareSupport::SimpleRiftController *gSimpleRiftControllerInput;

    static glm::dvec2 windowSize;

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
