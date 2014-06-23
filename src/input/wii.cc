#include "wii.hh"
#include "wiicpp.h"

using namespace std;

//initialize static variables
glm::dvec2 Wii::initialPosition;
glm::dvec2 Wii::movement;
glm::dvec2 Wii::movementScale;
bool Wii::rightMouseButtonDown = false;
bool Wii::leftMouseButtonDown = false;
bool Wii::mouseWheelScrollUp = false;
bool Wii::mouseWheelScrollDown = false;

Wii::Wii(GLFWwindow* window, ACGL::HardwareSupport::SimpleRiftController *simpleRiftController, World *world) :
        Input(window, simpleRiftController, world) {
    // handle the mouse as a callback
    glfwSetCursorPosCallback(window, mouseMoveCallback);

    // handle the mouse wheel as a callback
    glfwSetScrollCallback(window, mouseWheelCallback);
}

// Whenever a mouse button gets pushed and the mouse dragged the cursor should get
// hidden. To prevent the mouse to leave the window, the cursor will get reset to the
// center after each run of this function, to not confuse the user it will get
// moved to the original position when the button gets released.
// Enables FPS View while holding the right mouse button
void Wii::mouseMoveCallback(GLFWwindow *window, double x, double y) {
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
void Wii::mouseWheelCallback(GLFWwindow *window, double xoffset, double yoffset) {
    if (yoffset > 0) {
        mouseWheelScrollUp = true;
    } else {
        mouseWheelScrollDown = true;
    }
}

void Wii::handleInput() {
    Input::handleInput();

    //----------
    //LIGHTSABER
    //----------
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
        Input::world->moveLightsaber(lightsaberMovement);
    }

    //rotate
    float rotateRad = ACGL::Math::Functions::calcDegToRad(5.0f);
    if (mouseWheelScrollUp) {
        if (controlPressed) {
            Input::world->rotateLightsaber(0.0f, -rotateRad, 0.0f);
        } else {
            Input::world->rotateLightsaber(0.0f, 0.0f, -rotateRad);
        }
        //Reset state
        mouseWheelScrollUp = false;
    } else if (mouseWheelScrollDown) {
        if (controlPressed) {
            Input::world->rotateLightsaber(0.0f, rotateRad, 0.0f);
        } else {
            Input::world->rotateLightsaber(0.0f, 0.0f, rotateRad);
        }
        //Reset state
        mouseWheelScrollDown = false;
    }

    //----
    //VIEW
    //----
    if (rightMouseButtonDown) {
        // LookAround functionality
        Input::gSimpleRiftControllerInput->getCamera()->FPSstyleLookAround(movementScale.x, movementScale.y);
    }
}
