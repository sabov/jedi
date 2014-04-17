#pragma once

#include "AL.hh"
#include <string>
#include <ACGL/Math/Math.hh>

/*
 * A SimpleSound is a trivial OpenAL wrapper for one sound object.
 *
 * An audio source (AS) is the description of a piece of audio (how to play it)
 * An audio buffer (AB) is the wave to play (what to play)
 * Both are defined by ALuint handles (like pointers to the objects)
 *
 * This class capsuled both. Note, that in case one sound should be played multiple times
 * at the same time it is more usefull to split this up and have one buffer for the data
 * but multiple sources playing the data!
 */


class SimpleSound {
public:
    SimpleSound( const std::string &wavFileName );
    ~SimpleSound();

    void setPosition( glm::vec3 position );
    void setVelocity( glm::vec3 velocity );

    void setLooping( bool b );

    void play();
private:

    ALuint mSource;
    ALuint mBuffer;
};
