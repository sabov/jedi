/*
 * Simple game template for the programming courses 2014.
 *
 * Change whatever you need.
 *
 * This file will handle the window creation with GLFW.
 * The rendering is in the renderer.cc.
 * The audio is in audio.cc.
 * The input handling (mouse, keyboard, gamepad) is in input.cc.
 *
 */

#ifndef __GLEW_H__
#include <GL/glew.h>
#endif

// OpenGL defines and function pointers. We use this instead of the system GL.h or glew.h.
// This has to be included before glfw.h !
#include <ACGL/OpenGL/GL.hh>

// used to open a window and get user input:
#include <GLFW/glfw3.h>

#include <ACGL/OpenGL/Managers.hh>
#include <ACGL/Math/Math.hh>
#include <ACGL/HardwareSupport/SimpleRiftController.hh>
#include <ACGL/Base/Settings.hh>

#include "audio/audio.hh"
#include "world/world.hh"
#include "input/mouse.hh"
#include "input/wii.hh"

//Set input device

typedef Wii INPUT;

//
// Store the world in a global object. This is not pretty nor is it good software design,
// but it keeps the example simple.
//
World *gWorld = NULL;

// Handles input
INPUT *gInput = NULL;

extern void initRenderer(GLFWwindow *window, ACGL::HardwareSupport::SimpleRiftController *simpleRiftController); // see renderer.cc
extern void renderFrame();
extern void shutdownRenderer();

//
// Returns a valid pointer to a window if a window with the desired context could get created.
// NULL otherwise.
// Requested OpenGL version gets set by ACGL defines.
//
// if monitorNumber is < 0 or > number of displays the primary display is used
GLFWwindow* createWindow(bool fullscreen, int monitorNumber) {
    // we assume a core profile here to stay Mac compatible:
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef ACGL_OPENGL_PROFILE_CORE
#error "Compatibility profiles will break Mac support, use Core"
#endif

#ifdef __APPLE__
#if (ACGL_OPENGL_VERSION >= 30)
    // request OpenGL 3.2, this will return a 4.1 context on Mavericks,
    // this is a known bug (you can't request 4.1 currently). If this gets fixed,
    // request the version you need on a Mac
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
#endif
#else
    // non-apple: request the actual version:
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, ACGL_OPENGL_VERSION / 10);
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, ACGL_OPENGL_VERSION % 10);
#endif

    //
    // Try to get a native debug context, this will slow down GL a bit but be helpful for finding
    // errors.
    glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // Define whether the window can get resized:
    glfwWindowHint( GLFW_RESIZABLE, true);

    // The framebuffer should support sRGB if possible - note: this does not activate the support itself!
    // sRGB framebuffers are needed for gamma correct rendering.
    glfwWindowHint( GLFW_SRGB_CAPABLE, true);

    //
    // try to create an OpenGL context in a window and check the supported OpenGL version:
    //
    GLFWwindow* window = NULL;
    if (!fullscreen) {
        // windowed:
        window = glfwCreateWindow(1280, 800, "SimpleVRGame", NULL, NULL);
    } else {
        // fullscreen:
        int numberOfMonitors = 1;
        GLFWmonitor **monitors = glfwGetMonitors(&numberOfMonitors);
        GLFWmonitor *monitor = NULL;

        if (monitorNumber >= numberOfMonitors || monitorNumber < 0) {
            monitor = glfwGetPrimaryMonitor();
        } else {
            monitor = monitors[monitorNumber];
        }

        const GLFWvidmode *currentVideoMode = glfwGetVideoMode(monitor);
        window = glfwCreateWindow(currentVideoMode->width, currentVideoMode->height, "SimpleVRGame", monitor, NULL);
    }
    if (!window) {
        ACGL::Utils::error() << "Failed to open a GLFW window - requested OpenGL version: " << ACGL_OPENGL_VERSION << std::endl;
        return NULL;
    }

    //
    // "activate" this context. Only called once as this application only has one context / window.
    //
    glfwMakeContextCurrent(window);

    return window;
}

