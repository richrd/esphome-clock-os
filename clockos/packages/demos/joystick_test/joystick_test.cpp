|-
    // Config
    int screen_w = 128;
    int screen_h = 64;

    float x_min = 0.14f;
    float x_center = 1.66f;
    float x_max = 3.14f;

    float y_min = 0.14f;
    float y_center = 1.70f;
    float y_max = 3.14f;

    it.print(0, 0, clockos_font_sm , COLOR_ON, "Calibrate joystick:");
    float x_filtered_value = id(clockos_joystick_x).state;
    float y_filtered_value = id(clockos_joystick_y).state;

    auto apply_exponential_curve = [](float x) -> float {
        float exponent = 4.0; 
        float sign = (x < 0.0 ? -1.0 : 1.0);
        float curve_magnitude = std::pow(std::fabs(x), exponent);
        
        return sign * curve_magnitude;
    };

    float curve_x = apply_exponential_curve(x_filtered_value);
    float curve_y = apply_exponential_curve(y_filtered_value);

    it.printf(0, 20, id(clockos_font_xxs), "Raw X: %.2f Raw Y: %.2f", id(clockos_joystick_x).raw_state, id(clockos_joystick_y).raw_state);
    it.printf(0, 30, id(clockos_font_xxs), "F X: %.2f F Y: %.2f", x_filtered_value, y_filtered_value);
    it.printf(0, 40, id(clockos_font_xxs), "C X: %.2f C Y: %.2f", curve_x, curve_y);
