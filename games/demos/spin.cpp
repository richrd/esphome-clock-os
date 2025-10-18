|-
    static int frame = 0;
    float speed = -0.2;
    frame++;
    int x = 64 + int(20 * sin(frame * speed));
    int y = 32 + int(20 * cos(frame * speed));
    it.fill(COLOR_OFF);
    it.filled_circle(x, y, 5);
