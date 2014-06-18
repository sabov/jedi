#include "AL.hh"
#include "audio.hh"
#include <ACGL/Utils/Log.hh>
#include "../world/world.hh"

using namespace std;
using namespace ACGL::Utils;

// the audio device is the soundcard to use
ALCdevice *audioDevice = NULL;

// the context is similar to the OpenGL context we get when opening a window, here we have to
// grab one explicitly
ALCcontext *audioContext = NULL;

extern World *gWorld;

// Will configure an audio context so sounds can be played with OpenAL:
// If this fails, applicationHasNoAudio() will return true, this can be used to quit all audio
// functions quickly.
void initAudio() {
    // open the default audio device / soundcard:
    audioDevice = alcOpenDevice(NULL);
    if (audioDevice == NULL) {
        error() << "OpenAL problem: Can't connect to soundcard" << endl;
        return;
    }

    // if the device is open, create a context:
    audioContext = alcCreateContext(audioDevice, NULL);
    if (audioContext == NULL) {
        error() << "OpenAL problem: Can't create a context" << endl;
        alcCloseDevice(audioDevice);
        return;
    }

    // activate the context (only needed once as we only have one in this application):
    if (!alcMakeContextCurrent(audioContext)) {
        error() << "OpenAL problem: Can't activate context" << endl;
        alcDestroyContext(audioContext);
        alcCloseDevice(audioDevice);
        return;
    }
}

// small helper:
// if the context is not set, the initialisation failed and there is no audio in this application!
bool applicationHasNoAudio() {
    return (audioContext == NULL);
}

void renderAudio() {
    if (applicationHasNoAudio())
        return;

    //
    // place the player in the world, this is equivalent to define the
    // view matrix in rendering.
    glm::vec3 playerPosition = gWorld->getPlayerPosition();
    ALfloat playerOrientation[6];
    gWorld->getPlayerOrientation(playerOrientation);
    alListener3f( AL_POSITION, playerPosition.x, playerPosition.y, playerPosition.z);
    alListener3f( AL_VELOCITY, 0.0f, 0.0f, 0.0f); // TODO: give the player a velocity for free doppler effects!
    alListenerfv( AL_ORIENTATION, playerOrientation);
    printOpenALErrors();

    //
    // other audio sources are playing automatically when they got started,
    // maybe you want to move them around here, maybe the objects will update themself.

}

void shutdownAudio() {
    if (applicationHasNoAudio())
        return;

    // deactivate our context, delete it and close the connection to the sound card:
    alcMakeContextCurrent (NULL);
    alcDestroyContext(audioContext);
    alcCloseDevice(audioDevice);
}

void printOpenALErrors() {
    ALCenum errorCode = alGetError();
    if (errorCode != AL_NO_ERROR) {
        error() << "OpenAL error occured :-(" << endl;
    }
}
