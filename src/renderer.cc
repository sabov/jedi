#include <ACGL/Utils/Log.hh>
#include <ACGL/OpenGL/GL.hh> // this has to be included before glfw.h !
#include <ACGL/OpenGL/Debug.hh>
#include <ACGL/HardwareSupport/SimpleRiftController.hh>
#include <ACGL/OpenGL/Objects.hh>
#include <GLFW/glfw3.h>
#include "world/world.hh"

using namespace std;
using namespace ACGL::Utils;
using namespace ACGL::OpenGL;
using namespace ACGL::Scene;

// see main.cc:
extern World *gWorld;

//
// Global variables for the rendering:
// You can move the whole rendering code into a class and make this variables
// members. This was not done here to keep the example code a bit simpler.
//
ACGL::HardwareSupport::SimpleRiftController *gSimpleRiftControllerRenderer;
SharedTexture2D gLeftEyeRendering;
SharedTexture2D gRightEyeRendering;
SharedTexture2D gAnyEyeDepthBuffer;
SharedFrameBufferObject gLeftEyeFBO;
SharedFrameBufferObject gRightEyeFBO;

// in case of stereo rendering store the render size per eye:
glm::uvec2 gPerEyeRenderSize;

// the output size in 2D or the resolition of the HMD:
glm::uvec2 gOutputWindowSize;

// 3D Rift rendering or 2D?
bool gRenderForTheRift = false;

//
// A debug callback gets called for each OpenGL error and also for some warnings and even hints.
// Here it just prints those messages.
//
// If you set a brackpoint in here, you can see where the gl error came from and in which state your application is.
//
void APIENTRY ogl_debug_callback( GLenum _source, GLenum _type, GLuint _id, GLenum _severity, GLsizei /*_length*/, const GLchar *_message, void* /* _userParam*/ )
{
    if (_type == GL_DEBUG_TYPE_ERROR) {
        error() << "<" << _id << "> severity: " << debugSeverityName(_severity) << " source: " << debugSourceName(_source) << ": " << _message << endl;
        // Set a breakpoint in the line above! This way you get a stack trace in case something fails!
    } else {
        debug() << "<" << _id << "> severity: " << debugSeverityName(_severity) << " source: " << debugSourceName(_source) << ": " << _message << endl;
    }

    // delete all errors to not create another error log for the same problem:
    while ( glGetError() != GL_NO_ERROR ) {}
}

//
// Will get called for each window resize, but not for window creation!
//
void resizeCallback( GLFWwindow*, int newWidth, int newHeight )
{
    gOutputWindowSize = glm::uvec2( newWidth, newHeight );

    if (gRenderForTheRift) {
        // update raster size:
        gSimpleRiftControllerRenderer->setOutputViewportSize( gOutputWindowSize );
    } else {
        // Update projection matrix (as the aspect ratio might have changed) in case of 2D rendering:
        gSimpleRiftControllerRenderer->getCamera()->setAspectRatio( (float)newWidth / (float)newHeight );
        gSimpleRiftControllerRenderer->getCamera()->setVerticalFieldOfView( 75.0f );
    }
}

