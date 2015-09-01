/*
 * replay.c
 *
 * Copyright (c) 2010-2015 Institute for System Programming
 *                         of the Russian Academy of Sciences.
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 *
 */

#include "qemu-common.h"
#include "replay.h"
#include "replay-internal.h"
#include "qemu/timer.h"

ReplayMode replay_mode = REPLAY_MODE_NONE;

ReplayState replay_state;

bool replay_next_event_is(int event)
{
    bool res = false;

    /* nothing to skip - not all instructions used */
    if (replay_state.instructions_count != 0) {
        assert(replay_data_kind == EVENT_INSTRUCTION);
        return event == EVENT_INSTRUCTION;
    }

    while (true) {
        if (event == replay_data_kind) {
            res = true;
        }
        switch (replay_data_kind) {
        default:
            /* clock, time_t, checkpoint and other events */
            return res;
        }
    }
    return res;
}

uint64_t replay_get_current_step(void)
{
    return cpu_get_icount_raw();
}

int replay_get_instructions(void)
{
    int res = 0;
    replay_mutex_lock();
    if (replay_next_event_is(EVENT_INSTRUCTION)) {
        res = replay_state.instructions_count;
    }
    replay_mutex_unlock();
    return res;
}

void replay_account_executed_instructions(void)
{
    if (replay_mode == REPLAY_MODE_PLAY) {
        replay_mutex_lock();
        if (replay_state.instructions_count > 0) {
            int count = (int)(replay_get_current_step()
                              - replay_state.current_step);
            replay_state.instructions_count -= count;
            replay_state.current_step += count;
            if (replay_state.instructions_count == 0) {
                assert(replay_data_kind == EVENT_INSTRUCTION);
                replay_finish_event();
                /* Wake up iothread. This is required because
                   timers will not expire until clock counters
                   will be read from the log. */
                qemu_notify_event();
            }
        }
        replay_mutex_unlock();
    }
}

bool replay_exception(void)
{
    if (replay_mode == REPLAY_MODE_RECORD) {
        replay_save_instructions();
        replay_mutex_lock();
        replay_put_event(EVENT_EXCEPTION);
        replay_mutex_unlock();
        return true;
    } else if (replay_mode == REPLAY_MODE_PLAY) {
        bool res = replay_has_exception();
        if (res) {
            replay_mutex_lock();
            replay_finish_event();
            replay_mutex_unlock();
        }
        return res;
    }

    return true;
}

bool replay_has_exception(void)
{
    bool res = false;
    if (replay_mode == REPLAY_MODE_PLAY) {
        replay_account_executed_instructions();
        replay_mutex_lock();
        res = replay_next_event_is(EVENT_EXCEPTION);
        replay_mutex_unlock();
    }

    return res;
}

bool replay_interrupt(void)
{
    if (replay_mode == REPLAY_MODE_RECORD) {
        replay_save_instructions();
        replay_mutex_lock();
        replay_put_event(EVENT_INTERRUPT);
        replay_mutex_unlock();
        return true;
    } else if (replay_mode == REPLAY_MODE_PLAY) {
        bool res = replay_has_interrupt();
        if (res) {
            replay_mutex_lock();
            replay_finish_event();
            replay_mutex_unlock();
        }
        return res;
    }

    return true;
}

bool replay_has_interrupt(void)
{
    bool res = false;
    if (replay_mode == REPLAY_MODE_PLAY) {
        replay_account_executed_instructions();
        replay_mutex_lock();
        res = replay_next_event_is(EVENT_INTERRUPT);
        replay_mutex_unlock();
    }
    return res;
}