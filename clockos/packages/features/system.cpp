|-
    if (id(main_menu).is_active())
    {
        return;
    }

    it.fill(COLOR_OFF);

    // Config
    int screen_w = 128;
    int screen_h = 64;

    int line_height = 10;
    int y = 0;

    it.printf(0, y, id(font_xs), "IP: %s", id(ip_address).state.c_str());
    y += line_height;

    if (id(wifi_rssi).has_state())
    {
        float rssi = 0;
        int quality = 0;
        if (id(wifi_rssi).has_state()) {
            rssi = id(wifi_rssi).state;
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
        it.printf(0, y, id(font_xs), "WiFi: %.0f dBm (%d%%)", rssi, quality);
        y += line_height;
    }

    if (id(uptime_sensor).has_state())
    {
        uint32_t secs = (uint32_t)id(uptime_sensor).state;
        int hours = secs / 3600;
        int minutes = (secs % 3600) / 60;
        it.printf(0, y, id(font_xs), "Uptime: %dh %02dm", hours, minutes);
        y += line_height;
    }

    if (id(free_memory).has_state())
    {
        it.printf(0, y, id(font_xs), "Free Mem: %d B", (int)id(free_memory).state);
        y += line_height;
    }

    it.printf(0, y, id(font_xs), "FW: %s", id(esphome_version).state.c_str());