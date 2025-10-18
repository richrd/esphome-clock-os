|-
      if (id(main_menu).is_active()) {
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
      if (id(wifi_rssi).has_state()) {
        float rssi = id(wifi_rssi).state;
        if (rssi <= -100) {
          quality = 0;
        } else if (rssi >= -50) {
          quality = 100;
        } else {
          quality = (int)(2 * (rssi + 100));
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
      
      it.graph(2, 48, id(wifi_graph));

      // Show current time
      it.printf(0, -10, id(font_xxl), "%02d:%02d", id(homeassistant_time).now().hour, id(homeassistant_time).now().minute);

      // Show raw sunrise time
      int small_text_y = 31;
      draw_sun(0, small_text_y+2);
      if (id(sunrise_time).has_state()) {
        std::string sunrise = id(sunrise_time).state.c_str();
        it.printf(14, small_text_y, id(font_sm), "%s", sunrise.c_str());
      } else {
        it.print(14, small_text_y, id(font_sm), "--:--");
      }

      // Show temperature & humidity
      it.printf(it.get_width(), small_text_y, id(font_sm), TextAlign::TOP_RIGHT, "%.1f°C  %.0f%%", id(ha_temperature).state, id(ha_humidity).state);