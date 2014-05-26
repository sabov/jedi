#include "input.hh"

using namespace std;

// see main.cc:
extern World *gWorld;

//initialize static variables
bool Input::sForwardPressed = false;
bool Input::sBackwardPressed = false;
bool Input::sLeftPressed = false;
bool Input::sRightPressed = false;
bool Input::sRotateLeftPressed = false;
bool Input::sRotateRightPressed = false;
bool Input::sForcePressed = false;

float Input::gAnalogLeftRightMovement = 0.0f;
float Input::gAnalogForwardBackMovement = 0.0f;
float Input::gAnalogLeftRightRotation = 0.0f;
float Input::gAnalogDucking = 1.0f;

ACGL::HardwareSupport::SimpleRiftController *Input::gSimpleRiftControllerInput = NULL;

Input::Input(GLFWwindow* window, ACGL::HardwareSupport::SimpleRiftController *simpleRiftController) {
    // 0 = ducked, 1 = standing

    // don't miss short keypresses, which are shorter than one frame and would be missed otherwise:
    // useful if the keys are read directly and not as a callback.
    glfwSetInputMode(window, GLFW_STICKY_KEYS, true);

    // handle the mouse as a callback
    glfwSetCursorPosCallback(window, mouseMoveCallback);

    // handle the keyboard inputs as a callback as well
    glfwSetKeyCallback(window, keyboardCallback);

    // store it to update the attached camera based on the Rifts input later each frame:
    Input::gSimpleRiftControllerInput = simpleRiftController;
}

// Whenever a mouse button gets pushed and the mouse dragged the cursor should get
// hidden. To prevent the mouse to leave the window, the cursor will get reset to the
// center after each run of this function, to not confuse the user it will get
// moved to the original position when the button gets released.
// Enables FPS View while holding the right mouse button
void Input::mouseMoveCallback(GLFWwindow *window, double x, double y) {
    static glm::dvec2 initialPosition; // to restore the courser pos later
    static glm::dvec2 windowSize;
    static bool rightMouseButtonDown = false; // right button state
    static bool leftMouseButtonDown = false; // left button state

    glm::dvec2 movement; // movement of mouse cursor
    glm::dvec2 movementScale; // movement scaled depending on window size

    // Get the window size (do this each mouse press in case the window was resized,
    // this could be stored and only get refreshed in case a resize happened but this
    // would need more global state -> let's keep this example simple).
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    windowSize = glm::dvec2(width, height);

    // Right button
    if (!rightMouseButtonDown && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
        rightMouseButtonDown = true;

        initialPosition = glm::dvec2(x, y);

        // no movement in this call:
        x = width * 0.5;
        y = height * 0.5;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // hide the cursor
    } else if (rightMouseButtonDown && !glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
        rightMouseButtonDown = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // unhide the cursor
        glfwSetCursorPos(window, initialPosition.x, initialPosition.y);
    }

    // Left button
    if (!leftMouseButtonDown && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
        leftMouseButtonDown = true;

        initialPosition = glm::dvec2(x, y);

        // no movement in this call:
        x = width * 0.5;
        y = height * 0.5;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // hide the cursor
    } else if (leftMouseButtonDown && !glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
        leftMouseButtonDown = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // unhide the cursor
        glfwSetCursorPos(window, initialPosition.x, initialPosition.y);
    }

    // Get the movement in pixels.
    // Y axis up should be positive. A scaling of the mouse speed can be done here as well!
    movement = glm::dvec2(x, y) - 0.5 * windowSize;
    movement *= glm::dvec2(1.0, -1.0);
    movementScale.x = movement.x / windowSize.x;
    movementScale.y = -movement.y / windowSize.y;

    if (rightMouseButtonDown) {
        // LookAround functionality
        Input::gSimpleRiftControllerInput->getCamera()->FPSstyleLookAround(movementScale.x, movementScale.y);
        glfwSetCursorPos(window, windowSize.x * 0.5, windowSize.y * 0.5);
    } else if (leftMouseButtonDown) {
        // move lightsaber
        glm::vec3 lightsaberMovement;
        lightsaberMovement.x = movementScale.x;
        lightsaberMovement.y = -movementScale.y;
        gWorld->moveLightsaber(lightsaberMovement);
        glfwSetCursorPos(window, windowSize.x * 0.5, windowSize.y * 0.5);
    } else {
        // TODO
    }
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
        Input::sForwardPressed = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_A)
        Input::sLeftPressed = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_S)
        Input::sBackwardPressed = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_D)
        Input::sRightPressed = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_Q)
        Input::sRotateLeftPressed = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_E)
        Input::sRotateRightPressed = (action != GLFW_RELEASE);

    if (key == GLFW_KEY_SPACE)
        Input::sForcePressed = (action != GLFW_RELEASE);

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
    gAnalogLeftRightMovement = gamePad.getAxis(GamePad::LEFT_ANALOG_STICK_X);
    gAnalogForwardBackMovement = gamePad.getAxis(GamePad::RIGHT_ANALOG_STICK_Y);
    gAnalogLeftRightRotation = gamePad.getAxis(GamePad::RIGHT_ANALOG_STICK_X);
    gAnalogDucking = 1.0f - gamePad.getAxis(GamePad::LEFT_ANALOG_TRIGGER);

    // nice for debugging, pressed keys are printed in UPPERCASE:
    //gamePad.printPressedButtons();
}

void Input::handleInput() {
    // Get GLFW to grab input events from the OS
    // this will trigger the callbacks.
    glfwPollEvents();
    handleGamePad();

    //
    // move the player:
    glm::vec3 playersBodyMovement;
    // TODO: make this framerate independent!
    // The movements should be based on the actual time that has passed since the last
    // query of the inputs!
    float movementSpeed = 0.1f;
    if (sForwardPressed)
        playersBodyMovement.z -= movementSpeed;
    if (sLeftPressed)
        playersBodyMovement.x -= movementSpeed;
    if (sBackwardPressed)
        playersBodyMovement.z += movementSpeed;
    if (sRightPressed)
        playersBodyMovement.x += movementSpeed;

    // add analog gamepad movements:
    playersBodyMovement.z -= gAnalogForwardBackMovement;
    playersBodyMovement.x += gAnalogLeftRightMovement;

    gWorld->movePlayer(playersBodyMovement);

    // rotate players body:
    float playersBodyRotation = 0.0f;
    if (sRotateLeftPressed)
        playersBodyRotation -= 5.0f;
    if (sRotateRightPressed)
        playersBodyRotation += 5.0f;

    // add analog gamepad rotation:
    playersBodyRotation += gAnalogLeftRightRotation;
    gWorld->rotatePlayer(ACGL::Math::Functions::calcDegToRad(playersBodyRotation));

    // apply ducking. TODO: add a gamepad less option (keyboard)
    gWorld->duckPlayer(gAnalogDucking);

    if (sForcePressed) {
        gWorld->useForcePlayer();
    }

    // Grabs the input from the Rift to update the cameras orientation:
    // Note: If no Rift is connected, this will do nothing.
    gSimpleRiftControllerInput->updateCamera();
}
