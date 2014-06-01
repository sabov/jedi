#include <ACGL/OpenGL/GL.hh> // this has to be included before glfw.h !
#include <GLFW/glfw3.h>
#include <ACGL/Math/Math.hh>
#include <ACGL/Utils/Log.hh>
#include <ACGL/HardwareSupport/GamePad.hh>
#include <ACGL/HardwareSupport/SimpleRiftController.hh>
#include "world/world.hh"

using namespace ACGL::Utils;
using namespace ACGL::HardwareSupport;
using namespace std;

//
// Global variables for the rendering:
// You can move the whole rendering code into a class and make this variables
// members. This was not done here to keep the example code a bit simpler.
//
ACGL::HardwareSupport::SimpleRiftController *gSimpleRiftControllerInput;

// see main.cc:
extern World *gWorld;

//
// Some bools to store the desired movements.
// The player can move the character by using WASD or a gamepad.
bool sForwardPressed  = false;
bool sBackwardPressed = false;
bool sLeftPressed     = false;
bool sRightPressed    = false;
bool sRotateLeftPressed  = false;
bool sRotateRightPressed = false;

bool sForcePressed = false;

//
// Some floats to store the analog counterparts for gamepad and
// later the Virtualizer inputs:
float gAnalogLeftRightMovement   = 0.0f;
float gAnalogForwardBackMovement = 0.0f;
float gAnalogLeftRightRotation   = 0.0f;
float gAnalogDucking             = 1.0f; // 0 = ducked, 1 = standing

void mouseMoveCallback( GLFWwindow *window, double x, double y );
void keyboardCallback( GLFWwindow* window, int key, int scancode, int action, int modifier );

void initInput( GLFWwindow* window, ACGL::HardwareSupport::SimpleRiftController *simpleRiftController )
{
    // don't miss short keypresses, which are shorter than one frame and would be missed otherwise:
    // usefull if the keys are read directly and not as a callback.
    glfwSetInputMode( window, GLFW_STICKY_KEYS, true );

    //
    // handle the mouse as a callback:
    glfwSetCursorPosCallback( window, mouseMoveCallback );

    //
    // handle the keyboard inputs as a callback as well:
    glfwSetKeyCallback( window, keyboardCallback );

    // store it to update the attached camera based on the Rifts input later each frame:
    gSimpleRiftControllerInput = simpleRiftController;
}



// Whenever a mouse button gets pushed and the mouse dragged the curser should get
// hidden. To prevent the mouse to leave the window, the curser will get reset to the
// center after each run of this function, to not confuse the user it will get
// moved to the original position when the button gets released.
// Enables FPS View while holding the right mouse button
void mouseMoveCallback( GLFWwindow *window, double x, double y )
{
    static glm::dvec2 initialPosition; // to restore the courser pos later
    static glm::dvec2 windowSize;
    static bool rightMouseButtonDown = false; // right button state
    static bool leftMouseButtonDown = false;  // left button state


    // track button state and react (once) on changes:
    if (!rightMouseButtonDown && glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_2 )) {
        rightMouseButtonDown = true;

        // Get the window size (do this each mouse press in case the window was resized,
        // this could be stored and only get refreshed in case a resize happened but this
        // would need more global state -> let's keep this example simple).
        int width, height;
        glfwGetWindowSize( window, &width, &height );
        windowSize = glm::dvec2( width, height );

        initialPosition = glm::dvec2( x,y );

        // no movement in this call:
        x = width  * 0.5;
        y = height * 0.5;
        glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN ); // hide the cursor
    } else if (rightMouseButtonDown && !glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_2 )) {
        rightMouseButtonDown = false;
        glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL ); // unhide the cursor
        glfwSetCursorPos( window, initialPosition.x, initialPosition.y );
    }

    if (!leftMouseButtonDown && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
        leftMouseButtonDown = true;
    } else if (leftMouseButtonDown && !glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
        leftMouseButtonDown = false;
    }

    // Get the movement in pixels.
    // Y axis up should be positive. A scaling of the mouse speed can be done here as well!
    glm::dvec2 movement = glm::dvec2( x,y ) - 0.5*windowSize;
    movement *= glm::dvec2( 1.0, -1.0 );


    if (rightMouseButtonDown) {
        debug() << glm::to_string( movement ) << endl;

        // The input could now be used with a cameras
        // FPSstyleLookAround( relativeMovement.x, relativeMovement.y ) method
        // to get look around functionality by using the mouse, or any other
        // functionality!
        gSimpleRiftControllerInput->getCamera()->FPSstyleLookAround(movement.x/windowSize.x, -movement.y/windowSize.y);
        glfwSetCursorPos( window, windowSize.x * 0.5, windowSize.y * 0.5 );
    }

    if (leftMouseButtonDown) {
        sForcePressed = true;
    } else {
        sForcePressed = false;
    }
}

