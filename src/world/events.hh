/**************************************************************************
 *
 * Define several events for the game here
 *
 * ***********************************************************************/

#ifndef EVENTS_HH
#define EVENTS_HH

#include "event_sys/GLGBaseEvent.h"
#include "event_sys/GLGEventManager.h"

typedef unsigned int ActorID    ;

class CEvtData_DestroyActor : public GameLogic::CBaseEventData
{
private:
    static const GameLogic::EventType   m_EventType ;
    ActorID                             m_id        ;
public:
    explicit CEvtData_DestroyActor ( ActorID _id )
        : m_id(_id) {}

    virtual const GameLogic::EventType & VGetEventType() const { return m_EventType; }
    virtual ~CEvtData_DestroyActor() {}
    virtual void VSerialize(std::ostream &out) const { out << m_id; }

};

#endif // EVENTS_HH
