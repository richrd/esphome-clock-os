|-
    // Config
    int screen_w = it.get_width();
    int screen_h = it.get_height();
    // global_game_snake_speed: 1 (slow) to 20 (fast)
    int frames_per_update = 21 - id(global_game_snake_speed);
    int max_x = 25;
    int max_y = 11;
    int pickup_score = 10;
    int rumble_duration = 4;

    struct BodyPart {
        int x;
        int y;
    };

    struct Pickup {
        int x;
        int y;
        int type;
    };


    // State
    static int frame = 0;
    static int rumble_frames = 0;
    static int direction_x = 1; // 1=right, -1=left
    static int direction_y = 0; // 1=down, -1=up
    static int score = 0;
    static int score_ticker = 0;
    static BodyPart target_pickup = {-1, -1};

    // This is defined in main.yaml and updated by the knob events
    // When a game update cycle is completed the value is reset to 0
    int knob_direction = id(clockos_global_knob_direction);

    int PICKUP_TYPE_NORMAL = 1;
    static std::vector<Pickup> pickups = {};

    static std::vector<BodyPart> snake_body = {
        {4, 5},
        {5, 5},
        {6, 5},
    };

    auto spawn_pickups = [&]()
    {
        // Number of pickups to maintain on the board
        static int pickup_count = 4;

        // Only spawn pickups if we have fewer than pickup_count
        while (pickups.size() < pickup_count) {
            // Spawn at random location not occupied by snake or existing pickups
            int px = 0;
            int py = 0;
            bool valid_position = false;
            while (!valid_position) {
                px = esp_random() % max_x;
                py = esp_random() % max_y;
                valid_position = true;
                // Check snake body
                for (auto part : snake_body) {
                    if (part.x == px && part.y == py) {
                    valid_position = false;
                    break;
                    }
                }
                // Check existing pickups
                if (valid_position) {
                    for (auto p : pickups) {
                        if (p.x == px && p.y == py) {
                            valid_position = false;
                            break;
                        }
                    }
                }
            }
            pickups.push_back({px, py, PICKUP_TYPE_NORMAL});
        }
    };
    
    if (pickups.empty()) {
        spawn_pickups();
    }

    auto reset_game = [&]()
    {
        frame = 0;
        direction_x = 1;
        direction_y = 0;
        score = 0;
        score_ticker = 0;
        pickups = {};
        spawn_pickups();
        snake_body = {
            {4, 5},
            {5, 5},
            {6, 5},
        };
    };

    auto rumble = [&]() {
        if (id(global_game_rumble)) {
            id(clockos_output_rumble).turn_on();
            rumble_frames = rumble_duration;
        }
    };

    ////////////////////////////////
    // Game logic
    ////////////////////////////////
    frame++;
    if (frame > frames_per_update) {
        frame = 0;
    }

    // Animate score ticker
    if (score_ticker < score) {
        score_ticker++;
    }

    // Handle rumble feedback
    if (rumble_frames > 0) {
        rumble_frames--;
        if (rumble_frames == 0) {
            id(clockos_output_rumble).turn_off();
        }
    }

    int old_direction_x = direction_x;
    int old_direction_y = direction_y;
    
    BodyPart head = snake_body.back();
    if (frame == frames_per_update) {
        if (!id(clockos_global_game_autoplay)) {
            if (knob_direction == 1) {
                // Knob turned clockwise
                if (direction_x != 0) {
                    // Moving horizontally, turn vertically
                    if (direction_x == 1) {
                        direction_y = 1;
                    } else {
                        direction_y = -1;
                    }
                    direction_x = 0;
                } else if (direction_y != 0) {
                    // Moving vertically, turn horizontally
                    if (direction_y == 1) {
                        direction_x = -1;
                    } else {
                        direction_x = 1;
                    }
                    direction_y = 0;
                }
            }
            else if (knob_direction == -1) {
                // Knob turned anti-clockwise
                if (direction_x != 0) {
                    // Moving horizontally, turn vertically
                    if (direction_x == 1) {
                        direction_y = -1;
                    } else {
                        direction_y = 1;
                    }
                    direction_x = 0;
                } else if (direction_y != 0) {
                    // Moving vertically, turn horizontally
                    if (direction_y == 1) {
                        direction_x = 1;
                    } else {
                        direction_x = -1;
                    }
                    direction_y = 0;
                }
            }
        } else {
            // Determine new target pickup
            // Check if the current target pickup still exists
            bool found = false;
            for (const auto& p : pickups) {
                if (p.x == target_pickup.x && p.y == target_pickup.y) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                target_pickup.x = -1;
                target_pickup.y = -1;
            }

            // If current pickup doesn't exist, pick a new one
            if (target_pickup.x == -1 && !pickups.empty()) {
                target_pickup.x = pickups[0].x;
                target_pickup.y = pickups[0].y;
            }
            
            
            // Autoplay: turn towards pickup if it's left or right of the head
            if (target_pickup.x != -1) {
                if (direction_x != 0) { // Moving horizontally
                    if (target_pickup.y < head.y && direction_y == 0) {
                        direction_x = 0;
                        direction_y = -1;
                    } else if (target_pickup.y > head.y && direction_y == 0) {
                        direction_x = 0;
                        direction_y = 1;
                    }
                } else if (direction_y != 0) { // Moving vertically
                    if (target_pickup.x < head.x && direction_x == 0) {
                        direction_x = -1;
                        direction_y = 0;
                    } else if (target_pickup.x > head.x && direction_x == 0) {
                        direction_x = 1;
                        direction_y = 0;
                    }
                }
            }

            // Autoplay: avoid running into the snake itself
            int test_x = head.x + direction_x;
            int test_y = head.y + direction_y;
            bool will_collide = false;
            for (const auto& part : snake_body) {
                if (part.x == test_x && part.y == test_y) {
                    will_collide = true;
                    break;
                }
            }
            if (will_collide) {
                // Try alternate directions: up, down, left, right (not reversing)
                struct Dir { int dx, dy; };
                std::vector<Dir> alternatives;
                if (direction_x != 0) {
                    alternatives = { {0, 1}, {0, -1} };
                } else {
                    alternatives = { {1, 0}, {-1, 0} };
                }
                bool found_safe = false;
                for (const auto& alt : alternatives) {
                    int nx = head.x + alt.dx;
                    int ny = head.y + alt.dy;
                    bool safe = true;
                    for (const auto& part : snake_body) {
                        if (part.x == nx && part.y == ny) {
                            safe = false;
                            break;
                        }
                    }
                    if (safe) {
                        direction_x = alt.dx;
                        direction_y = alt.dy;
                        found_safe = true;
                        break;
                    }
                }
                // If no safe direction found, keep current direction (will reset on collision)
            }
        }

        // Move snake forward
        int new_x = head.x + direction_x;
        int new_y = head.y + direction_y;

        bool collided_with_wall = false;
        // Check for wall collisions
        if (new_x < 0 || new_x >= max_x || new_y < 0 || new_y >= max_y) {
            if (id(global_game_snake_walls)) {
                // Collided with wall - game over
                collided_with_wall = true;
                reset_game();
            } else {
                // Wrap around
                if (new_x < 0) {
                    new_x = max_x - 1;
                } else if (new_x >= max_x) {
                    new_x = 0;
                }
                if (new_y < 0) {
                    new_y = max_y - 1;
                } else if (new_y >= max_y) {
                    new_y = 0;
                }
            }
        }

        

        if(!collided_with_wall) {
            // Check for collision with self
            bool collided = false;
            for (const auto& part : snake_body) {
                if (part.x == new_x && part.y == new_y) {
                    if (direction_x != old_direction_x || direction_y != old_direction_y) {
                        // Snake has turned into itself due to knob turn, ignore the turn
                        new_x = head.x + old_direction_x;
                        new_y = head.y + old_direction_y;
                        break;
                    } else {
                        collided = true;
                        break;
                    }
                }
            }

            if (collided) {
                // Collided with self - game over
                reset_game();
                return;
            } else {
                bool ate_pickup = false;
                for (auto it = pickups.begin(); it != pickups.end(); ++it) {
                    if (it->x == new_x && it->y == new_y) {
                    ate_pickup = true;
                    pickups.erase(it);
                    score = score + pickup_score;
                    rumble();
                    spawn_pickups();
                    break;
                    }
                }
                if (!ate_pickup) {
                    snake_body.erase(snake_body.begin());
                }
                snake_body.push_back({new_x, new_y});
            }
        }

        // Handle completed update
        frame = 0;
        id(clockos_global_knob_direction) = 0;
    }

    /*
    // Push to the end of the array (snake grows at the tail)
    snake_body.push_back({screen_w / 2 - 3, screen_h / 2});

    // Push to the start of the array (snake moves forward at the head)
    snake_body.insert(snake_body.begin(), {screen_w / 2 + 1, screen_h / 2});

    // Remove from the start of the array (remove head)
    snake_body.erase(snake_body.begin());

    // Remove from the end of the array (remove tail)
    snake_body.pop_back();
    */

    ////////////////////////////////
    // Rendering
    ////////////////////////////////

    auto draw_score = [&]()
    {
        // Draw score in top right of status bar
        int padding_x = 2;
        it.printf(screen_w - padding_x, -2, id(font_xxs), TextAlign::TOP_RIGHT, "%d", score_ticker);
    };

    auto draw_direction = [&]()
    {
        // Draw knob direction indicator in top center of status bar
        if (id(clockos_global_knob_direction) == 0) {
            return;
        }
        int direction_icon_w = 5;
        int direction_icon_h = 6;
        int direction_icon_x = (screen_w / 2) - (direction_icon_w / 2);
        int direction_icon_y = 1;

        it.draw_pixel_at(direction_icon_x + 2, direction_icon_y, COLOR_ON);
        it.draw_pixel_at(direction_icon_x + 2, direction_icon_y+4, COLOR_ON);
        it.line(direction_icon_x, direction_icon_y+2, direction_icon_x+direction_icon_w-1, direction_icon_y+2);
        if (id(clockos_global_knob_direction) == 1) {
            // Clockwise
            it.line(direction_icon_x, direction_icon_y+2, direction_icon_x, direction_icon_y+direction_icon_h-1);
            it.line(direction_icon_x+3, direction_icon_y+1, direction_icon_x+3, direction_icon_y+3);
        } else if (id(clockos_global_knob_direction) == -1) {
            // Anti-clockwise
            it.line(direction_icon_x+direction_icon_w-1, direction_icon_y+2, direction_icon_x+direction_icon_w-1, direction_icon_y+direction_icon_h-1);
            it.line(direction_icon_x+1, direction_icon_y+1, direction_icon_x+1, direction_icon_y+3);
        }
    };


    it.fill(COLOR_OFF);
    draw_score();
    draw_direction();

    // Snake and grid
    int box_size = 4;
    int offset_y = 8;

    /*
    // Draw grid
    for (int y = 0; y < max_y; y++) {
        for (int x = 0; x < max_x; x++) {
            it.rectangle(x * (box_size+1), offset_y + y * (box_size+1), box_size, box_size);
        }
    }
    */

    if(id(global_game_snake_walls)) {
        // Draw border
        it.rectangle(0, offset_y, max_x * (box_size+1)-1, max_y * (box_size+1)-1);
    } else {
        // Draw corners to indicate the playing field without full border, 4x4 L shapes in each corner
        int l_size = 4;
        int thickness = 1;

        // Top-left corner
        it.filled_rectangle(0, offset_y, l_size, thickness); // horizontal
        it.filled_rectangle(0, offset_y, thickness, l_size); // vertical

        // Top-right corner
        int right_x = max_x * (box_size + 1) - l_size;
        it.filled_rectangle(right_x, offset_y, l_size, thickness); // horizontal
        it.filled_rectangle(right_x + l_size - thickness, offset_y, thickness, l_size); // vertical

        // Bottom-left corner
        int bottom_y = offset_y + max_y * (box_size + 1) - l_size;
        it.filled_rectangle(0, bottom_y + l_size - thickness, l_size, thickness); // horizontal
        it.filled_rectangle(0, bottom_y, thickness, l_size); // vertical

        // Bottom-right corner
        it.filled_rectangle(right_x, bottom_y + l_size - thickness, l_size, thickness); // horizontal
        it.filled_rectangle(right_x + l_size - thickness, bottom_y, thickness, l_size); // vertical
    }

    // Snake body
    for (size_t i = 0; i < snake_body.size(); ++i) {
        const auto& part = snake_body[i];
        int x1 = part.x * (box_size + 1);
        int y1 = offset_y + part.y * (box_size + 1);

        it.filled_rectangle(x1, y1, box_size, box_size);

        // Draw overlapping rectangle with previous part for continuity,
        // but skip if wrapping occurred
        if (i > 0) {
            const auto& prev = snake_body[i - 1];
            int dx = std::abs(part.x - prev.x);
            int dy = std::abs(part.y - prev.y);

            // If not wrapping (adjacent cells), draw connector
            if ((dx == 1 && dy == 0) || (dx == 0 && dy == 1)) {
                int x2 = prev.x * (box_size + 1);
                int y2 = offset_y + prev.y * (box_size + 1);

                int min_x = std::min(x1, x2);
                int min_y = std::min(y1, y2);
                int max_x = std::max(x1, x2);
                int max_y = std::max(y1, y2);

                // Draw a rectangle spanning both parts
                it.filled_rectangle(min_x, min_y, max_x - min_x + box_size, max_y - min_y + box_size);
            }
            // else: wrapping occurred, skip drawing connector
        }
    }

    // Draw full horizontal and vertical lines (crosshairs) through the head
    head = snake_body.back();
    int head_cx = head.x * (box_size + 1) + box_size / 2;
    int head_cy = offset_y + head.y * (box_size + 1) + box_size / 2;
    // Horizontal line
    for (int x = 0; x < max_x; ++x) {
        int px = x * (box_size + 1) + box_size / 2;
        it.draw_pixel_at(px, head_cy, COLOR_ON);
    }
    // Vertical line
    for (int y = 0; y < max_y; ++y) {
        int py = offset_y + y * (box_size + 1) + box_size / 2;
        it.draw_pixel_at(head_cx, py, COLOR_ON);
    }

    for (auto pickup : pickups) {
        // Draw a larger, more interesting pickup: a 3x3 square with a cross
        int px = pickup.x * (box_size + 1);
        int py = offset_y + pickup.y * (box_size + 1);

        // Draw 3x3 filled square
        it.filled_rectangle(px - 1, py - 1, box_size + 2, box_size + 2);

        // Draw a cross inside the square
        it.line(px - 1, py - 1, px + box_size, py + box_size, COLOR_OFF);
        it.line(px + box_size, py - 1, px - 1, py + box_size, COLOR_OFF);
    }
