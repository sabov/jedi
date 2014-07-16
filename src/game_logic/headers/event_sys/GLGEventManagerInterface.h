/**********************************************************************
 *
 * Interface for the EventManagerClass, defining several virtual
 * methods for managing the Events in the game
 *
 **********************************************************************/

#include <string>
#include <cassert>
#include <boost/smart_ptr/shared_ptr.hpp>
#include "GLGBaseEvent.h"
#include "GLGEventListener.h"

#ifndef GLGEVENTMANAGERINTERFACE_H
#define GLGEVENTMANAGERINTERFACE_H

namespace GameLogic {

class                                       IEventManager   ;
typedef boost::shared_ptr<IEventManager>    IEventManagerPtr;

class IEventManager
{
public:
    enum eConstants { kINFINITE = 0xffffffff };
    explicit IEventManager () ;
    virtual ~IEventManager() {}

    /* Register a handler for a specific event type, implicitly the event
     * type will be added to the known event types if not already known.
     *
     * The function will return false on failure for any reason.
     * The only really anticipated failure reason is if the input event type
     * is bad ( e.g. known-ident number with different signature text, or
     * signature text is empty )
     */
    virtual bool VAddListener( EventListenerPtr const & _Listener, EventType const & _Type ) = 0;

    /* Remove a listener/type pairing from internal tables.
     * Returns false if  the pairing was not found.
     */
    virtual bool VDelListener ( EventListenerPtr const & _Listener, EventType const & _Type ) = 0;

    /* Fire off event - synchronous - do it now.
     * Returns true if the event was consumed, false if not. Note that it is
     * acceptable for all event listeners to act on an event and not consume it,
     * this return signature exists to allow complete propagation of that shred
     * of information from the internals of this sysem to outside users.
     * Thsi function breaks the the paradigm of remote event handling, but just
     * in case it is needed, it is provided here.
     */
    virtual bool VTrigger ( IEventData const & _Event ) const = 0;

    /* Fire off event - asynchronous - do it when the event system VTick()
     * method is called.
     * Returns true, if the message was added to the processing queue, false
     * otherwise.
     */
    virtual bool VQueueEvent( IEventDataPtr const & _Event ) = 0;

    /* Find next available instance of named event type and remove it from
     * processing queue.
     * If "_allOfType"  is set to true, all events of specified event type are
     * removed.
     * Returns true if eventwas found and removed, false otherwise.
     */
    virtual bool VAbortEvent( EventType const & _Type, bool _allOfType = false ) = 0;

    /* Alow for processing of any queued messages, optionally specify a
     * processing time limit so hat the event processing does not take too long.
     * (There may be the risk that all messages may not get processed!!)
     * Return true if all messages ready for processing were completed, false otherwise.
     */
    virtual bool VTick ( unsigned int long _maxMillisecs = kINFINITE ) = 0;

    /* Information lookup function
     * Validate an event type, this does NOT add it to the internal registry
     * , only verifies that i is legal ( e.g. either the ident number is not
     * yet assigned, or it is assigned to matching signature text, and the signature
     * text is not empty ).
     */
    virtual bool VValidateEventType( EventType const & _Type ) const = 0;

protected:
/*
    /* Internal use only accessor for the static methods in the helper
     * to use to get the active global instance.
     * /

    static IEventManagerPtr Get();

    /* Following methods are declared friends in order to get access to the
     * Get() method. Since there is no other private entity declared in this
     * class, this does not break encapsulation, but allows us to do this without
     * requiring macros or old-style mechanisms.
     * /

    friend bool safeAddListener( EventListenerPtr const & _Listener, EventType const & _Type );
    friend bool safeDelListener( EventListenerPtr const & _Listener, EventType const & _Type );
    friend bool safeTriggerEvent( IEventData const & _Event );
    friend bool safeQueueEvent( IEventDataPtr const & _Event );
    friend bool safeAbortEvent( EventType const & _Type, bool _allOfType = false );
    friend bool safeTickEventManager( unsigned int long _maxMillisecs = kINFINITE );
    friend bool safeValidateEventType( EventType const & _Type );

    bool safeAddListener(EventListenerPtr const & _Listener, EventType const & _Type )
    {
        assert(IEventManager::Get());
        return IEventManager::Get()->VAddListener( _Listener, _Type ) ;
    }

    bool safeDelListener(EventListenerPtr const & _Listener, EventType const & _Type )
    {
        assert(IEventManager::Get());
        return IEventManager::Get()->VDelListener( _Listener, _Type ) ;
    }

    bool safeTriggerEvent( IEventData const & _Event )
    {
        assert(IEventManager::Get());
        return IEventManager::Get()->VTrigger( _Event );
    }

    bool safeQueueEvent( IEventDataPtr const & _Event )
    {
        assert(IEventManager::Get());
        return IEventManager::Get()->VQueueEvent( _Event );
    }

    bool safeAbortEvent( EventType const & _Type, bool _allOfType )
    {
        assert(IEventManager::Get());
        return IEventManager::Get()->VAbortEvent( _Type, _allOfType );
    }

    bool safeTickEventManager( unsigned int long _maxMillisecs )
    {
        assert(IEventManager::Get());
        return IEventManager::Get()->VTick( _maxMillisecs );
    }

    bool safeValidateEventType( EventType const & _Type )
    {
        assert(IEventManager::Get());
        return IEventManager::Get()->VValidateEventType( _Type );
    }
*/
};

} //Namespace

#endif // GLGEVENTMANAGERINTERFACE_H
