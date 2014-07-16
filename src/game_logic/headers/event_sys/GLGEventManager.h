/************************************************************************
 *
 * Implemenation of EventManager. Implements metods for collecting
 * and delegate events to EventListeners
 *
 ************************************************************************/

#ifndef GLGEVENTMANAGER_H
#define GLGEVENTMANAGER_H

#include "GLGEventManagerInterface.h"
#include <boost/smart_ptr/shared_ptr.hpp>
#include <ctime>
#include <vector>
#include <list>
#include <map>
#include <set>

namespace GameLogic {

//
// Class Headers and Typedefs
//
class                                       CEventManager       ;
typedef boost::shared_ptr<CEventManager>    EventManagerPtr     ;
typedef std::vector<EventListenerPtr>       EventListenerList   ;
typedef std::vector<EventType>              EventTypeList       ;

class CEventManager : public IEventManager
{
private:
    static EventManagerPtr m_Instance  ;

    CEventManager() : IEventManager(), m_activeQueue(0) {}
    CEventManager(const CEventManager&) {}

    typedef std::set<EventType> EventTypeSet;

    //insert result into event type set
    typedef std::pair<EventTypeSet::iterator, bool> EventTypeSetIRes;

    //one list per event type (stored in the map)
    typedef std::list<EventListenerPtr> EventListenerTable;

    //mapping of event ident to lisener list
    typedef std::map<unsigned int, EventListenerTable> EventListenerMap;

    //entry in the event listener map
    typedef std::pair<unsigned int, EventListenerTable> EventListenerMapEntry;

    //insert result into listener map
    typedef std::pair<EventListenerMap::iterator, bool> EventListenerMapIRes;

    //queue of pending- or processing-events
    typedef std::list<IEventDataPtr> EventQueue  ;

    enum eConstants { eNumQueues = 2 };

    //list of registered event types
    EventTypeSet        m_TypeList              ;
    //mapping of event types to listeners
    EventListenerMap    m_Registry              ;
    //double buffered event processing queue (prevents infinite cycles)
    EventQueue          m_Queues[eNumQueues]    ;
    //which queue is actively processing, en-queueing events goes to opposing queue
    int                 m_activeQueue           ;

public:
    //getting the instance
    static EventManagerPtr getInstance()
    {
        if (!m_Instance)
            m_Instance = EventManagerPtr(new CEventManager) ;

        return m_Instance ;
    }
    //destroying instance
    static void destroyInstance()
    {
        m_Instance.reset(); ;
    }
   ~CEventManager()
    {
        m_activeQueue = 0;
    }

    bool VAddListener(const EventListenerPtr &_Listener, const EventType &_Type);
    bool VDelListener(const EventListenerPtr &_Listener, const EventType &_Type);
    bool VTrigger(const IEventData &_Event) const;
    bool VQueueEvent(const IEventDataPtr &_Event) ;
    bool VAbortEvent(const EventType &_Type, bool _allOfType);
    bool VTick(unsigned long _maxMillisecs = kINFINITE);
    bool VValidateEventType(const EventType &_Type) const;

    //  information lookup funcions

    //get the list of listeners associated with a specified event type.
    EventListenerList GetListenerList( EventType const & _eventType ) const;

    //get the list of known event types
    EventTypeList GetTypeList() const;
};

}   //namespace

#endif // GLGEVENTMANAGER_H
