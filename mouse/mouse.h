void handle_scroll_wheel(struct terminal_t* term, struct libinput_event* evt)
{
  struct libinput_event_pointer *p = libinput_event_get_pointer_event(evt);
  if(libinput_event_pointer_has_axis(p, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL))
  {
    int offset = libinput_event_pointer_get_axis_value(p, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL) > 0 ? -1 : 1;
    scroll(term, term->scroll.top, term->scroll.bottom, offset);
  }
}

void mouse_do_work(struct terminal_t* term)
{
    static struct libinput* lib_input = NULL;
    if(!lib_input)
    {
        lib_input = mouse_events_init();
        return;
    }
    
    struct libinput_event * evt = mouse_events_get_event(lib_input);
    if(!evt)
    {
        return;
    }

    switch(libinput_event_get_type(evt))
    {
        case LIBINPUT_EVENT_POINTER_AXIS:
            handle_scroll_wheel(term, evt);
            break;
        default:
            break;
    }
    libinput_event_destroy(evt);
}