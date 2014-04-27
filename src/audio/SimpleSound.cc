#include "SimpleSound.hh"
#include "loadWavFile.hh"

SimpleSound::SimpleSound( const std::string &wavFileName )
{
    // load the data:
    mBuffer = createBufferFromWAV( wavFileName );

    // generate a default sound description:
    alGenSources((ALsizei)1, &mSource );

    alSourcei( mSource, AL_BUFFER, mBuffer );
}

SimpleSound::~SimpleSound()
{
    alDeleteSources((ALsizei) 1, &mSource );
    alDeleteBuffers((ALsizei) 1, &mBuffer );
}

void SimpleSound::setPosition( glm::vec3 position )
{
    alSource3f( mSource, AL_POSITION, position.x, position.y, position.z );
}

void SimpleSound::setVelocity( glm::vec3 velocity )
{
    alSource3f( mSource, AL_VELOCITY, velocity.x, velocity.y, velocity.z );
}

void SimpleSound::setLooping( bool b )
{
    alSourcei( mSource, AL_LOOPING, (ALint)b );
}

void SimpleSound::play()
{
    alSourcePlay( mSource );
}

bool SimpleSound::isPlaying()
{
    ALint sourceState;
    alGetSourcei(mSource, AL_SOURCE_STATE, &sourceState);
    return sourceState == AL_PLAYING;
}
