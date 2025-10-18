|-
    // Config
    int screen_w = 128;
    int screen_h = 64;

    static int frame = 0;
    static int x = 0;
    static int y = 0;
    static int direction_x = 1;
    static int direction_y = 1;
    int size = 10;

    x = x + direction_x;
    y = y + direction_y;

    if (x < 0) {
    direction_x = 1;
    }
    if (y < 0) {
    direction_y = 1;
    }
    if (x+size > screen_w) {
    direction_x = -1;
    }
    if (y+size > screen_h) {
    direction_y = -1;
    }

    it.fill(COLOR_OFF);
    it.filled_rectangle(x, y, size, size);
