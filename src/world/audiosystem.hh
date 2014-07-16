#ifndef AUDIOSYSTEM_HH
#define AUDIOSYSTEM_HH

#include <boost/shared_ptr.hpp>
#include <map>
#include <string>
#include "../audio/SimpleSound.hh"
#include "event_sys/GLGEventListener.h"

class                                   CAudioSystem    ;
typedef boost::shared_ptr<CAudioSystem> AudioSystemPtr  ;

//Class for playing different sounds

class CAudioSystem : public GameLogic::IEventListener
{
private:
    std::map<unsigned long, SimpleSound*>   mSoundMap   ;

    void clear();

public:
    CAudioSystem() : GameLogic::IEventListener() {}
    ~CAudioSystem()
    {
        clear();
        mSoundMap.clear();
    }

    //Add new sound for certain event type
    bool AddSound( std::string _filename, const GameLogic::EventType& _eventType );

    //Play sound according to received event type
    virtual bool VHandleEvent(const GameLogic::IEventData &_event);

};

#endif // AUDIOSYSTEM_HH
