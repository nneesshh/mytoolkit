#include "timer.h"
#include "clock.h"

void TTimer::restart() {
    _startTicks = g_clock.millis();
    _stopped = false;
}

ticks_t TTimer::ticksElapsed() {
    return g_clock.millis() - _startTicks;
}
