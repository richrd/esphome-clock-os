|-
    // Alarm ringing screen
    it.fill(COLOR_OFF);
    // TODO: Fancy graphics
    it.printf(0, 20, id(font_md), "Alarm: %02d:%02d", id(global_feature_alarm_hour), id(global_feature_alarm_minute));
    it.printf(0, 40, id(font_md), "Now: %02d:%02d", id(clockos_main_time).now().hour, id(clockos_main_time).now().minute);
    
