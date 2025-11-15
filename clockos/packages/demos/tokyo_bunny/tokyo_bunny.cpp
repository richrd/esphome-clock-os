|-
    // Config
    int screen_w = it.get_width();
    int screen_h = it.get_height();

    // State
    static int frame = 0;
    static int max_frame = 100;
    static int image_frame = 1;
    static int frames_per_update = 10;

    // Track elapsed time to detect when app has been paused
    static uint32_t last_ms = 0;
    if (last_ms == 0) {
        last_ms = millis();
    }
    uint32_t current_ms = millis();
    uint32_t elapsed_ms = current_ms - last_ms;
    last_ms = current_ms;

    // App has been paused for a while, "restart"
    if (elapsed_ms > 100) {
        frame = 0;
    }

    if (id(clockos_global_knob_direction) != 0) {
        // Speed up/down animation based on knob turn
        frames_per_update += id(clockos_global_knob_direction);
        // Reset knob direction to avoid continuous changes
        id(clockos_global_knob_direction) = 0;
    }

    frame++;
    if (frame % frames_per_update == 0) {
        image_frame++;
        if (image_frame > 4) {
            image_frame = 1;
        }
    }

    it.fill(COLOR_OFF);
    
    if (image_frame == 1) {
        it.image(0, 0, id(image_bunnygirl_01));
    } else if (image_frame == 2) {
        it.image(0, 0, id(image_bunnygirl_02));
    } else if (image_frame == 3) {
        it.image(0, 0, id(image_bunnygirl_03));
    } else if (image_frame == 4) {
        it.image(0, 0, id(image_bunnygirl_04));
    }
