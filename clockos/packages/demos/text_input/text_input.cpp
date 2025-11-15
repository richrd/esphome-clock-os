|-
    // Config
    int screen_w = it.get_width();
    int screen_h = it.get_height();

    static char input_text[] = "";
    const int max_input_length = 10;

    const char available_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    const int available_chars_count = sizeof(available_chars);
    static int current_char_index = 0;

    if(id(clockos_global_knob_pressed)) {
        // Activate the main menu
        id(main_menu).show();
        id(clockos_global_knob_pressed) = false;
        return;
    }

    if (id(clockos_global_knob_direction) != 0) {
        current_char_index += id(clockos_global_knob_direction);
        if( current_char_index < 0 ) {
            current_char_index = available_chars_count - 1;
        } else if( current_char_index >= available_chars_count ) {
            current_char_index = 0;
        }

        // Reset knob direction to avoid continuous changes
        id(clockos_global_knob_direction) = 0;
    }


    it.fill(COLOR_OFF);
    it.printf(0, 0, id(font_xxs), TextAlign::TOP_LEFT, "INDEX: %d", current_char_index);
    it.printf(0, 10, id(font_xxs), TextAlign::TOP_LEFT, "CHAR: %c", available_chars[current_char_index]);

    int field_x = 10;
    int field_y = 40;
    int field_w = screen_w - 20;
    int field_h = 18;
    int field_text_offset_x = 4;
    int character_width = 12;

    it.rectangle(field_x, field_y, field_w, field_h);
    it.printf(field_x + field_text_offset_x, field_y, id(font_sm), "%c", available_chars[current_char_index]);

