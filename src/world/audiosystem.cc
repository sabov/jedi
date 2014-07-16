#include "audiosystem.hh"


void CAudioSystem::clear()
{
    for ( unsigned long i = 0 ; i < mSoundMap.size(); ++i )
        delete mSoundMap[i];
}

bool CAudioSystem::AddSound(std::string _filename, const GameLogic::EventType &_eventType)
{
    bool success = false;

    unsigned long hash = _eventType.getHashValue() ;

    mSoundMap[ hash ] = new SimpleSound( _filename );

    if ( mSoundMap[ hash ] == NULL )
    {
        mSoundMap.erase(hash);
        return success;
    }

    mSoundMap[ hash ]->setLooping(false);

    success = true;
    return success;
}

bool CAudioSystem::VHandleEvent(const GameLogic::IEventData &_event)
{
    unsigned long hash = _event.VGetEventType().getHashValue();

    mSoundMap[ hash ]->play();

    return true;
}
