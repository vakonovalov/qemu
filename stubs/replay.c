#include "replay/replay.h"
#include "sysemu/sysemu.h"
#include "qemu/notify.h"
#include "ui/input.h"

ReplayMode replay_mode;

int64_t replay_save_clock(unsigned int kind, int64_t clock)
{
    abort();
    return 0;
}

int64_t replay_read_clock(unsigned int kind)
{
    abort();
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

bool replay_events_enabled(void)
{
    return false;
}

void qemu_system_shutdown_request(void)
{
}
