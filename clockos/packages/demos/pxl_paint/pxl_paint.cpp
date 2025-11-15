|-
    // Config
    const int screen_w = 128;
    const int screen_h = 64;

    static int frame = 0;
    static float x = 0;
    static float y = 0;

    float speed_x = 0.1;
    float speed_y = 0.1;

    static bool pixels[screen_w * screen_h] = {};
    
    auto draw_pixels = [&]()
    {
        for (int idx = 0; idx < screen_w * screen_h-2; ++idx) {
            if (pixels[idx]) {
                int px = idx % screen_w;
                int py = idx / screen_w;
                it.draw_pixel_at(px, py, COLOR_ON);
            }
        }
    };

    auto draw_cursor = [&]()
    {
        // Left edge indicator
        it.line(0, y, 1, y, COLOR_ON);
        // Right edge indicator
        it.line(screen_w-1, y, screen_w, y, COLOR_ON);

        // Top edge indicator
        it.line(x, 0, x, 1, COLOR_ON);
        // Bottom edge indicator
        it.line(x, screen_h - 1, x, screen_h, COLOR_ON);

        // Variables for crosshair offset and length
        const int crosshair_offset = 3;
        const int crosshair_length = 2;

        // Draw orthogonal crosshair lines, using variables for offset and length
        // Up
        it.line(x, y - crosshair_offset, x, y - (crosshair_offset + crosshair_length), COLOR_ON);
        // Down
        it.line(x, y + crosshair_offset, x, y + (crosshair_offset + crosshair_length), COLOR_ON);
        // Left
        it.line(x - crosshair_offset, y, x - (crosshair_offset + crosshair_length), y, COLOR_ON);
        // Right
        it.line(x + crosshair_offset, y, x + (crosshair_offset + crosshair_length), y, COLOR_ON);
    };

    auto apply_exponential_curve = [](float x) -> float {
        float exponent = 4.0; 
        float sign = (x < 0.0 ? -1.0 : 1.0);
        float curve_magnitude = std::pow(std::fabs(x), exponent);
        return sign * curve_magnitude;
    };

    // Joystick support
    if (!id(clockos_joystick_x).is_failed()) {
        float max_speed = 4.5f; // Adjust for desired curve (higher = more exponential)
        if (id(clockos_global_button_a_pressed) || id(clockos_global_button_b_pressed)) {
            max_speed = 1.0f;
        }
        float joystick_pos_x = apply_exponential_curve(id(clockos_joystick_x).state);
        float joystick_pos_y = apply_exponential_curve(id(clockos_joystick_y).state);
    
        speed_y = joystick_pos_y * max_speed * -1;
        speed_x = joystick_pos_x * max_speed * -1;
    }

    x = x + speed_x;
    y = y + speed_y;

    
    int xi = static_cast<int>(x);
    int yi = static_cast<int>(y);

    if (id(clockos_global_knob_pressed)) {
        pixels[yi * screen_w + xi] = !pixels[yi * screen_w + xi];
        id(clockos_global_knob_pressed) = false;
    }

    if (id(clockos_global_button_a_pressed)) {
        pixels[yi * screen_w + xi] = 1;
    }
    if (id(clockos_global_button_b_pressed)) {
        pixels[yi * screen_w + xi] = 0;
    }

    if (x < 0) {
        x = 0;
    }
    if (y < 0) {
        y = 0;
    }
    if (x > screen_w) {
        x = screen_w-1;
    }
    if (y > screen_h) {
        y = screen_h-1;
    }

    draw_pixels();
    draw_cursor();
    
