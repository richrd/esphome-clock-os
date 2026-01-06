|-
    // Config
    int screen_w = it.get_width();
    int screen_h = it.get_height();

    static float accumulated_time = 0.0f;
    static float start_time = 0.0f;
    static bool running = false;

    static const char* options[] = {
        "Start",
        "Stop",
        "Reset",
        "Back"
    };
    static const int options_count = sizeof(options) / sizeof(options[0]);
    static int selected_option = -1;

    if (id(clockos_global_knob_direction) != 0) {
        selected_option += id(clockos_global_knob_direction);
        if( selected_option < 0 ) {
            selected_option = options_count - 1;
        } else if( selected_option >= options_count ) {
            selected_option = 0;
        }

        // Reset knob direction to avoid continuous changes
        id(clockos_global_knob_direction) = 0;
    }

    if (id(clockos_global_knob_pressed)) {
        if (selected_option == 0) {
            // Start
            if (start_time == 0.0f) {
                start_time = millis();
            }
            running = true;
        } else if (selected_option == 1) {
            // Stop
            if (running) {
                accumulated_time += millis() - start_time;
            }
            start_time = 0.0f;
            running = false;
        } else if (selected_option == 2) {
            // Reset
            accumulated_time = 0.0f;
            start_time = 0.0f;
            running = false;
        } else if (selected_option == 3) {
            // Back
            selected_option = -1;
            id(script_back_to_menu)->execute();
            return;
        }
        id(clockos_global_knob_pressed) = false;
    }

    it.fill(COLOR_OFF);
    float now = millis();
    // Print time in format MM:SS.ss
    int total_milliseconds = accumulated_time;
    if (running) {
        total_milliseconds = now - start_time + accumulated_time;
    }
    int minutes = (total_milliseconds / 60000) % 60;
    int seconds = (total_milliseconds / 1000) % 60;
    int milliseconds = total_milliseconds % 1000;
    it.printf(0, 0, id(font_xl), TextAlign::TOP_LEFT, "%02d:%02d.%02d", minutes, seconds, milliseconds / 10);

    for (int i = 0; i < options_count; i++) {
        int x = i * (screen_w/options_count);
        if (i == selected_option) {
            it.rectangle(x, screen_h-18, (screen_w/options_count), 18);
        }
        it.printf(x+1, screen_h-16, id(font_xs), "  %s", options[i]);
    }

