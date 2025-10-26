|-
    // Config
    int screen_w = it.get_width();
    int screen_h = it.get_height();

    // State
    static int frame = 0;
    static int current_index = -1;

    struct FloorplanItem {
        int x;
        int y;
        std::string entity_name;
    };

    static FloorplanItem floor_plan_items[] = {
        {48, 6, ""}, // Index 0 reserved for back button
        // KITCHEN
        // Kitchen led strip
        {8, 8, "light.new_esp32_1_esp_32_1_led_strip"},
        // Kitchen vibe light
        {6, 42, "light.sonoff_socket_6_kitchen_socket_6_relay"},
        // Dining room ceiling light
        {15, 52, "light.sonoff_mini_2_sonoff_mini_2_relay"},

        // LIVINGROOM
        // TV backlight
        {43, 35, "light.esp_32_n3_esp_32_m3_led_strip"},
        // Main light
        {56, 47, "light.sonoff_mini_1_sonoff_mini_1_relay"},
        // Monitor backlight
        {85, 53, "light.espnode2_monitor_led_strip"},

        // BEDROOM
        // Bedroom leds
        {94, 36, "light.sonoff_basic_1_bedroom_sonoff_basic_1_relay"},
        // Bedroom ceiling light
        {111, 33, "light.sonoff_basic_3_bedroom_ceiling_lamp_sonoff_basic_3_relay"},
        // Bed light
        {120, 43, "light.esp32_3_new_led_strip"},

        // BATHROOM
        {50, 22, "light.esp32_01_short_led_strip"},
    };

    if (id(global_knob_direction) != 0) {
        // Speed up/down animation based on knob turn
        current_index += id(global_knob_direction);
        // Reset knob direction to avoid continuous changes
        id(global_knob_direction) = 0;
        if (current_index < 0) {
            current_index = 0;
        } else if (current_index >= sizeof(floor_plan_items)/sizeof(FloorplanItem)) {
            current_index = sizeof(floor_plan_items)/sizeof(FloorplanItem) - 1;
        }
    }

    if (id(global_knob_pressed)) {
        // Activate entity at current index
        if (current_index == 0) {
            // Back button pressed
            current_index = -1;
            id(script_back_to_menu)->execute();
            return;
        } else if (current_index > 0) {
            std::string entity_name = floor_plan_items[current_index].entity_name;
            if (entity_name != "") {
                ESP_LOGW("floorplan", "Toggling entity: %s", entity_name.c_str());
                id(script_toggle_ha_entity)->execute(entity_name);
            }
        }
        id(global_knob_pressed) = false;
    }


    it.fill(COLOR_OFF);
    it.image(0, 0, id(image_floorplan));

    for (int i = 0; i < sizeof(floor_plan_items)/sizeof(FloorplanItem); i++) {
        if (i == 0) {
            // Draw back button
            int x = floor_plan_items[i].x - 5;
            int y = floor_plan_items[i].y - 4;

            it.line(x + 2, y + 4, x + 8, y + 4);
            it.line(x + 2, y + 4, x + 4, y + 2);
            it.line(x + 2, y + 4, x + 4, y + 6);
            if (i == current_index) {
                it.rectangle(x, y, 11, 9);
            }
            continue;
        }
        else if (i == current_index) {
            // Highlight current item
            it.filled_circle(floor_plan_items[i].x, floor_plan_items[i].y, 3);
        } else {
            it.circle(floor_plan_items[i].x, floor_plan_items[i].y, 2);
        }
    }

