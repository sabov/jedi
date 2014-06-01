/*****************************************************************
 *
 * EventListenerClass-Interface, listen for and handle Events
 * and EventData
 *
 *****************************************************************/

#ifndef GLGEVENTLISTENER_H
#define GLGEVENTLISTENER_H

#include <boost/smart_ptr/shared_ptr.hpp>
#include "GLGBaseEvent.h"

namespace GameLogic {

//
// Typedefs and class headers
//
class                                       IEventListener  ;
typedef boost::shared_ptr<IEventListener>   EventListenerPtr;

/*
 * Eventlistener-Interface
 * Every claass interested in events should inherit rom this class
*/

class IEventListener
{
public:
    explicit IEventListener() {}
    virtual ~IEventListener() {}

    virtual bool VHandleEvent( IEventData const & _event ) = 0;
};

}   //Namespace

#endif // GLGEVENTLISTENER_H
