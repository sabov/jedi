#include "input.hh"

using namespace std;

//initialize static variables
glm::dvec2 Input::windowSize;

bool Input::forwardPressed = false;
bool Input::backwardPressed = false;
bool Input::leftPressed = false;
bool Input::rightPressed = false;
bool Input::rotateLeftPressed = false;
bool Input::rotateRightPressed = false;
bool Input::forcePressed = false;
bool Input::controlPressed = false;

bool Input::previousForceState = false;

float Input::analogLeftRightMovement = 0.0f;
float Input::analogForwardBackMovement = 0.0f;
float Input::analogLeftRightRotation = 0.0f;
float Input::analogDucking = 1.0f;

ACGL::HardwareSupport::SimpleRiftController *Input::gSimpleRiftControllerInput = NULL;
World *Input::world = NULL;

Input::Input(GLFWwindow* window, ACGL::HardwareSupport::SimpleRiftController *simpleRiftController, World *world) {
    Input::world = world;

    // don't miss short keypresses, which are shorter than one frame and would be missed otherwise:
    // useful if the keys are read directly and not as a callback.
    glfwSetInputMode(window, GLFW_STICKY_KEYS, true);

    // handle the keyboard inputs as a callback
    glfwSetKeyCallback(window, keyboardCallback);

    // store it to update the attached camera based on the Rifts input later each frame:
    Input::gSimpleRiftControllerInput = simpleRiftController;
}

// The keyboard callback: Gets called for each keypress.
// The scan code is system dependent, use the key to get the button pressed.
// The repeat action does not get called each frame if the button is pressed down, so
// in most cases it's better to store pushed down keys on our own.
void Input::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int modifier) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        // signal that the window should get closed if ESC is pressed
        // glfwWindowShouldClose will return true if glfwSetWindowShouldClose is set!
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_W)
        Input::forwardPressed = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_A)
        Input::leftPressed = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_S)
        Input::backwardPressed = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_D)
        Input::rightPressed = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_Q)
        Input::rotateLeftPressed = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_E)
        Input::rotateRightPressed = (action != GLFW_RELEASE);

    if (key == GLFW_KEY_SPACE)
        Input::forcePressed = (action != GLFW_RELEASE);

    if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL)
        Input::controlPressed = (action != GLFW_RELEASE);

    // prints the events for debugging and finding out keycodes:
    /*
     cout << key << " ";
     if (action == GLFW_PRESS) cout << "pressed";
     if (action == GLFW_REPEAT) cout << "repeat";
     if (action == GLFW_RELEASE) cout << "released";

     if (modifier & GLFW_MOD_ALT) cout << " +ALT ";
     if (modifier & GLFW_MOD_CONTROL) cout << " +CTRL ";
     if (modifier & GLFW_MOD_SHIFT) cout << " +SHIFT ";
     if (modifier & GLFW_MOD_SUPER) cout << " +SUPER ";
     cout << endl;
     */
}

void Input::handleGamePad() {
    // probably not the best place to store our gamepad, but
    // it makes the example simpler:
    static GamePad gamePad;

    // in case no gamepad was found, do nothing:
    if (!gamePad.ok())
        return;

    // update the gamepad state (catch new inputs):
    // (this is needed in addition to glfwPollEvents)
    gamePad.update();

    // Example of how to query the gamepad: For known gamepads the axes have names
    // based on there position on the gamepad, so all known gamepads behave the same.
    // In case you want to support more gamepads, see ACGL::HardwareSupport::GamePad on
    // how to extend this or read the raw values from the gamepad based on numbered
    // axes.
    analogLeftRightMovement = gamePad.getAxis(GamePad::LEFT_ANALOG_STICK_X);
    analogForwardBackMovement = gamePad.getAxis(GamePad::RIGHT_ANALOG_STICK_Y);
    analogLeftRightRotation = gamePad.getAxis(GamePad::RIGHT_ANALOG_STICK_X);
    analogDucking = 1.0f - gamePad.getAxis(GamePad::LEFT_ANALOG_TRIGGER);

    // nice for debugging, pressed keys are printed in UPPERCASE:
    //gamePad.printPressedButtons();
}

void Input::handleInput() {
    // Get GLFW to grab input events from the OS
    // this will trigger the callbacks.
    glfwPollEvents();
    handleGamePad();

    //------
    //PLAYER
    //------
    glm::vec3 playersBodyMovement;
    // TODO: make this framerate independent!
    // The movements should be based on the actual time that has passed since the last
    // query of the inputs!
    float movementSpeed = 0.1f;
    if (forwardPressed)
        playersBodyMovement.z -= movementSpeed;
    if (leftPressed)
        playersBodyMovement.x -= movementSpeed;
    if (backwardPressed)
        playersBodyMovement.z += movementSpeed;
    if (rightPressed)
        playersBodyMovement.x += movementSpeed;

    // add analog gamepad movements:
    playersBodyMovement.z -= analogForwardBackMovement;
    playersBodyMovement.x += analogLeftRightMovement;

    world->movePlayer(playersBodyMovement);

    // rotate players body:
    float playersBodyRotation = 0.0f;
    if (rotateLeftPressed)
        playersBodyRotation -= 5.0f;
    if (rotateRightPressed)
        playersBodyRotation += 5.0f;

    // add analog gamepad rotation:
    playersBodyRotation += analogLeftRightRotation;
    world->rotatePlayer(ACGL::Math::Functions::calcDegToRad(playersBodyRotation));

    // apply ducking. TODO: add a gamepad less option (keyboard)
    world->duckPlayer(analogDucking);

    //Use force
    if (forcePressed) {
        if (previousForceState != forcePressed) {
            world->toggleLightsaber();
        }
        world->useForcePlayer();
    }
    previousForceState = forcePressed;

    // Grabs the input from the Rift to update the cameras orientation:
    // Note: If no Rift is connected, this will do nothing.
    gSimpleRiftControllerInput->updateCamera();
}
