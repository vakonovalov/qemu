#include "replay/replay.h"
#include "sysemu/sysemu.h"

ReplayMode replay_mode;

int64_t replay_save_clock(unsigned int kind, int64_t clock)
{
}

int64_t replay_read_clock(unsigned int kind)
{
    return 0;
}

bool replay_checkpoint(ReplayCheckpoint checkpoint)
{
    return 0;
}

int runstate_is_running(void)
{
    return 0;
}

uint64_t replay_get_current_step(void)
{
    return 0;
}

bool replay_events_enabled(void)
{
    return false;
}

void replay_add_bh_event(void *bh, uint64_t id)
{
}
