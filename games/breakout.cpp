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
    int knob_max = 100.0;
    int pause_duration = 100; // 100 frames, at 30fps = ~3 seconds
    int points_per_brick = 5;
    int points_per_paddle_hit = 10;
    bool auto_play = true;

    // Calculate pad position based on knob state
    int paddle_x = id(knob).state * ((float)(screen_w - paddle_w) / knob_max);
    int paddle_y = screen_h - paddle_h;
    bool paddle_hit = false;

    // State
    static int pause_frames = pause_duration;
    static int max_lives = 5;
    static int lives = 5;
    static int score = 0;
    static int score_ticker = 0;
    static int level = 0;
    static int ball_x = screen_w / 2;
    static int ball_y = screen_h - 10;
    static int direction_x = 1;
    static int direction_y = 1;

    // Autoplay
    if (id(global_game_autoplay))
    {
        paddle_x = ball_x - (paddle_w / 2);
        if (paddle_x < 0)
        {
            paddle_x = 0;
        }
        else if (paddle_x + paddle_w > screen_w)
        {
            paddle_x = screen_w - paddle_w;
        }
    }

    struct Brick
    {
        int x;
        int y;
        int hp;
        int type;
        // Type defines the behavior of the brick
        // 0 = normal
        // ? = unbreakable
        // ? = upwards only gate
        // ? = inverter brick
        // ? = extra ball
        // ? = extra life
        // ?  ==explode adjacent bricks
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
    auto clear_brick_hp = [&]()
    {
        for (int i = 0; i < brick_count; i++)
        {
            bricks[i].hp = 0;
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

    auto setup_next_level = [&]()
    {
        level++;
        lives = 5;
        direction_y = -1;
        pause_frames = pause_duration;
        int brick_hp = level;
        if (brick_hp > brick_max_hp)
        {
            brick_hp = brick_max_hp;
        }
        clear_brick_hp();
        int start_brick = 8;
        int end_brick = brick_count - 16;

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
    };

    auto on_brick_hit = [&](int id)
    {
        bricks[id].hp--;
        score = score + points_per_brick;
        if (id(global_game_sound)) {
            id(rtttl_player).stop();
            id(rtttl_player).play("Untitled:d=4,o=6,b=63:32c");
        }
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

    // Check if paused
    if (pause_frames)
    {
        // Consume a frame
        pause_frames--;

        // Keep ball on paddle
        ball_x = paddle_x + (paddle_w / 2);
        ball_y = screen_h - paddle_h - ball_size;

        // If all frames consumed, check for game over
        if (!pause_frames && lives == 0)
        {
            reset_game();
        }
    }
    else
    {
        // Game loop logix
        ball_x = ball_x + direction_x;
        ball_y = ball_y + direction_y;

        // Wall collisions
        if (ball_x < 0)
        { // Left
            direction_x = 1;
        }
        if (ball_y < 0)
        { // Top
            direction_y = 1;
        }
        if (ball_x + ball_size > screen_w)
        { // Right
            direction_x = -1;
        }

        // Bottom (missed paddle)
        if (ball_y + ball_size > screen_h)
        {
            lives--;
            ball_x = paddle_x + (paddle_w / 2);
            ball_y = screen_h - paddle_h - ball_size;
            direction_y = -1;
            pause_frames = pause_duration;
        }

        // Brick collisions
        for (int i = 0; i < brick_count; i++)
        {
            Brick& brick = bricks[i];

            if (brick.hp < 1)
            {
                continue;
            }

            // Horizontal collisions
            if (
                ball_y >= brick.y && ball_y + ball_size <= brick.y + brick_h)
            {
                // Collision on left edge (ball moving right)
                if (direction_x == 1)
                {
                    if (ball_x + ball_size >= brick.x && ball_x + ball_size <= brick.x + brick_w)
                    {
                        direction_x = -1;
                        on_brick_hit(i);
                    }
                }
                // Collision on right edge (ball moving left)
                else if (direction_x == -1)
                {
                    if (ball_x <= brick.x + brick_w && ball_x >= brick.x)
                    {
                        direction_x = 1;
                        on_brick_hit(i);
                    }
                }
            }
            // Vertical collisions
            if (
                ball_x >= brick.x && ball_x + ball_size <= brick.x + brick_w)
            {
                // Collision on top edge (ball moving down)
                if (direction_y == 1)
                {
                    if (ball_y + ball_size >= brick.y && ball_y + ball_size <= brick.y + brick_h)
                    {
                        direction_y = -1;
                        on_brick_hit(i);
                    }
                }
                // Collision on bottom edge (ball moving up)
                else if (direction_y == -1)
                {
                    if (ball_y <= brick.y + brick_h && ball_y >= brick.y)
                    {
                        direction_y = 1;
                        on_brick_hit(i);
                    }
                }
            }
        }

        // Check if ball hits paddle
        if (
            ball_x >= paddle_x
            && ball_x <= paddle_x + paddle_w
            && ball_y + ball_size > screen_h - paddle_h
            && direction_y == 1
        )
        {
            direction_y = -1;
            score = score + points_per_paddle_hit;
            paddle_hit = true;
            if (ball_x + (ball_size / 2) > paddle_x + (paddle_w / 2))
            {
                direction_x = 1;
            }
            else
            {
                direction_x = -1;
            }
        }
    }

    ////////////////////////////////
    // Rendering
    ////////////////////////////////

    // Functions
    auto draw_heart = [&](int x, int y)
    {
        it.line(x + 1, y, x + 2, y);
        it.line(x + 4, y, x + 5, y);              //  ## ##
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
        // it.printf(screen_w - padding_x, -2, id(font_xxs), TextAlign::TOP_RIGHT, "%d", score);
        it.printf(screen_w - padding_x, -2, id(font_xxs), TextAlign::TOP_RIGHT, "%d", score_ticker);
    };

    auto draw_level = [&]()
    {
                // Draw centered text
        int text_center_x = screen_w / 2;
        int text_center_y = 0;
        it.printf(text_center_x, text_center_y, id(font_xxs), TextAlign::CENTER, "LVL%d", level);
        // it.printf(screen_w - padding_x, -2, id(font_xxs), TextAlign::TOP_RIGHT, "%d", score_ticker);
    };

    auto draw_bricks = [&]()
    {
        // Draw bricks with different graphics based on HP
        for (int i = 0; i < brick_count; i++)
        {
            Brick brick = bricks[i];
            if (brick.hp < 1)
            {
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
            else
            {
                it.rectangle(bricks[i].x, bricks[i].y, brick_w, brick_h);
            }
        }
    };

    // Clear display
    it.fill(COLOR_OFF);

    // Top bar
    draw_lives_left();
    draw_score();
    draw_level();


    draw_bricks();

    // Paddle
    if (paddle_hit)
    {
        it.filled_rectangle(paddle_x, paddle_y, paddle_w, paddle_h);
    }
    else
    {
        it.rectangle(paddle_x, paddle_y, paddle_w, paddle_h);
    }

    // Ball
    it.filled_rectangle(ball_x, ball_y, ball_size, ball_size);

    // Overlay text
    if (pause_frames > 0)
    {
        std::string text1 = "[GLITCH???]";
        std::string text2 = "";

        if (lives == max_lives)
        {
            // TODO: draw "LEVEL 1" and "GET READY!" on separate lines
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