//
// Stuff that has to be done only once:
//
void initRenderer( GLFWwindow *window, ACGL::HardwareSupport::SimpleRiftController *simpleRiftController )
{
    //
    // Register our own OpenGL debug callback:
    //
    GLint v;
    glGetIntegerv( GL_CONTEXT_FLAGS, &v );
    if ((v & GL_CONTEXT_FLAG_DEBUG_BIT) != 0) {
        glDebugMessageCallback( ogl_debug_callback, NULL );
    }

    // Enable V-Sync (e.g. limit rendering to 60 FPS in case the screen is a 60 Hz screen):
    // Increases lag but removes tearing (if only one screen is present), often the best option for VR.
    glfwSwapInterval( 1 );

    // Get the current window size (we don't want to communicate that as global variables):
    int width, height;
    glfwGetWindowSize( window, &width, &height );

    // Set up the Rift.
    // store a pointer to the RiftController which we will use for the final render pass each frame:
    gSimpleRiftControllerRenderer = simpleRiftController;

    // use the render size to init offscreen textures etc.

    // calculate a good offscreen render size:
    gPerEyeRenderSize = gSimpleRiftControllerRenderer->getPhysicalScreenResolution();
    gPerEyeRenderSize.x = gPerEyeRenderSize.x / 2; // for one eye only!
    // 1.5 times higher resolution to decrease sampling artefacts when rendering the distortion.
    // Much higher won't increase quality much, lower might increase the performance but looks worse:
    gPerEyeRenderSize = glm::uvec2( glm::vec2(gPerEyeRenderSize) * 1.5f );

    // note that all intermediate textures for the rendering except for the final images
    // can be shared between the two framebuffers (reuse them):
    // (TextureRectangle would also work, but the ACGL Rift post-process does not support them yet - it's on my TODO)
    gLeftEyeRendering  = SharedTexture2D( new Texture2D( gPerEyeRenderSize ) );
    gRightEyeRendering = SharedTexture2D( new Texture2D( gPerEyeRenderSize ) );
    gAnyEyeDepthBuffer = SharedTexture2D( new Texture2D( gPerEyeRenderSize, GL_DEPTH24_STENCIL8 ) );

    // These textures are used as offscreen renderbuffers and don't need MipMaps
    // Note that the filtering default is assuming MipMaps but you need to generate them
    // manually!
    gLeftEyeRendering->setMinFilter(  GL_LINEAR );
    gRightEyeRendering->setMinFilter( GL_LINEAR );
    gAnyEyeDepthBuffer->setMinFilter( GL_LINEAR );

    gLeftEyeFBO = SharedFrameBufferObject( new FrameBufferObject() );
    gLeftEyeFBO->attachColorTexture( "oColor",  gLeftEyeRendering );
    gLeftEyeFBO->setDepthTexture(               gAnyEyeDepthBuffer );

    gRightEyeFBO = SharedFrameBufferObject( new FrameBufferObject() );
    gRightEyeFBO->attachColorTexture( "oColor",  gRightEyeRendering );
    gRightEyeFBO->setDepthTexture(               gAnyEyeDepthBuffer );

    //
    // More complex engines will need more textures as render targets and more framebuffers.
    // Note that only the last renderpass needs two framebuffers for both eyes, everything else
    // can be shared.
    //

    //
    // Stuff that is window size dependent and has to be updated if the window resizes,
    // to prevent code dublication, just call the resize function once:
    //
    resizeCallback( window, width, height );

    // Get informed if the window gets resized later on again.
    // Starting in fullscreen can trigger additional resizes right at the beginning (seen on KDE).
    glfwSetWindowSizeCallback(  window, resizeCallback );

    // define the color the glClear should draw:
    glClearColor( 0.8f, 0.8f, 1.0f, 1.0f );
    glEnable( GL_DEPTH_TEST );
}

void shutdownRenderer()
{
    // delete all resources:
    // all resources with Shared* pointers are reference counted and don't need an explicit delete!
}

// assumes a framebuffer is bound to render into and glViewport is set correctly
void renderScene()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    gWorld->render();
}

//
// Does all the rendering for the frame. A more complex application
// would render (and prepare to render) everything that is independent on
// the head rotation before the head rotation gets read out of the Rift to minimize
// latency (every millisecond counts!). For simplicity reasons we don't do this here.
void renderFrame()
{
    //
    // Render shadow maps and other stuff that has to be updated each frame but is
    // independent on the exact camera position ( == independent on the eye) first
    // ( and only once! )
    //


    if (gRenderForTheRift) {
        // as the viewport gets changed during rendering onto the screen
        // (gSimpleRiftControllerRenderer->renderDistorted), this has to be reset each
        // frame:
        glViewport( 0, 0, gPerEyeRenderSize.x, gPerEyeRenderSize.y );

        //
        // Now render the left eye into a texture:
        //
        gSimpleRiftControllerRenderer->getCamera()->setEye( GenericCamera::EYE_LEFT );
        gLeftEyeFBO->bind();
        renderScene();

        //
        // Now render the right eye into a texture:
        //
        gSimpleRiftControllerRenderer->getCamera()->setEye( GenericCamera::EYE_RIGHT );
        gRightEyeFBO->bind();
        renderScene();

        //
        // Now combine both views into a distorted, Rift compatible view:
        //
        gSimpleRiftControllerRenderer->renderDistorted( gLeftEyeRendering, gRightEyeRendering );
    } else {

        glViewport( 0, 0, gOutputWindowSize.x, gOutputWindowSize.y );

        //
        // Render the left eye onto the screen as the only eye:
        // (note: this mode is not complete ;-) it would be better to also deactivate
        // the stereo option of the camera)
        //
        gSimpleRiftControllerRenderer->getCamera()->setEye( GenericCamera::EYE_LEFT );

        // the screen is framebuffer 0, bind that:
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );

        renderScene();
    }
}
