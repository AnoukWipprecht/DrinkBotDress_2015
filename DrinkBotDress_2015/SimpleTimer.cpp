#include "SimpleTimer.h"

SimpleTimer::SimpleTimer() {
    long current_millis = millis();

    for (int i = 0; i < MAX_TIMERS; i++) {
        calls[i].enabled = false;
        calls[i].callback = 0;
        calls[i].prev_millis = current_millis;
        calls[i].params = -1;
    }

    num_timers = 0;
}


void SimpleTimer::run() {
    int i;
    long current_millis;

    // get current time
    current_millis = millis();

    for (i = 0; i < MAX_TIMERS; i++) {

        // only process active timers
        if (!calls[i].callback || !calls[i].enabled) continue;

		// is it time to process this timer ?
		if (current_millis - calls[i].prev_millis >= calls[i].delay) {

			// update time
			calls[i].prev_millis = current_millis;

			// "run forever" timers must always be executed
			if (calls[i].maxRuns == RUN_FOREVER) {
				(calls[i].callback)(calls[i].params);
			}
			// other timers get executed the specified number of times

			//BUG take better care of your memory
			//if an earlier short timer gets deleted before an long later one, it gets overriden because
			//of the num_timers just stacks upwards and doesnt look down

			else if (calls[i].numRuns < calls[i].maxRuns) {
				(calls[i].callback)(calls[i].params);
				calls[i].numRuns++;

				// after the last run, delete the timer
				// to save some cycles
				if (calls[i].numRuns >= calls[i].maxRuns) {
					deleteTimer(i);
				}
			}
        }
    }
}

uint8_t SimpleTimer::setTimer(long d, timer_delegate f, uint8_t n) {
	if (num_timers >= MAX_TIMERS) {
        return -1;
    }

    uint8_t free_spot = getAvailableSpot();
    if(free_spot < 0)return -1;

    calls[free_spot].delay = d;
    calls[free_spot].callback = f;
    calls[free_spot].maxRuns = n;
    calls[free_spot].enabled = true;
    calls[free_spot].numRuns = 0;
    calls[free_spot].prev_millis = millis();
    calls[free_spot].params = free_spot + num_timers;

    num_timers = getNumTimers();

    return free_spot;
}

uint8_t SimpleTimer::setTimer(long d, timer_delegate f, uint8_t n, uint8_t param) {
    if (num_timers >= MAX_TIMERS) {
        return -1;
    }

    uint8_t free_spot = getAvailableSpot();
    if(free_spot < 0)return -1;

    calls[free_spot].delay = d;
    calls[free_spot].callback = f;
    calls[free_spot].maxRuns = n;
    calls[free_spot].enabled = true;
    calls[free_spot].numRuns = 0;
    calls[free_spot].prev_millis = millis();
    calls[free_spot].params = param;

    num_timers = getNumTimers();

    return free_spot;
}

uint8_t SimpleTimer::setInterval(long d, timer_delegate f) {
    return setTimer(d, f, RUN_FOREVER);
}


uint8_t SimpleTimer::setTimeout(long d, timer_delegate f) {
    return setTimer(d, f, RUN_ONCE);
}

void SimpleTimer::change_delay(uint8_t numTimer, unsigned long d){
	calls[numTimer].delay = d;
	calls[numTimer].prev_millis = millis();
}

void SimpleTimer::deleteTimer(uint8_t numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    // nothing to disable if no timers are in use
    if (num_timers == 0) {
        return;
    }

    calls[numTimer].callback = 0;
    calls[numTimer].enabled = false;
    calls[numTimer].delay = 0;

    num_timers = getNumTimers();
}


boolean SimpleTimer::isEnabled(uint8_t numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return false;
    }

    return calls[numTimer].enabled;
}


void SimpleTimer::enable(uint8_t numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    calls[numTimer].enabled = true;
}


void SimpleTimer::disable(uint8_t numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    calls[numTimer].enabled = false;
}


void SimpleTimer::toggle(uint8_t numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    calls[numTimer].enabled = !calls[numTimer].enabled;
}

uint8_t SimpleTimer::getAvailableSpot() {
	for (int i = 0; i < MAX_TIMERS; i++) {
		if(calls[i].callback == 0)return i;
	}
	return -1;
}

uint8_t SimpleTimer::getNumTimers() {
	uint8_t numTimers = 0;
    for (int i = 0; i < MAX_TIMERS; i++) {
		if(calls[i].callback && calls[i].enabled)numTimers++;
	}
    return numTimers;
}
