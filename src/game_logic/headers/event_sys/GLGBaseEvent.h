/*******************************************************************************
 *
 *  Defines the BaseClass for Events in the Eventsystem. All Events should
 *  inherit from CBaseEventData
 *
 ******************************************************************************/

#ifndef GLGBASEEVENT_H
#define GLGBASEEVENT_H

#include <boost/smart_ptr/shared_ptr.hpp>
#include <ostream>
#include "hashed_string.h"

namespace GameLogic {

//
//Typedefs and class headers
//

typedef CHashedString   EventType   ;

class                                       IEventData      ;
class                                       CBaseEventData  ;
typedef boost::shared_ptr<IEventData>       IEventDataPtr   ;
typedef boost::shared_ptr<CBaseEventData>   EventDataPtr    ;

/*
 * Define Interface for BaseEventData
 * Mainly defines virtual getter-functions
 */
class IEventData
{
public:
    virtual const EventType & VGetEventType() const = 0 ;
    virtual float VGetTimeSTamp() const = 0 ;
    virtual void VSerialize(std::ostream &out) const = 0 ;
};

/*
 * Define abstract base class for events in the game
 */
class CBaseEventData : public IEventData
{
protected:
    const float m_TimeStamp ;

public:
    explicit CBaseEventData( const float _timeStamp = 0.0f )
        : m_TimeStamp ( _timeStamp ){}
    virtual ~CBaseEventData() {}
    virtual const EventType & VGetEventType() const = 0 ;
    virtual float VGetTimeSTamp() const { return m_TimeStamp; }
    virtual void VSerialize(std::ostream &out) const {}
};

}   //Namespace

#endif // GLGBASEEVENT_H
