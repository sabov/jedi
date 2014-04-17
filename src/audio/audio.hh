/*
 * We suggest OpenAL for audio here.
 *
 * It's a simple audio API which supports 3D audio and even binaural audio (3D audio with headphones)
 * in the OpenAL Soft implementation which can be used on Linux, Windows and MacOS X.
 *
 * The API is designed to be similar to OpenGL (function naming scheme, extension system etc).
 *
 * Audio objects can get created and placed in the world in 3D space. They can have additional attributes
 * like the orientation and speed, so OpenAL can compute effects like the doppler effect on its own.
 *
 * In addition to the sound sources a listener has to be placed which should be linked to the players
 * position and orientation.
 *
 * OpenAL itself does not support loading of any audio data. Note that while ALUT (think of GLUT for OpenAL)
 * does include this, ALUT is not well supported on all plattforms.
 */
#pragma once

void initAudio();
void renderAudio();
void shutdownAudio();

// helper function:
void printOpenALErrors();