// the callback GLFW calls if there were any problems (uncommon) :
void myGLFWErrorCallback(int errorCode, const char *description) {
    ACGL::Utils::error() << "GLFW error " << errorCode << ": " << description << std::endl;
}

int main(int argc, char *argv[]) {
    //
    // Initialise GLFW
    //
    glfwSetErrorCallback(myGLFWErrorCallback); // yes, this can be called before the init!
    if (!glfwInit()) {
        ACGL::Utils::error() << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }
    //
    // Create an OpenGL capable window:
    // parameter is whether the window should be fullscreen or not and which monitor to use if fullscreen
    //
    GLFWwindow* myWindow = createWindow(false, 1);
    if (!myWindow) {
        glfwTerminate();
        exit(-1);
    }

    //
    // Now that a window is open (with OpenGL), we can init ACGL:
    // The true parameter tells ACGL to simulate the debug functions of OpenGL 4.3 if they are not supported.
    // The final version should have a false here to increase the games speed a bit.
    //
    ACGL::init(true);
    // tell ACGL in which subdirectory the shaders are:
    ACGL::Base::Settings::the()->setShaderPath("shader/");

    //GLEW Initialization
    glewExperimental = TRUE; // <--- Depends on your graphics card
    GLenum initStatus = glewInit();
    if (initStatus != GLEW_OK) {
        glfwTerminate();
        ACGL::Utils::error() << "Failed to initialize GLEW" << std::endl;
        exit(-1);
    }

    //
    // Create a SimpleRiftController
    //
    ACGL::HardwareSupport::SimpleRiftController *simpleRiftController = new ACGL::HardwareSupport::SimpleRiftController();
    // use a larger field of view:
    simpleRiftController->setDistortionScaleFactor(1.75f);

    //
    // init whatever you need:
    //
    initRenderer(myWindow, simpleRiftController);
    initAudio();

    //
    // Create a world object
    //
    gWorld = new World();
    gWorld->setPlayerCamera(simpleRiftController);

    //Use mouse input
    gInput = new INPUT(myWindow, simpleRiftController, gWorld);

    /************************************************************************
     * Deferred Shading - Setup
     * *********************************************************************/
    gWorld->setWidthHeight(1600, 900);
    gWorld->InitDS();

    //
    // main loop
    //
    double startTimeInSeconds = glfwGetTime();
    do {
        double now = glfwGetTime() - startTimeInSeconds;

        //
        // shader file reloading once a second:
        // good for development, should be removed for the final version
        //
        static double nextReloadTime = 1.0;
        if (now > nextReloadTime) {
            ACGL::OpenGL::ShaderProgramFileManager::the()->updateAll();
            nextReloadTime = now + 1.0; // check again in one second
        }

        static double nextUpdateTime = 0.1;
        if (now > nextUpdateTime) {
            gWorld->update((int) (1000.0 * 0.1));
            nextUpdateTime = now + 0.1;
        }

        //
        // per frame tasks:
        // you might want to add game logic, physics etc.
        //
        gInput->handleInput();
        renderAudio();
        renderFrame();

        // Swap the front and the back buffer to display the rendered image:
        // If VSync is active, this will block until the next frame can get displayed (up to 16.6msec)
        // Note that in VR you want VSync as tearing lines are very distractive!
        glfwSwapBuffers(myWindow);

        // until either the user pressed the X of the window (in case it has a windowbar (== not fullscreen)
        // or the program signaled to get closed by setting glfwSetWindowShouldClose( window, true ) somewhere!
    } while (!glfwWindowShouldClose(myWindow));

    // clean up:
    delete gWorld;
    delete gInput;

    shutdownAudio();
    shutdownRenderer();
    glfwDestroyWindow(myWindow);
    glfwTerminate();
    exit(0);
}
