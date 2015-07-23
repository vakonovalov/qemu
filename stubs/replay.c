#include "replay/replay.h"
#include "sysemu/sysemu.h"
#include "qemu/notify.h"
#include "ui/input.h"
#include "block/aio.h"
#include "block/thread-pool.h"

ReplayMode replay_mode;

int64_t replay_save_clock(unsigned int kind, int64_t clock)
{
    return 0;
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

void qemu_system_shutdown_request(void)
{
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

void replay_add_thread_event(void *opaque, void *opaque2, uint64_t id)
{
}

void qemu_input_event_send_impl(QemuConsole *src, InputEvent *evt)
{
}

void qemu_input_event_sync_impl(void)
{
}

void aio_bh_call(QEMUBH *bh)
{
}

void thread_pool_work(ThreadPool *pool, void *r)
{
}
