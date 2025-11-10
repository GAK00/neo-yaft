#define EVT_DEV_FILE_PATTERN "/dev/input/event%d"

#define MAX_FILE_PATH_LEN (256)

void attempt_open_evt(struct evdev_mon_t * evt, int event_num)
{
    char file_path[MAX_FILE_PATH_LEN];
    snprintf(file_path,MAX_FILE_PATH_LEN, EVT_DEV_FILE_PATTERN, event_num);
    evt->fd = open(file_path, O_RDONLY | O_NONBLOCK);
    if(evt->fd < 0)
    {
        return;
    }
    if(libevdev_new_from_fd(evt->fd, &evt->dev) < 0)
    {
        return;
    }
    evt->opened = true;
}

bool mouse_event_process(struct evdev_mon_t * evt, struct terminal_t * term)
{
    int res = 0;
    struct input_event ev;
    bool ret = false;
    while((res = libevdev_next_event(evt->dev, LIBEVDEV_READ_FLAG_NORMAL, &ev)) == 0)
    {
        if(ev.type != EV_REL)
            continue;
        if(ev.code == REL_WHEEL)
        {
            ret = true;
            scroll_view(term, ev.value * -2);
        }
    }

    if(res < 0 && res != -EAGAIN)
    {
        libevdev_free(evt->dev);
        close(evt->fd);
        evt->opened = false;
    }
    return ret;
}