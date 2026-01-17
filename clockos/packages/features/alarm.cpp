|-
    // Alarm ringing screen
    int hour = id(clockos_time_sntp).now().hour;
    int minute = id(clockos_time_sntp).now().minute;

    float current_timestamp = id(clockos_current_timestamp).state;

    if (!isnan(current_timestamp)) {
        auto current_time_object = ESPTime::from_epoch_local((uint32_t)current_timestamp);
        hour = current_time_object.hour;
        minute = current_time_object.minute;
    }

    
    it.fill(COLOR_OFF);
    // TODO: Fancy graphics
    it.printf(0, 20, id(clockos_font_md), "Alarm: %02d:%02d", id(clockos_global_feature_alarm_hour), id(clockos_global_feature_alarm_minute));
    it.printf(0, 40, id(clockos_font_md), "Now: %02d:%02d", hour, minute);
    


