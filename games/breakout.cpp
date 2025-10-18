|-
    // Config
    int screen_w = it.get_width();
    int screen_h = it.get_height();
    int ball_size = 3;
    int paddle_w = 22;
    int paddle_h = 3;
    int brick_w = 15;
    int brick_h = 7;
    int brick_max_hp = 5;
    int knob_max = 50;
    int rumble_duration = 4;
    int pause_duration = 100; // 100 frames, at 30fps = ~3 seconds
    int points_per_brick = 5;
    int points_per_paddle_hit = 10;
    bool auto_play = true;
    // int frames_per_update = 1;

    // Sounds
    // blip: Untitled:d=32,o=6,b=200:32e6,g
    std::string sound_brick_hit = "Untitled:d=32,o=7,b=250:f,a";
    std::string sound_paddle_hit = "Untitled:d=32,o=7,b=250:f#";
    std::string sound_ball_lost = "Untitled:d=32,o=7,b=250:a,p,a#,p,p,p,f#,p,p,p,p,c";

    struct Ball {
        int x;
        int y;
        int direction_x;
        int direction_y;
        bool alive;
    };

    // State
    static int frame = 0;
    static int pause_frames = pause_duration;
    static int rumble_frames = 0;
    static int max_lives = 5;
    static int lives = 5;
    static int score = 0;
    static int score_ticker = 0;
    static int level = 0;
    static int shield_amount = 0;
    static const int max_balls = 10;
    static Ball balls[max_balls] = {
        {0, 0, true},
        {0, 0, false},
        {0, 0, false},
        {0, 0, false},
        {0, 0, false},
        {0, 0, false},
        {0, 0, false},
        {0, 0, false},
        {0, 0, false},
        {0, 0, false},
    };

    // Consume frames for throttling
    // if (frame < frames_per_update)
    // {
    //     frame++;
    //     return;
    // } else {
    //     frame = 0;
    // }

    // Calculate pad position based on knob state
    int paddle_x = id(knob).state * ((float)(screen_w - paddle_w) / knob_max);
    int paddle_y = screen_h - paddle_h;
    bool paddle_hit = false;

    // Autoplay
    if (id(global_game_autoplay))
    {
        int ball_position_x = 64; // Middle by default
        for (int b = 0; b < max_balls; b++) {
            if (balls[b].alive) {
                ball_position_x = balls[b].x;
                break;
            }
        }

        paddle_x = ball_position_x - (paddle_w / 2);
        if (paddle_x < 0)
        {
            paddle_x = 0;
        }
        else if (paddle_x + paddle_w > screen_w)
        {
            paddle_x = screen_w - paddle_w;
        }
    }

    int BRICK_TYPE_NORMAL = 0;
    int BRICK_TYPE_SHIELD = 1;
    int BRICK_TYPE_EXTRA_BALL = 2;
    

    struct Brick
    {
        int x;
        int y;
        int hp;
        int type;
        // Type defines the behavior of the brick
        // 0 = normal
        // TODO:
        // ? = upwards only gate
        // ? = inverter brick (flip screen for x amount of time or until the next level)
        // ? = extra ball
        // ? = extra life
        // ? = explode adjacent bricks

        // Defined with < 0 HP:
        // ? = unbreakable

    };

    ////////////////////////////////
    // Bricks
    ////////////////////////////////
    int brick_count = 48;
    static Brick bricks[48] = {
        // 1
        {0, 0, 0, 0},
        {16, 0, 0, 0},
        {32, 0, 0, 0},
        {48, 0, 0, 0},
        {64, 0, 0, 0},
        {80, 0, 0, 0},
        {96, 0, 0, 0}, 
        {112, 0, 0, 0},
        // 2
        {0, 8, 0, 0},
        {16, 8, 0, 0},
        {32, 8, 0, 0},
        {48, 8, 0, 0},
        {64, 8, 0, 0},
        {80, 8, 0, 0},
        {96, 8, 0, 0},
        {112, 8, 0, 0},
        // 3
        {0, 16, 0, 0},
        {16, 16, 0, 0},
        {32, 16, 0, 0},
        {48, 16, 0, 0},
        {64, 16, 0, 0},
        {80, 16, 0, 0},
        {96, 16, 0, 0},
        {112, 16, 0, 0},
        // 4
        {0, 24, 0, 0},
        {16, 24, 0, 0},
        {32, 24, 0, 0},
        {48, 24, 0, 0},
        {64, 24, 0, 0},
        {80, 24, 0, 0},
        {96, 24, 0, 0},
        {112, 24, 0, 0},
        // 5
        {0, 32, 0, 0},
        {16, 32, 0, 0},
        {32, 32, 0, 0},
        {48, 32, 0, 0},
        {64, 32, 0, 0},
        {80, 32, 0, 0},
        {96, 32, 0, 0},
        {112, 32, 0, 0},
        // 6
        {0, 40, 0, 0},
        {16, 40, 0, 0},
        {32, 40, 0, 0},
        {48, 40, 0, 0},
        {64, 40, 0, 0},
        {80, 40, 0, 0},
        {96, 40, 0, 0},
        {112, 40, 0, 0},
    };


    ////////////////////////////////
    // Functions
    ////////////////////////////////
    auto clear_bricks = [&]()
    {
        for (int i = 0; i < brick_count; i++)
        {
            bricks[i].hp = 0;
            bricks[i].type = BRICK_TYPE_NORMAL;
        }
    };

    auto reset_game = [&]()
    {
        score = 0;
        score_ticker = 0;
        lives = 5;
        level = 0; // 0 Triggers level 1 setup
        pause_frames = pause_duration;
        for (int i = 8; i < brick_count; i++)
        {
            bricks[i].hp = 4;
        }
    };

    auto is_level_cleared = [&]() -> bool
    {
        for (int i = 0; i < brick_count; i++)
        {
            Brick brick = bricks[i];
            if (brick.hp > 0)
            {
                return false;
            }
        }
        return true;
    };

    auto place_ball_on_paddle = [&]()
    {
        balls[0].alive = true;
        balls[0].x = paddle_x + (paddle_w / 2);
        balls[0].y = screen_h - paddle_h - ball_size;
    };

    auto setup_next_level = [&]()
    {
        level++;
        lives = 5;
        // Ball needs to be moving up to avoid giving points for the paddle hit as soon as the game starts
        balls[0].direction_y = -1;
        pause_frames = pause_duration;
        int brick_hp = level;
        if (brick_hp > brick_max_hp)
        {
            brick_hp = brick_max_hp;
        }
        clear_bricks();
        int start_brick = 8;
        int end_brick = brick_count - 16;

        // Add new row at level 2
        if (level > 1)
        {
            end_brick = brick_count - 8;
        }

        // Add new row at level 6
        if (level > 5) {
            end_brick = brick_count;
        }

        for (int i = start_brick; i < end_brick; i++)
        {
            bricks[i].hp = brick_hp;
        }

        // Place random bricks according to level number
        int assigned = 0;
        int max_assigned = level;
        if (max_assigned > 40) {
            max_assigned = 40;
        }
        while (assigned < max_assigned) {
            int idx = 8 + (esp_random() % (end_brick - 8));
            if (bricks[idx].hp > 0 && bricks[idx].type == BRICK_TYPE_NORMAL) {
                // Randomly choose between shield or extra ball
                int rtype = (esp_random() % 2) ? BRICK_TYPE_SHIELD : BRICK_TYPE_EXTRA_BALL;
                bricks[idx].type = rtype;
                assigned++;
            }
        }
    };

    auto add_new_ball = [&]() {
        for (int i = 0; i < max_balls; i++) {
            if (!balls[i].alive) {
                balls[i].alive = true;
                balls[i].x = paddle_x + (paddle_w / 2);
                balls[i].y = 0; // Place new ball at the top of the screen
                balls[i].direction_x = (i % 2 == 0) ? 1 : -1; // Alternate direction for variety
                balls[i].direction_y = 1; // Start moving downwards
                break; // Only add one ball at a time
            }
        }
    };

    auto play_sound = [&](std::string sound) {
        if (id(global_game_sound)) {
            id(rtttl_player).stop();
            id(rtttl_player).play(sound);
        }
    };

    auto rumble = [&]() {
        if (id(global_game_rumble)) {
            id(rumble_output).turn_on();
            rumble_frames = rumble_duration;
        }
    };

    auto on_brick_hit = [&](int id)
    {

        if (bricks[id].hp > 0)
        {
            bricks[id].hp--;
            score = score + points_per_brick;
            rumble();

            play_sound(sound_brick_hit);

            if (bricks[id].type == BRICK_TYPE_EXTRA_BALL) {
                bricks[id].hp = 0;
                add_new_ball();
            }

            if (bricks[id].type == BRICK_TYPE_SHIELD) {
                bricks[id].hp = 0;
                shield_amount++;
            }
        }
    };

    auto any_balls_alive = [&]() -> bool {
        for (int i = 0; i < max_balls; i++) {
            if (balls[i].alive) {
                return true;
            }
        }
        return false;
    };

    ////////////////////////////////
    // Game logic
    ////////////////////////////////
    if (level == 0)
    {
        // Setup level 1
        setup_next_level();
    }

    // Check if level cleared
    bool level_cleared = is_level_cleared();
    if (level_cleared)
    {
        ESP_LOGW("game", "level cleared");
        setup_next_level();
    }

    // Animate score ticker
    if (score_ticker < score) {
        score_ticker++;
    }

    // Handle rumble feedback
    if (rumble_frames > 0) {
        rumble_frames--;
        if (rumble_frames == 0) {
            id(rumble_output).turn_off();
        }
    }

    // Handle pause logic (used when game is about to start or has ended)
    if (pause_frames)
    {
        // Consume a frame
        pause_frames--;

        // Keep ball on paddle
        place_ball_on_paddle();

        // If all frames consumed, check for game over
        if (!pause_frames && lives == 0)
        {
            reset_game();
        }
    }
    else
    {
        // Game loop logic
        
        // Iterate over all balls and handle movement and collisions
        for (int b = 0; b < max_balls; b++) {
            Ball& ball = balls[b];
            if (!ball.alive) continue;

            ball.x += ball.direction_x;
            ball.y += ball.direction_y;

            // Wall collisions
            if (ball.x < 0)
            { // Left
                ball.direction_x = 1;
            }
            if (ball.y < 0)
            { // Top
                ball.direction_y = 1;
            }
            if (ball.x + ball_size > screen_w)
            { // Right
                ball.direction_x = -1;
            }

            // Bottom (missed paddle)
            if (ball.y + ball_size > screen_h)
            {
                if (shield_amount) {
                    ball.direction_y = -1;
                    shield_amount--;
                } else {
                    ball.alive = false;

                    play_sound(sound_ball_lost);

                    if(!any_balls_alive()) {
                        lives--;
                        place_ball_on_paddle();
                        ball.direction_y = -1;
                        pause_frames = pause_duration;
                    }
                }
            }

            // Brick collisions
            for (int i = 0; i < brick_count; i++)
            {
                Brick& brick = bricks[i];

                if (brick.hp == 0)
                {
                    continue;
                }

                // Horizontal collisions
                if (
                    ball.y >= brick.y && ball.y + ball_size <= brick.y + brick_h)
                {
                    // Collision on left edge (ball moving right)
                    if (ball.direction_x == 1)
                    {
                        if (ball.x + ball_size >= brick.x && ball.x + ball_size <= brick.x + brick_w)
                        {
                            ball.direction_x = -1;
                            on_brick_hit(i);
                        }
                    }
                    // Collision on right edge (ball moving left)
                    else if (ball.direction_x == -1)
                    {
                        if (ball.x <= brick.x + brick_w && ball.x >= brick.x)
                        {
                            ball.direction_x = 1;
                            on_brick_hit(i);
                        }
                    }
                }
                // Vertical collisions
                if (
                    ball.x >= brick.x && ball.x + ball_size <= brick.x + brick_w)
                {
                    // Collision on top edge (ball moving down)
                    if (ball.direction_y == 1)
                    {
                        if (ball.y + ball_size >= brick.y && ball.y + ball_size <= brick.y + brick_h)
                        {
                            ball.direction_y = -1;
                            on_brick_hit(i);
                        }
                    }
                    // Collision on bottom edge (ball moving up)
                    else if (ball.direction_y == -1)
                    {
                        if (ball.y <= brick.y + brick_h && ball.y >= brick.y)
                        {
                            ball.direction_y = 1;
                            on_brick_hit(i);
                        }
                    }
                }
            }

            // Check if ball hits paddle
            if (
                ball.x >= paddle_x
                && ball.x <= paddle_x + paddle_w
                && ball.y + ball_size > screen_h - paddle_h
                && ball.direction_y == 1
            )
            {
                ball.direction_y = -1;
                score = score + points_per_paddle_hit;
                paddle_hit = true;

                play_sound(sound_paddle_hit);

                if (ball.x + (ball_size / 2) > paddle_x + (paddle_w / 2))
                {
                    ball.direction_x = 1;
                }
                else
                {
                    ball.direction_x = -1;
                }
            }
        }
    }

    ////////////////////////////////
    // Rendering
    ////////////////////////////////

    auto draw_heart = [&](int x, int y)
    {
        it.line(x + 1, y, x + 2, y);
        it.line(x + 4, y, x + 5, y);              //  ## ## I love ESPHome!
        it.filled_rectangle(x, y + 1, 7, 2);      // #######
                                                  // #######
        it.line(x + 1, y + 3, x + 5, y + 3);      //  #####
        it.line(x + 2, y + 4, x + 4, y + 4);      //   ###
        it.draw_pixel_at(x + 3, y + 5, COLOR_ON); //    #
    };

    auto draw_lives_left = [&]()
    {
        for (int i = 0; i < lives; i++)
        {
            draw_heart(1 + i * 8, 1);
        }
    };

    auto draw_score = [&]()
    {
        int padding_x = 2;
        it.printf(screen_w - padding_x, -2, id(font_xxs), TextAlign::TOP_RIGHT, "%d", score_ticker);
    };

    auto draw_level = [&]()
    {
        it.printf(screen_w / 2, -2, id(font_xxs), TextAlign::TOP_CENTER, "LVL%d", level);
    };

    auto draw_special_brick_corners = [&](int x, int y)
    {
        // Draw 2x2 L-shaped corners
        // Top-left
        it.draw_pixel_at(x, y, COLOR_ON);
        it.draw_pixel_at(x + 1, y, COLOR_ON);
        it.draw_pixel_at(x, y + 1, COLOR_ON);

        // Top-right
        it.draw_pixel_at(x + brick_w - 2, y, COLOR_ON);
        it.draw_pixel_at(x + brick_w - 1, y, COLOR_ON);
        it.draw_pixel_at(x + brick_w - 1, y + 1, COLOR_ON);

        // Bottom-left
        it.draw_pixel_at(x, y + brick_h - 2, COLOR_ON);
        it.draw_pixel_at(x, y + brick_h - 1, COLOR_ON);
        it.draw_pixel_at(x + 1, y + brick_h - 1, COLOR_ON);

        // Bottom-right
        it.draw_pixel_at(x + brick_w - 2, y + brick_h - 1, COLOR_ON);
        it.draw_pixel_at(x + brick_w - 1, y + brick_h - 1, COLOR_ON);
        it.draw_pixel_at(x + brick_w - 1, y + brick_h - 2, COLOR_ON);
    };

    auto draw_unbreakable_brick = [&](int x, int y)
    {
        it.filled_rectangle(x, y, brick_w, brick_h);
        // Clear out 2x2 squares in each corner and keep 1px border intact
        it.filled_rectangle(x + 1, y + 1, 2, 2, COLOR_OFF);
        it.filled_rectangle(x + brick_w - 3, y + 1, 2, 2, COLOR_OFF);
        it.filled_rectangle(x + 1, y + brick_h - 3, 2, 2, COLOR_OFF);
        it.filled_rectangle(x + brick_w - 3, y + brick_h - 3, 2, 2, COLOR_OFF);
    };

    auto draw_bricks = [&]()
    {
        // Draw bricks with different graphics based on HP
        for (int i = 0; i < brick_count; i++)
        {
            Brick brick = bricks[i];

            if (brick.hp == 0)
            {
                continue;
            }

            // Special rendering for multiball brick
            if (brick.type == BRICK_TYPE_EXTRA_BALL) {
                // Draw brick outline with corners
                draw_special_brick_corners(bricks[i].x, bricks[i].y);

                // Draw a ball (circle) on the left side
                int ball_cx = bricks[i].x + 3;
                int ball_cy = bricks[i].y + brick_h / 2;
                int r = 2;
                it.filled_circle(ball_cx, ball_cy, r);

                // Draw plus sign on the right side
                int plus_cx = bricks[i].x + brick_w - 6;
                int plus_cy = bricks[i].y + brick_h / 2;
                it.line(plus_cx - 2, plus_cy, plus_cx + 2, plus_cy);
                it.line(plus_cx, plus_cy - 2, plus_cx, plus_cy + 2);

                continue;
            }
            // Special rendering for multiball brick
            if (brick.type == BRICK_TYPE_SHIELD) {
                // Draw brick outline with corners
                draw_special_brick_corners(bricks[i].x, bricks[i].y);

                // Draw a horizontal line at the bottom, offset by 4 pixels from each edge
                int line_y = bricks[i].y + brick_h - 1;
                it.line(bricks[i].x + 4, line_y, bricks[i].x + brick_w - 5, line_y);
                continue;
            }

            if (brick.hp > 3)
            {
                it.filled_rectangle(bricks[i].x, bricks[i].y, brick_w, brick_h);
            }
            else if (brick.hp == 3)
            {
                it.rectangle(bricks[i].x, bricks[i].y, brick_w, brick_h);
                it.filled_rectangle(bricks[i].x + 2, bricks[i].y + 2, brick_w - 4, brick_h - 4);
            }
            else if (brick.hp == 2)
            {
                it.rectangle(bricks[i].x, bricks[i].y, brick_w, brick_h);
                it.rectangle(bricks[i].x + 2, bricks[i].y + 2, brick_w - 4, brick_h - 4);
            }
            // Unbreakable bricks
            else if (brick.hp < 0)
            {
                draw_unbreakable_brick(bricks[i].x, bricks[i].y);
            }
            else
            {
                it.rectangle(bricks[i].x, bricks[i].y, brick_w, brick_h);
            }
        }
    };

    auto draw_paddle = [&]()
    {
        if (paddle_hit)
        {
            it.filled_rectangle(paddle_x, paddle_y, paddle_w, paddle_h);
        }
        else
        {
            it.rectangle(paddle_x, paddle_y, paddle_w, paddle_h);
        }
    };

    auto draw_shield = [&]()
    {
        if (shield_amount) {
            // Draw shield under paddle
            int shield_y = screen_h-1;
            int shield_x_start = 0;
            // Draw dashed shield line under paddle
            int x = shield_x_start;
            while (x < screen_w) {
                int dash = std::min(shield_amount, screen_w - x);
                it.line(x, shield_y, x + dash - 1, shield_y);
                x += dash + 1; // dash width + 1px gap
            }
        }
    };

    // Clear display
    it.fill(COLOR_OFF);

    // Top bar
    draw_lives_left();
    draw_score();
    draw_level();

    // Game elements
    draw_bricks();
    draw_paddle();
    draw_shield();

    // Balls
    for (int b = 0; b < max_balls; b++) {
        if (balls[b].alive) {
            it.filled_rectangle(balls[b].x, balls[b].y, ball_size, ball_size);
        }
    }

    // Overlay text during pauses
    if (pause_frames > 0)
    {
        std::string text1 = "[GLITCH???]";
        std::string text2 = "";

        if (lives == max_lives)
        {
            text1 = "LEVEL " + std::to_string(level);
            text2 = "GET READY!";
        }
        else if (lives > 0)
        {
            text1 = "BALLS: " + std::to_string(lives);
        }
        else
        {
            text1 = "SCORE:";
            text2 = std::to_string(score);
        }

        int overlay_height = 17;
        if (text2 != "")
        {
            overlay_height = 26;
        }

        int overlay_h_padding = 10;
        int overlay_outline = 2;
        int text_overlay_y = (screen_h / 2 - overlay_height / 2);
        it.filled_rectangle(
            overlay_h_padding - overlay_outline,
            text_overlay_y - overlay_outline,
            (screen_w - (overlay_h_padding * 2) + overlay_outline * 2),
            overlay_height + (overlay_outline * 2), COLOR_OFF);
        it.rectangle(overlay_h_padding, text_overlay_y, screen_w - overlay_h_padding * 2, overlay_height);

        // Show pause progress bar
        int progress_bar_padding = 2;
        int progress_bar_max = (screen_w - overlay_h_padding * 2) - progress_bar_padding * 2;
        int progress_bar_width = (progress_bar_max * (pause_duration - pause_frames)) / pause_duration;
        it.rectangle(overlay_h_padding+progress_bar_padding, text_overlay_y+overlay_height-progress_bar_padding, progress_bar_width, 3);

        // Draw centered text
        int text_center_x = screen_w / 2;
        int text_center_y = screen_h / 2;
        if (text2 == "")
        {
            it.printf(text_center_x, text_center_y, id(font_sm), TextAlign::CENTER, text1.c_str());
        } else {
            it.printf(text_center_x, text_center_y-5, id(font_sm), TextAlign::CENTER, text1.c_str());
            it.printf(text_center_x, text_center_y+5, id(font_xxs), TextAlign::CENTER, text2.c_str());
        }
    }
