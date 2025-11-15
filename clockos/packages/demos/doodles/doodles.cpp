|-
    // State
    static int image_frame = 0;
    static int max_frames = 1;

    if (id(clockos_global_knob_direction) != 0) {
        // Choose frame based on knob direction
        image_frame += id(clockos_global_knob_direction);
        // Reset knob direction to avoid continuous changes
        id(clockos_global_knob_direction) = 0;
    }

    // Clamp image_frame within valid range
    if (image_frame < 0) {
        image_frame = 0;
    } else if (image_frame > max_frames-1) {
        image_frame = max_frames-1;
    }

    it.fill(COLOR_OFF);

    if (image_frame == 0) {
        it.image(0, 0, id(image_doodle_01));
    }
