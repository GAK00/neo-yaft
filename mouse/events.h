int if_open(const char* path, int flags, void* ctx)
{
    (void)(ctx);
    int fd = open(path, flags);
    return fd < 0 ? -errno : fd;
}

void if_close(int fd, void* ctx)
{
    (void)(ctx);
    close(fd);
}

struct libinput* mouse_events_init(void)
{
    static const struct libinput_interface input_if = {.open_restricted = if_open, .close_restricted = if_close};
	struct udev *udev = udev_new();
    struct libinput* lib_input = NULL;
    if(udev)
    {
        struct libinput* lib_input = libinput_udev_create_context(&input_if, NULL, udev);
        if(libinput_udev_assign_seat(lib_input, "seat0"))
        {
		    libinput_unref(lib_input);
            lib_input = NULL;
        }
    }
    return lib_input;
}

struct libinput_event * mouse_events_get_event(struct libinput* lib_input)
{
    struct libinput_event * evt = libinput_get_event(lib_input);
    enum libinput_event_type evt_typ = libinput_event_get_type(evt);
    if(evt_typ == LIBINPUT_EVENT_NONE)
    {    
        libinput_dispatch(lib_input);
        evt = libinput_get_event(lib_input);
        evt_typ = libinput_event_get_type(evt);
    }

    if(evt_typ == LIBINPUT_EVENT_DEVICE_ADDED || evt_typ == LIBINPUT_EVENT_DEVICE_REMOVED)
    {
        struct libinput_device* device = libinput_event_get_device(evt);
        libinput_device_config_tap_set_enabled(device, LIBINPUT_CONFIG_TAP_ENABLED);
        libinput_device_config_tap_set_drag_enabled(device, LIBINPUT_CONFIG_DRAG_ENABLED);
        libinput_device_config_dwt_set_enabled(device, LIBINPUT_CONFIG_DWT_DISABLED);
        libinput_device_config_click_set_method(device, LIBINPUT_CONFIG_CLICK_METHOD_NONE);
    }

    return evt_typ != LIBINPUT_EVENT_NONE ? evt : NULL;
}