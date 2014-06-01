#include "event_sys/GLGEventManager.h"

namespace GameLogic {

bool CEventManager::VAddListener(const EventListenerPtr &_Listener, const EventType &_Type)
{
    if ( !VValidateEventType( _Type ) )
        return false;

    //check / update the list
    EventTypeSet::iterator evIt = m_TypeList.find( _Type );
    if ( evIt == m_TypeList.end() )
        m_TypeList.insert( _Type );

    //find listener map entry, create one if no table already exists for this entry..
    EventListenerMap::iterator elmIt = m_Registry.find( _Type.getHashValue() ) ;

    if ( elmIt == m_Registry.end() )
    {
        EventListenerMapIRes elmIRes = m_Registry.insert(
                    EventListenerMapEntry( _Type.getHashValue(), EventListenerTable() ) );

        //could not insert into map
        if ( elmIRes.second == false )
            return false;

        //should not be possible, but check anyway
        if ( elmIRes.first == m_Registry.end() )
            return false;

        //store it so we can update the mapped list next...
        elmIt = elmIRes.first;
    }

    /* Update the mapped list of listeners, walk the existing list (if any extries)
     * to prevent duplicate addition of listeners. This is a bit more costly at
     * registration time but will prevent the hard-to-notice duplicate event
     * propagation sequences that would happen if double-entries were allowed.
     */
    EventListenerTable& evlTable = (*elmIt).second;

    for ( EventListenerTable::iterator it = evlTable.begin(), itEnd = evlTable.end(); it != itEnd ; it++)
    {
        bool bListenerMatch = (*it == _Listener);

        if ( bListenerMatch )
            return false;
    }

    //event type validated, event listener validated,
    //event listener not already in map, add it.
    evlTable.push_back( _Listener );
    return true;
}

bool  CEventManager::VDelListener(const EventListenerPtr &_Listener, const EventType &_Type)
{
    if ( !VValidateEventType( _Type ) )
        return false;

    bool rc = false;

    //brute force
    for ( EventListenerMap::iterator it = m_Registry.begin(), itEnd = m_Registry.end();
          it != itEnd ; it++ )
    {
        //unsigned int const  kEventId = it->first;
        EventListenerTable & table = it->second;

        for ( EventListenerTable::iterator it2 = table.begin(), it2End = table.end();
              it2 != it2End; it2++)
        {
            if ( *it2 == _Listener )
            {
                //found match, remove it from table
                table.erase( it2 );
                rc = true;
                //early-quit the inner loop as addListener() code
                //ensures that each listener can only appear in one
                //event's processing list once
                break;
            }
        }
    }
    return rc;
}

bool CEventManager::VTrigger(const IEventData &_Event) const
{
    if ( !VValidateEventType( _Event.VGetEventType() ) )
        return false;

    EventListenerMap::const_iterator itWC = m_Registry.find(0);

    //listeners that handle all type of events
    if ( itWC != m_Registry.end() )
    {
        EventListenerTable const & table = itWC->second;

        for ( EventListenerTable::const_iterator it2 = table.begin(), it2End = table.end();
              it2 != it2End; it2++ )
        {
            (*it2)->VHandleEvent( _Event );
        }
    }

    EventListenerMap::const_iterator it = m_Registry.find( _Event.VGetEventType().getHashValue() );

    if ( it == m_Registry.end() )
        return false;

    EventListenerTable const & table = it->second;

    bool processed = false;

    //listeners that handle this particular event
    for ( EventListenerTable::const_iterator it2 = table.begin(), it2End = table.end();
          it2 != it2End; it2++ )
    {
        EventListenerPtr listener = *it2;
        if ( listener->VHandleEvent( _Event) )
            processed= true;
    }
    return processed;
}

bool CEventManager::VQueueEvent(const IEventDataPtr &_Event)
{
    assert( m_activeQueue >= 0 );
    assert( m_activeQueue < eNumQueues );

    if ( !VValidateEventType( _Event->VGetEventType() ) )
        return false;

    EventListenerMap::const_iterator it = m_Registry.find( _Event->VGetEventType().getHashValue() );

    if ( it == m_Registry.end() )
    {
        //if global listener is not active then abort inserting
        EventListenerMap::const_iterator itWC = m_Registry.find(0);

        if ( itWC == m_Registry.end() )
        {
            //no listeners for this event, skip it
            return false;
        }
    }
    m_Queues[m_activeQueue].push_back( _Event );

    return true ;
}

bool CEventManager::VAbortEvent(const EventType &_Type, bool _allOfType)
{
    assert( m_activeQueue >= 0 );
    assert( m_activeQueue < eNumQueues );

    if ( !VValidateEventType( _Type ) )
        return false;

    EventListenerMap::const_iterator it = m_Registry.find( _Type.getHashValue() );

    if ( it == m_Registry.end() )
        return false;   //no listeners for this event, skip it

    bool rc = false;

    EventQueue & evtQueue = m_Queues[m_activeQueue];

    for ( EventQueue::iterator it = evtQueue.begin(), itEnd = evtQueue.end();
          it != itEnd ; it++ )
    {
        if ( (*it)->VGetEventType() == _Type )
        {
            it = evtQueue.erase( it );
            rc = true;
            if ( !_allOfType )
                break;
        }
        else
        {
            ++it;
        }
    }
    return rc;
}

bool CEventManager::VTick(unsigned long _maxMillisecs)
{
    std::clock_t t = std::clock();
    unsigned long curMs = static_cast<unsigned long>(t);
    unsigned long maxMs = ( _maxMillisecs == IEventManager::kINFINITE ) ?
                IEventManager::kINFINITE : (curMs + _maxMillisecs );

    EventListenerMap::const_iterator itWC = m_Registry.find(0);

    //swap active queues, make sure new queue is empty after the swap...

    int queueToProcess = m_activeQueue;
    m_activeQueue = ( m_activeQueue + 1 ) % eNumQueues;
    m_Queues[m_activeQueue].clear();

    //now process as many elements as we can (possibly time limited) ...
    //always do AT LEAST one event, if ANY are available ...

    while ( m_Queues[queueToProcess].size() > 0 )
    {
        IEventDataPtr event = m_Queues[queueToProcess].front();
        m_Queues[queueToProcess].pop_front();

        EventType const & eventType = event->VGetEventType();

        EventListenerMap::const_iterator itListeners = m_Registry.find( eventType.getHashValue() );

        if ( itWC != m_Registry.end() )
        {
            EventListenerTable const & table = itWC->second;
            //bool processed = false;

            for ( EventListenerTable::const_iterator it2 = table.begin(), it2End = table.end();
                  it2 != it2End; it2++ )
            {
                (*it2)->VHandleEvent( *event ) ;
            }
        }

        //no listeners currently for this event type, so skip it
        if ( itListeners == m_Registry.end() )
            continue;

        //unsigned int const kEventId = itListeners->first ;
        EventListenerTable const & table = itListeners->second ;

        for ( EventListenerTable::const_iterator it = table.begin(), end = table.end();
              it != end ; it++)
        {
            if ( (*it)->VHandleEvent( *event ) )
                break;
        }

        t = std::clock();
        curMs = static_cast<unsigned long>(t);

        if ( _maxMillisecs != IEventManager::kINFINITE )
        {
            if ( curMs >= maxMs ) //time ran out, abort processing loop
                break;
        }
    }

    //if any events left to process, pus them onto the active queue.
    bool queueFlushed = ( m_Queues[queueToProcess].size() == 0 );

    if ( !queueFlushed )
    {
        while ( m_Queues[queueToProcess].size() > 0 )
        {
            IEventDataPtr event = m_Queues[queueToProcess].back();
            m_Queues[queueToProcess].pop_back();
            m_Queues[m_activeQueue].push_front( event );
        }
    }

    //all done, this pass
    return queueFlushed;
}

bool CEventManager::VValidateEventType(const EventType &_Type) const
{
    if ( std::strlen( _Type.getString() ) )
        return false;

    if ( ( _Type.getHashValue() == 0 ) &&
         (std::strcmp( _Type.getString(), kpWildCardEventType.c_str() ) != 0 ) )
        return false;

    EventTypeSet::const_iterator evIt = m_TypeList.find( _Type );

    if ( evIt == m_TypeList.end() )
    {
        assert( 0 && "Failed validation of an event type; it was probably not registred with the EventManager!" );
        return false;
    }

    return true;
}

EventListenerList CEventManager::GetListenerList(const EventType &_eventType) const
{
    //invalid event type
    if ( !VValidateEventType( _eventType ) )
        return EventListenerList();

    EventListenerMap::const_iterator itListeners = m_Registry.find( _eventType.getHashValue() );

    //no listeners currently for this event type
    if ( itListeners == m_Registry.end() )
        return EventListenerList();

    EventListenerTable const & table = itListeners->second;

    //there was, but is not now, any listeners currently for this event type
    if ( table.size() == 0 )
        return EventListenerList();

    EventListenerList result;
    result.reserve( table.size() );

    for ( EventListenerTable::const_iterator it = table.begin(), end = table.end();
          it != end ; it++ )
    {
        result.push_back( (*it) );
    }
    return result;
}

EventTypeList CEventManager::GetTypeList() const
{
    //no entries
    if ( m_TypeList.size() == 0 )
        return EventTypeList();

    EventTypeList result;
    result.reserve( m_TypeList.size() );

    for ( EventTypeSet::const_iterator it = m_TypeList.begin(), itEnd = m_TypeList.end();
          it != itEnd; it++ )
    {
        result.push_back( *it );
    }

    return result;
}

}   //Namespace
