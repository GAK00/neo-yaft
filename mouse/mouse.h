#define TRYOPEN_MS (5000)
#define POLL_MS (25)

static struct evdev_mon_t evt_devices[MAX_EVT_DEVS] = {0};

bool mouse_dowork(struct terminal_t *term)
{
    static uint64_t last_try_open_ms = 0;
    static uint64_t last_poll_ms = 0;
    struct timespec ts = {0};
    if(clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
    {
        return false;
    }
    uint64_t now_ms = ts.tv_sec * 1000;
    now_ms += (uint64_t)(ts.tv_nsec /1000000);

    if((now_ms - last_poll_ms) < POLL_MS)
    {
        return false;
    }

    bool evt_occured = false;
    last_poll_ms = now_ms;
    for(int i = 0; i < MAX_EVT_DEVS; i++)
    {
        if(!evt_devices[i].opened)
            continue;
        evt_occured = mouse_event_process(&(evt_devices[i]), term) || evt_occured;
    }

    if((now_ms - last_try_open_ms) < TRYOPEN_MS)
    {
        return evt_occured;
    }

    last_try_open_ms = now_ms;

    for(int i = 0; i < MAX_EVT_DEVS; i++)
    {
        if(evt_devices[i].opened)
            continue;
        attempt_open_evt(&(evt_devices[i]), i);
    }
    return evt_occured;
}