#ifndef EVENT_HANDLER_CENTER_HPP
#define EVENT_HANDLER_CENTER_HPP

#include "LoggingSystem.HPP"

class EventHandlerCenter
{
public:
    EventHandlerCenter();
    void Start();
private:
    LoggingSystem LOG;
};

#endif // EVENT_HANDLER_CENTER_HPP
