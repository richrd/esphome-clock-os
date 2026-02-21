|-
    if (id(clockos_menu_main).is_active())
    {
        return;
    }

    it.fill(COLOR_OFF);

    // Config
    int screen_w = 128;
    int screen_h = 64;

    int line_height = 10;
    int y = 0;

    it.print(0, y, id(clockos_font_xs), "ClockOS");
    y += line_height;
    it.print(0, y, id(clockos_font_xs), "A minimal ESPHome OS");
    y += line_height;
    it.print(0, y, id(clockos_font_xs), "for ESP32 devices");
    y += line_height;
    it.print(0, y, id(clockos_font_xs), "Code available at:");
    y += line_height;

    // Horizontal scrolling for the URL line with pause at direction change
    static int scroll_offset = 0;
    static int scroll_direction = 1;
    static int pause_counter = 0;
    const int pause_frames = 30; // Adjust for desired pause duration
    const char *url = "github.com/richrd/esphome-clock-os";
    int url_width = 200;
    int max_offset = url_width > screen_w ? url_width - screen_w : 0;

    it.print(-scroll_offset, y, id(clockos_font_xs), url);

    if (max_offset > 0) {
        if (pause_counter > 0) {
            pause_counter--;
        } else {
            scroll_offset += scroll_direction;
            if (scroll_offset >= max_offset || scroll_offset <= 0) {
                scroll_direction *= -1;
                pause_counter = pause_frames;
            }
        }
    }