// The keyboard callback: Gets called for each keypress.
// The scan code is system dependent, use the key to get the button pressed.
// The repeat action does not get called each frame if the button is pressed down, so
// in most cases it's better to store pushed down keys on our own.
void keyboardCallback( GLFWwindow* window, int key, int scancode, int action, int modifier )
{
    if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS ) {
        // signal that the window should get closed if ESC is pressed
        // glfwWindowShouldClose will return true if glfwSetWindowShouldClose is set!
        glfwSetWindowShouldClose( window, true );
    }

    if ( key == GLFW_KEY_W ) sForwardPressed  = (action != GLFW_RELEASE);
    if ( key == GLFW_KEY_A ) sLeftPressed     = (action != GLFW_RELEASE);
    if ( key == GLFW_KEY_S ) sBackwardPressed = (action != GLFW_RELEASE);
    if ( key == GLFW_KEY_D ) sRightPressed    = (action != GLFW_RELEASE);
    if ( key == GLFW_KEY_Q ) sRotateLeftPressed  = (action != GLFW_RELEASE);
    if ( key == GLFW_KEY_E ) sRotateRightPressed = (action != GLFW_RELEASE);

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

void handleGamePad()
{
    // probably not the best place to store our gamepad, but
    // it makes the example simpler:
    static GamePad gamePad;

    // in case no gamepad was found, do nothing:
    if (!gamePad.ok()) return;

    // update the gamepad state (catch new inputs):
    // (this is needed in addition to glfwPollEvents)
    gamePad.update();

    // Example of how to query the gamepad: For known gamepads the axes have names
    // based on there position on the gamepad, so all known gamepads behave the same.
    // In case you want to support more gamepads, see ACGL::HardwareSupport::GamePad on
    // how to extend this or read the raw values from the gamepad based on numbered
    // axes.
    gAnalogLeftRightMovement   = gamePad.getAxis( GamePad::LEFT_ANALOG_STICK_X );
    gAnalogForwardBackMovement = gamePad.getAxis( GamePad::RIGHT_ANALOG_STICK_Y );
    gAnalogLeftRightRotation   = gamePad.getAxis( GamePad::RIGHT_ANALOG_STICK_X  );
    gAnalogDucking             = 1.0f - gamePad.getAxis( GamePad::LEFT_ANALOG_TRIGGER  );

    // nice for debugging, pressed keys are printed in UPPERCASE:
    //gamePad.printPressedButtons();
}

void handleInput()
{
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
    if (sForwardPressed)  playersBodyMovement.z -= movementSpeed;
    if (sLeftPressed)     playersBodyMovement.x -= movementSpeed;
    if (sBackwardPressed) playersBodyMovement.z += movementSpeed;
    if (sRightPressed)    playersBodyMovement.x += movementSpeed;

    // add analog gamepad movements:
    playersBodyMovement.z -= gAnalogForwardBackMovement;
    playersBodyMovement.x += gAnalogLeftRightMovement;

    gWorld->movePlayer( playersBodyMovement );

    // rotate players body:
    float playersBodyRotation = 0.0f;
    if (sRotateLeftPressed)  playersBodyRotation -= 5.0f;
    if (sRotateRightPressed) playersBodyRotation += 5.0f;

    // add analog gamepad rotation:
    playersBodyRotation += gAnalogLeftRightRotation;
    gWorld->rotatePlayer( ACGL::Math::Functions::calcDegToRad( playersBodyRotation) );

    // apply ducking. TODO: add a gamepad less option (keyboard)
    gWorld->duckPlayer( gAnalogDucking );

    if (sForcePressed){
        gWorld->useForcePlayer();
    }

    // Grabs the input from the Rift to update the cameras orientation:
    // Note: If no Rift is connected, this will do nothing.
    gSimpleRiftControllerInput->updateCamera();
}
