|-
    static unsigned long lastTime = 0;
    static int frameCount = 0;
    static int fps = 0;

    frameCount++;
    unsigned long now = millis();
    if (now - lastTime >= 1000) {
        fps = frameCount;
        frameCount = 0;
        lastTime = now;
    }

    id(animation_bunnywiggle_02).next_frame();
    it.fill(COLOR_OFF);
    it.image(0, 0, id(animation_bunnywiggle_02), COLOR_ON, COLOR_OFF);

    // Draw FPS counter at top-left corner
    it.printf(0, 0, id(font_xs), COLOR_ON, "FPS: %d", fps);
