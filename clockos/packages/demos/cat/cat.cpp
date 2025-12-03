|-
    // Config
    int screen_w = it.get_width();
    int screen_h = it.get_height();
    int max_level_height = 64;
    int floor_position_y = 14;

    float ball_size = 8.0f;
    float ball_radius = ball_size / 2.0f;
    float ball_friction_x = 0.999f;
    float ball_friction_y = 0.999f;
    float ball_gravity = 0.12f;
    float wall_speed_factor = 0.8f; // Increase this for more bounce

    // Structures
    struct Ball
    {
        float x;
        float y;
        float speed_x;
        float speed_y;
        bool alive;
    };

    // State
    static int frame = 0;
    static int image_frame = 0;
    static int max_frames = 1;
    static int sitting_frames = 0;
    static int sleeping_frames = 0;

    static int ball_frame = 0;
    static int max_ball_frame = 3;

    static float cat_x = 0;

    static Ball ball = {
        64,
        32,
        0.5,
        0,
        true
    };


    auto update_ball = [&]()
    {
        // Gravity and friction
        ball.speed_y -= ball_gravity;
        ball.speed_x *= ball_friction_x;
        ball.speed_y *= ball_friction_y;
        
        // Update position
        ball.x += ball.speed_x;
        ball.y += ball.speed_y;

        // Wall collisions
        bool collided_with_wall = false;
        if (ball.x < 0)
        { // Left
            ball.x = 0;
            ball.speed_x = ball.speed_x * -1;
            collided_with_wall = true;
        }
        if (ball.x + ball_size > screen_w)
        { // Right
            ball.x = screen_w - ball_size;
            ball.speed_x = ball.speed_x * -1;
            collided_with_wall = true;
        }
        if (ball.y < floor_position_y)
        { // Bottom
            ball.y = floor_position_y;
            ball.speed_y = ball.speed_y * -0.8;
            collided_with_wall = true;
        }
        if (ball.y + ball_size > max_level_height)
        { // Top
            ball.y = max_level_height - ball_size;
            ball.speed_y = ball.speed_y * -1;
            collided_with_wall = true;
        }

        if (collided_with_wall) {
            ball.speed_x *= wall_speed_factor;
            ball.speed_y *= wall_speed_factor;
        }
    };

    if (id(clockos_global_knob_direction) != 0) {
        // Choose frame based on knob direction
        image_frame += id(clockos_global_knob_direction);
        // Reset knob direction to avoid continuous changes
        id(clockos_global_knob_direction) = 0;
    }

    // Clamp image_frame within valid range
    if (image_frame < 0) {
        image_frame = 0;
    } else if (image_frame > max_frames-1) {
        image_frame = max_frames-1;
    }

    // Update state
    update_ball();


    int padding = 7;
    int cat_center_x = cat_x + id(image_cat_sitting_01).get_width() / 2;
    bool ball_found = false;
    int cat_direction = 0;
    
    if (!sleeping_frames) {
        if (cat_center_x < ball.x+ball_radius-padding) {
            cat_direction = 1;
            cat_x += 0.5;
        } else if (cat_center_x > ball.x+ball_radius+padding) {
            cat_direction = -1;
            cat_x -= 0.5;
        } else if (ball.y < floor_position_y+1) {
            if (sitting_frames == 10) {
                int max_speed = 6;
                ball.speed_x = (static_cast<float>(rand()) / RAND_MAX * max_speed) - (max_speed / 2);
                ball.speed_y = (static_cast<float>(rand()) / RAND_MAX * max_speed);
                if ((static_cast<float>(rand()) / RAND_MAX) < 0.10f) {
                    // 10% chance to go to sleep after playing
                    sleeping_frames = 820;
                }
            } else if (!sitting_frames) {
                sitting_frames = 60; // Stay sitting for a few frames
            }
        }
    }


    // Render
    it.fill(COLOR_OFF);
    it.start_clipping(0, 0, screen_w, screen_h); // Work around SDL scaling bug when drawing outside bounds;

    // Draw floor
    it.line(0, screen_h - floor_position_y, screen_w, screen_h - floor_position_y);

    // Draw cat tail animation    
    if (frame % 9 == 0) {
        id(animation_cat_tail).next_frame();
    }

    if (sleeping_frames) {
        if (sleeping_frames % 50 == 0) {
            id(animation_cat_sleeping).next_frame();
        }
        it.image(cat_x, screen_h - id(animation_cat_sleeping).get_height() - floor_position_y, id(animation_cat_sleeping));
        sleeping_frames--;
    } else {
        if (sitting_frames) {
            it.image(cat_x, screen_h - id(image_cat_sitting_01).get_height() - floor_position_y, id(image_cat_sitting_01));
            it.image(cat_x+id(image_cat_sitting_01).get_width(), screen_h - id(animation_cat_tail).get_height() - floor_position_y, id(animation_cat_tail));
        } else {
            if (cat_direction == -1) {
                if (frame % 4 == 0) {
                    id(animation_cat_walk_left).next_frame();
                }
                it.image(cat_x, screen_h - id(animation_cat_walk_left).get_height() - floor_position_y, id(animation_cat_walk_left));
            } else {
                if (frame % 4 == 0) {
                    id(animation_cat_walk_right).next_frame();
                }
                it.image(cat_x, screen_h - id(animation_cat_walk_right).get_height() - floor_position_y, id(animation_cat_walk_right));
            }
        }
    }
    
    // Define ball speed x as absolute, to always keep it positive
    float abs_speed_x = std::abs(ball.speed_x);
    if (abs_speed_x > 0.1 && frame % (5  - int(abs_speed_x)) == 0) {
        if (ball.speed_x > 0) {
            id(animation_cat_ball).prev_frame();
        } else {
            id(animation_cat_ball).next_frame();
        }
    }

    int ball_screen_y = int(screen_h - ball.y - ball_size);
    it.image(ball.x, ball_screen_y, id(animation_cat_ball));

    if (sitting_frames) {
        sitting_frames--;
    }

    frame++;