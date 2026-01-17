|-
      if (id(clockos_menu_main).is_active()) {
        return;
      }

      // Config
      int screen_w = 128;
      int screen_h = 64;

      it.fill(COLOR_OFF);

      // Functions
      auto draw_sun = [&](int x, int y) {
        int size = 9;
        int half = 5;
        it.filled_circle(x+half,y+half, 4);
        it.line(x+1,y+1,x+size,y+size);
        it.line(x+size,y+1,x+1,y+size);
      };

      // // Optional: convert dBm to percentage
      int quality = 0;
      if (id(clockos_sensor_wifi_rssi).has_state()) {
        float rssi = id(clockos_sensor_wifi_rssi).state;
        if (rssi <= -100) {
          quality = 0;
        } else if (rssi >= -50) {
          quality = 100;
        } else {
          quality = (int)(2 * (rssi + 100));
        }
      }
      
      if(id(clockos_wifi).is_disabled()) {
        quality = 0;
          // Show X icon when wifi is disabled
        int x_start = 106;
        int y_start = 11;
        int x_end = x_start + 7;
        int y_top = y_start - 7;
        int y_bottom = y_start;

        // Draw X from top-left to bottom-right
        it.line(x_start, y_top, x_end, y_bottom);
        for (int offset = -1; offset <= 1; ++offset) {
          it.line(x_start + offset, y_top, x_end + offset, y_bottom);
        }
        // Draw X from top-right to bottom-left with thickness 3
        for (int offset = -1; offset <= 1; ++offset) {
          it.line(x_end + offset, y_top, x_start + offset, y_bottom);
        }
      }


      int bar_x_start = 106;
      int bar_y_start = 28;
      int bar_width = 4;
      // Optional: draw bars (like signal bars)
      for (int i = 0; i < 4; i++) {
        int bar_height = (i + 1) * 6;
        int bar_x = bar_x_start + i * 6;
        int bar_y = bar_y_start - bar_height;
      
        if (quality > i * 25) {
          it.filled_rectangle(bar_x, bar_y, bar_width, bar_height);
        } else {
          it.rectangle(bar_x, bar_y, bar_width, bar_height);
        }
      }

      it.graph(0, 48, id(wifi_graph));

      // Show current time
      int hour = id(clockos_time_sntp).now().hour;
      int minute = id(clockos_time_sntp).now().minute;

      float current_timestamp = id(clockos_current_timestamp).state;
      // Check if the timestamp is valid (not NAN)
      if (!isnan(current_timestamp)) {
        // 2. Convert the timestamp (float) back to a native ESPTime object
        // The method takes an 'epoch_second' (a 32-bit integer).
        auto current_time_object = ESPTime::from_epoch_local((uint32_t)current_timestamp);
        hour = current_time_object.hour;
        minute = current_time_object.minute;
      }
      it.printf(0, -10, id(clockos_font_xxl), "%02d:%02d", hour, minute);

      // Show raw sunrise time
      int small_text_y = 31;
      draw_sun(0, small_text_y+2);
      if (id(clockos_sensor_sunrise_time).has_state()) {
        std::string sunrise = id(clockos_sensor_sunrise_time).state.c_str();
        it.printf(14, small_text_y, id(clockos_font_sm), "%s", sunrise.c_str());
      } else {
        it.print(14, small_text_y, id(clockos_font_sm), "--:--");
      }

      // Show temperature & humidity
      it.printf(it.get_width(), small_text_y, id(clockos_font_sm), TextAlign::TOP_RIGHT, "%.1f°C  %.0f%%", id(clockos_ha_temperature).state, id(clockos_ha_humidity).state);