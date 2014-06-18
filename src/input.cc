#include "input.hh"

using namespace std;

// see main.cc:
extern World *gWorld;

//initialize static variables
glm::dvec2 Input::initialPosition;
glm::dvec2 Input::windowSize;
glm::dvec2 Input::movement;
glm::dvec2 Input::movementScale;
bool Input::rightMouseButtonDown = false;
bool Input::leftMouseButtonDown = false;
bool Input::mouseWheelScrollUp = false;
bool Input::mouseWheelScrollDown = false;

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

Input::Input(GLFWwindow* window, ACGL::HardwareSupport::SimpleRiftController *simpleRiftController) {
    // don't miss short keypresses, which are shorter than one frame and would be missed otherwise:
    // useful if the keys are read directly and not as a callback.
    glfwSetInputMode(window, GLFW_STICKY_KEYS, true);

    // handle the mouse as a callback
    glfwSetCursorPosCallback(window, mouseMoveCallback);

    // handle the mouse wheel as a callback
    glfwSetScrollCallback(window, mouseWheelCallback);

    // handle the keyboard inputs as a callback
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
    // Get the window size (do this each mouse press in case the window was resized,
    // this could be stored and only get refreshed in case a resize happened but this
    // would need more global state -> let's keep this example simple).
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    windowSize = glm::dvec2(width, height);

    // Get the movement in pixels.
    // Y axis up should be positive. A scaling of the mouse speed can be done here as well!
    movement = glm::dvec2(x, y) - 0.5 * windowSize;
    movement *= glm::dvec2(1.0, -1.0);
    movementScale.x = movement.x / windowSize.x;
    movementScale.y = -movement.y / windowSize.y;

    bool leftMouseButtonPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
    bool rightMouseButtonPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2);

    if ((!rightMouseButtonDown && rightMouseButtonPressed) || (!leftMouseButtonDown && leftMouseButtonPressed)) {
        //New button pressed
        initialPosition = glm::dvec2(x, y);

        // no movement in this call:
        x = width * 0.5;
        y = height * 0.5;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // hide the cursor

        if (!rightMouseButtonDown && rightMouseButtonPressed) {
            rightMouseButtonDown = true;
        }
        if (!leftMouseButtonDown && leftMouseButtonPressed) {
            leftMouseButtonDown = true;
        }

    } else if ((rightMouseButtonDown && !rightMouseButtonPressed) || (leftMouseButtonDown && !leftMouseButtonPressed)) {
        //New button not pressed
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // unhide the cursor
        glfwSetCursorPos(window, initialPosition.x, initialPosition.y);

        if (rightMouseButtonDown && !rightMouseButtonPressed) {
            rightMouseButtonDown = false;
        }
        if (leftMouseButtonDown && !leftMouseButtonPressed) {
            leftMouseButtonDown = false;
        }
    }

    if (rightMouseButtonDown || leftMouseButtonDown) {
        //Reset current cursor position
        glfwSetCursorPos(window, windowSize.x * 0.5, windowSize.y * 0.5);
    }
}

//Handles the mouse wheel
void Input::mouseWheelCallback(GLFWwindow *window, double xoffset, double yoffset) {
    if (yoffset > 0) {
        mouseWheelScrollUp = true;
    } else {
        mouseWheelScrollDown = true;
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

    //----------
    //LIGHTSABER
    //----------
    //Turn off/on
    if (forcePressed) {
        if(previousForceState != forcePressed) {
            gWorld->toggleLightsaber();
        }
    }
    previousForceState = forcePressed;


    //Move in space
    if (leftMouseButtonDown) {
        // Move lightsaber
        glm::vec3 lightsaberMovement;
        lightsaberMovement.x = movementScale.x;
        if (controlPressed) {
            //Move on z-axis instead of y-axis
            lightsaberMovement.z = movementScale.y;
        } else {
            lightsaberMovement.y = -movementScale.y;
        }
        gWorld->moveLightsaber(lightsaberMovement);
    }

    //rotate
    float rotateRad = ACGL::Math::Functions::calcDegToRad(5.0f);
    if (mouseWheelScrollUp) {
        if (controlPressed) {
            gWorld->rotateLightsaber(0.0f, -rotateRad, 0.0f);
        } else {
            gWorld->rotateLightsaber(0.0f, 0.0f, -rotateRad);
        }
        //Reset state
        mouseWheelScrollUp = false;
    } else if (mouseWheelScrollDown) {
        if (controlPressed) {
            gWorld->rotateLightsaber(0.0f, rotateRad, 0.0f);
        } else {
            gWorld->rotateLightsaber(0.0f, 0.0f, rotateRad);
        }
        //Reset state
        mouseWheelScrollDown = false;
    }

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

    gWorld->movePlayer(playersBodyMovement);

    // rotate players body:
    float playersBodyRotation = 0.0f;
    if (rotateLeftPressed)
        playersBodyRotation -= 5.0f;
    if (rotateRightPressed)
        playersBodyRotation += 5.0f;

    // add analog gamepad rotation:
    playersBodyRotation += analogLeftRightRotation;
    gWorld->rotatePlayer(ACGL::Math::Functions::calcDegToRad(playersBodyRotation));

    // apply ducking. TODO: add a gamepad less option (keyboard)
    gWorld->duckPlayer(analogDucking);

    //Use force
    if (forcePressed) {
        //gWorld->useForcePlayer();
    }

    //----
    //VIEW
    //----
    if (rightMouseButtonDown) {
        // LookAround functionality
        Input::gSimpleRiftControllerInput->getCamera()->FPSstyleLookAround(movementScale.x, movementScale.y);
    }

    // Grabs the input from the Rift to update the cameras orientation:
    // Note: If no Rift is connected, this will do nothing.
    gSimpleRiftControllerInput->updateCamera();
}
