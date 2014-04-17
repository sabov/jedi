#include "AL.hh"
#include <string>

//
// Loads a *.wav file and stores the data into an OpenAL buffer.
// The handle to the buffer is returned if the file could get loaded,
// 0 gets returned otherwise.
//
// Note that for 3D Sound only mono sources are usefull as one point in 3D only emmits one
// channel of audio.
//
// You might want to add your own loading functions to support more file formats.
ALuint createBufferFromWAV( const std::string &_filename );
