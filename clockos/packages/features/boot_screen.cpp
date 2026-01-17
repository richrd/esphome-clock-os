|-
    // Config
    int screen_w = it.get_width();
    int screen_h = it.get_height();

    static int progress = 0;
    progress += 5;


    it.fill(COLOR_OFF);
    int bar_height = 4;
    int y = (screen_h - bar_height) / 2;
    it.rectangle(0, y, screen_w, bar_height);
    it.filled_rectangle(0, y, progress, bar_height);

    if (progress > screen_w) {
        id(clockos_script_back_to_main_page)->execute();
    }
