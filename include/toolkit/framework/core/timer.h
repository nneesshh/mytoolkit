#ifndef TIMER_H
#define TIMER_H

#include "declarations.h"

class TTimer
{
public:
	TTimer() { restart(); }

    void restart();
    void stop() { _stopped = true; }

    ticks_t startTicks() { return _startTicks; }
    ticks_t ticksElapsed();
    float timeElapsed() { return ticksElapsed()/1000.0f; }

    bool running() { return !_stopped; }

private:
    ticks_t _startTicks;
    stdext::boolean<false> _stopped;
};

#endif