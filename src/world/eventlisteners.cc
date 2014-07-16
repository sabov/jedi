#include "eventlisteners.hh"
#include <iomanip>


bool EventDebugOuput::VHandleEvent(const GameLogic::IEventData &_event)
{
    std::cout << "Event: " << std::hex << &_event
              << ", Time: " << _event.VGetTimeSTamp()
              << ", Type: " << _event.VGetEventType().getString()
              << ", Hash: " << _event.VGetEventType().getHashValue()
              << std::endl;
    return true;
}
