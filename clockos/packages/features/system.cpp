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

    it.printf(0, y, id(clockos_font_xs), "IP: %s", id(clockos_ip_address).state.c_str());
    y += line_height;

    if (id(clockos_sensor_wifi_rssi).has_state())
    {
        float rssi = 0;
        int quality = 0;
        if (id(clockos_sensor_wifi_rssi).has_state()) {
            rssi = id(clockos_sensor_wifi_rssi).state;
            if (rssi <= -100)
            {
                quality = 0;
            }
            else if (rssi >= -50)
            {
                quality = 100;
            }
            else
            {
                quality = 2 * (rssi + 100);
            }
        }
        it.printf(0, y, id(clockos_font_xs), "WiFi: %.0f dBm (%d%%)", rssi, quality);
        y += line_height;
    }

    if (id(clockos_sensor_uptime).has_state())
    {
        uint32_t secs = (uint32_t)id(clockos_sensor_uptime).state;
        int hours = secs / 3600;
        int minutes = (secs % 3600) / 60;
        it.printf(0, y, id(clockos_font_xs), "Uptime: %dh %02dm", hours, minutes);
        y += line_height;
    }

    if (id(clockos_sensor_free_memory).has_state())
    {
        it.printf(0, y, id(clockos_font_xs), "Free Mem: %d B", (int)id(clockos_sensor_free_memory).state);
        y += line_height;
    }

    it.printf(0, y, id(clockos_font_xs), "FW: %s", id(clockos_esphome_version).state.c_str());