|-
    static int frame = 0;
    static int frameCount = 0;
    static unsigned long lastTime = 0;
    static int fps = 0;

    frame++;
    frameCount++;
    unsigned long now = millis();
    if (now - lastTime >= 1000) {
        fps = frameCount;
        frameCount = 0;
        lastTime = now;
    }
    float speed = -0.2;
    
    int x = 64 + int(20 * sin(frame * speed));
    int y = 32 + int(20 * cos(frame * speed));
    it.fill(COLOR_OFF);
    it.filled_circle(x, y, 5);

    // Draw FPS counter at top-left corner
    it.printf(0, 0, id(clockos_font_xs), COLOR_ON, "FPS: %d", fps);
