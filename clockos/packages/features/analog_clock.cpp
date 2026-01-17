|-
    if (id(clockos_menu_main).is_active()) {
      return;
    }

    // Config
    int screen_w = 128;
    int screen_h = 64;

    it.fill(COLOR_OFF);

    // Analog clock parameters
    int clock_radius = 25; // You can adjust this value for clock size
    int center_x = screen_w / 2;
    int center_y = screen_h / 2; // Adjusted to fit below digital time

    // Draw clock face
    it.circle(center_x, center_y, clock_radius, COLOR_ON);

    // Draw hour markers
    for (int h = 0; h < 12; ++h) {
      float angle = (h / 12.0f) * 2 * M_PI - M_PI / 2;
      int x1 = center_x + (int)(cos(angle) * (clock_radius - 2));
      int y1 = center_y + (int)(sin(angle) * (clock_radius - 2));
      int x2 = center_x + (int)(cos(angle) * (clock_radius - 6));
      int y2 = center_y + (int)(sin(angle) * (clock_radius - 6));
      it.line(x1, y1, x2, y2, COLOR_ON);
    }

    // Get current time
    int hour = id(clockos_time_sntp).now().hour % 12;
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

    // Calculate angles for hands
    float minute_angle = (minute / 60.0f) * 2 * M_PI - M_PI / 2;
    float hour_angle = ((hour + minute / 60.0f) / 12.0f) * 2 * M_PI - M_PI / 2;

    // Draw minute hand
    int min_x = center_x + (int)(cos(minute_angle) * (clock_radius - 8));
    int min_y = center_y + (int)(sin(minute_angle) * (clock_radius - 8));
    it.line(center_x, center_y, min_x, min_y, COLOR_ON);

    // Draw hour hand
    int hour_x = center_x + (int)(cos(hour_angle) * (clock_radius - 14));
    int hour_y = center_y + (int)(sin(hour_angle) * (clock_radius - 14));
    it.line(center_x, center_y, hour_x, hour_y, COLOR_ON);

    // Draw center dot
    it.circle(center_x, center_y, 2, COLOR_ON);
