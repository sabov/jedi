/**************************************************************************
 *
 * Define several events for the game here
 *
 * ***********************************************************************/

#ifndef EVENTS_HH
#define EVENTS_HH

#include "event_sys/GLGBaseEvent.h"
#include "event_sys/GLGEventManager.h"

class CEvtData_WorldInitialized : public GameLogic::CBaseEventData
{
private:
    static const GameLogic::EventType   m_EventType ;
public:
    explicit CEvtData_WorldInitialized () : GameLogic::CBaseEventData(0.0) {}
    explicit CEvtData_WorldInitialized (double _timeStamp) : GameLogic::CBaseEventData(_timeStamp) {}

    virtual const GameLogic::EventType & VGetEventType() const { return m_EventType; }
    virtual ~CEvtData_WorldInitialized() {}
    virtual void VSerialize(std::ostream &out) const { out << "Game-World initialized..." ; }
    static GameLogic::EventType GetEventType() { return m_EventType; }
};

class CEvtData_ToggleSword : public GameLogic::CBaseEventData
{
private:
    static const GameLogic::EventType   m_EventType ;
public:
    explicit CEvtData_ToggleSword () : GameLogic::CBaseEventData(0.0) {}
    explicit CEvtData_ToggleSword (double _timeStamp) : GameLogic::CBaseEventData(_timeStamp) {}

    virtual const GameLogic::EventType & VGetEventType() const { return m_EventType; }
    virtual ~CEvtData_ToggleSword() {}
    virtual void VSerialize(std::ostream &out) const { out << "Toggle Sword ..." ; }
    static GameLogic::EventType GetEventType() { return m_EventType; }
};

class CEvtData_CollisionLightSaber : public GameLogic::CBaseEventData
{
private:
    static const GameLogic::EventType   m_EventType ;
public:
    explicit CEvtData_CollisionLightSaber () : GameLogic::CBaseEventData(0.0) {}
    explicit CEvtData_CollisionLightSaber (double _timeStamp) : GameLogic::CBaseEventData(_timeStamp) {}

    virtual const GameLogic::EventType & VGetEventType() const { return m_EventType; }
    virtual ~CEvtData_CollisionLightSaber() {}
    virtual void VSerialize(std::ostream &out) const { out << "Collision with sword ..." ; }
    static GameLogic::EventType GetEventType() { return m_EventType; }
};

class CEvtData_CollisionPlayer : public GameLogic::CBaseEventData
{
private:
    static const GameLogic::EventType   m_EventType ;
public:
    explicit CEvtData_CollisionPlayer () : GameLogic::CBaseEventData(0.0) {}
    explicit CEvtData_CollisionPlayer (double _timeStamp) : GameLogic::CBaseEventData(_timeStamp) {}

    virtual const GameLogic::EventType & VGetEventType() const { return m_EventType; }
    virtual ~CEvtData_CollisionPlayer() {}
    virtual void VSerialize(std::ostream &out) const { out << "Collision with player ..." ; }
    static GameLogic::EventType GetEventType() { return m_EventType; }
};

#endif // EVENTS_HH
