/**************************************************************************
 *
 * Define several eventlisteners for the game here
 *
 * ***********************************************************************/

#ifndef EVENTLISTENERS_HH
#define EVENTLISTENERS_HH

#include <iostream>
#include "event_sys/GLGEventListener.h"

class EventDebugOuput : public GameLogic::IEventListener
{
public:
    explicit EventDebugOuput()
    {
        std::cout << "EventDebugOuput created." << std::endl;
    }
    ~EventDebugOuput()
    {
        std::cout << "EventDebugOuput removed." << std::endl;
    }

    virtual bool VHandleEvent(const GameLogic::IEventData &_event);
};

#endif // EVENTLISTENERS_HH
