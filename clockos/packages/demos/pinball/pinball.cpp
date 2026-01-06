|-
    // Config
    int screen_w = it.get_width();
    int screen_h = it.get_height();
    int max_level_height = 256;
    int scroll_padding = screen_h - 15;

    float ball_size = 5.0f;
    float ball_radius = ball_size / 2.0f;
    float ball_friction_x = 0.998f;
    float ball_friction_y = 0.999f;
    float ball_gravity = 0.1f;
    float ball_max_speed = 6.0f;
    float wall_speed_factor = 0.93f; // Increase this for more bounce
    float bumper_speed_factor = 1.22f; // Increase this for more bounce

    // State
    static int vertical_scroll_offset = 0;
    static float left_paddle_angle = 30;
    static float right_paddle_angle = 30;
    float paddle_x_offset = 35.0f;
    int paddle_min_angle = 30;
    int paddle_max_angle = -30;
    float paddle_radius = 4.5;
    int paddle_length = 23;

    struct Ball
    {
        float x;
        float y;
        float prev_x;
        float prev_y;
        float speed_x;
        float speed_y;
        bool alive;
    };

    struct Bumper
    {
        float x;
        float y;
        float radius;
        int hit_frames;
    };

    static const int max_balls = 10;
    static Ball balls[max_balls] = {
        {64, 0, 0, 0, 0.5, 0, true},
        {0, 0, 0, 0, 0, 0, false},
        {0, 0, 0, 0, 0, 0, false},
        {0, 0, 0, 0, 0, 0, false},
        {0, 0, 0, 0, 0, 0, false},
        {0, 0, 0, 0, 0, 0, false},
        {0, 0, 0, 0, 0, 0, false},
        {0, 0, 0, 0, 0, 0, false},
        {0, 0, 0, 0, 0, 0, false},
        {0, 0, 0, 0, 0, 0, false},
    };


    static const int max_bumpers = 12;
    // static const int max_bumpers = 0;
    static Bumper bumpers[max_bumpers] = {
        {20, 20, 4, 0},
        {108, 20, 4, 0},
        {50, 40, 4, 0},
        {78, 40, 4, 0},
        {20, 60, 4, 0},
        {108, 60, 4, 0},
        {50, 80, 4, 0},
        {78, 80, 4, 0},
        {20, 120, 4, 0},
        {108, 120, 4, 0},
        {50, 160, 4, 0},
        {78, 160, 4, 0},
    };


    struct Vector2D {
        float x;
        float y;
    };

    std::vector<std::vector<Vector2D>> field_triangles = {
        {{0,1},{1,0}, {0, 0}},
        {{0,1},{1,0}, {0, 0}},
        // Triangle points must be defined in clockwise order
        // {{10,10}, {60,10}, {10,60}},
        // {{50,100}, {50,50}, {100,50}},

        // {{10,60}, {60,10}, {10,10},},
        // {{100,50}, {50,50}, {50,100},},

        // {{10,160}, {80, 140}, {10, 140},},

        // {{float(screen_w)-10,160}, {float(screen_w)-10, 140}, {float(screen_w)-80, 140},},
        {{0,25},{0,40}, {35, 25}},
        {{float(screen_w)-35,25}, {float(screen_w), 40}, {float(screen_w), 25}},
    };

    // Subtract vector B from vector A
    auto subtract = [&](const Vector2D a, const Vector2D b) {
        Vector2D vec ={a.x - b.x, a.y - b.y};
        return vec;
    };


    // Add vector A and vector B
    auto add = [&](const Vector2D& a, const Vector2D& b) {
        Vector2D vec = {a.x + b.x, a.y + b.y};
        return vec;
    };

    // Scale a vector by a scalar
    auto scale = [&](const Vector2D& v, float s) {
        Vector2D vec = {v.x * s, v.y * s};
        return vec;
    };

    // Calculate the dot product
    auto dot = [&](const Vector2D& a, const Vector2D& b) {
        float vec = a.x * b.x + a.y * b.y;
        return vec;
    };

    // Calculate the squared length of a vector
    auto lengthSq = [&](const Vector2D& v) {
        return dot(v, v);
    };

    // Calculate the length (magnitude) of a vector
    auto length = [&](const Vector2D& v) {
        return std::sqrt(lengthSq(v));
    };

    // Normalize a vector (make its length 1)
    auto normalize = [&](const Vector2D& v) {
        float len = length(v);
        Vector2D vec = {0.0f, 0.0f};
        if (len > 1e-6) {
            vec = {v.x / len, v.y / len};
        }
        return vec;
    };

    // Returns true if point is inside triangle (assuming CCW winding)
    auto pointInTriangle = [&](Vector2D p, Vector2D a, Vector2D b, Vector2D c) -> bool {
        auto sign = [](Vector2D p1, Vector2D p2, Vector2D p3) {
            return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
        };
        bool b1 = sign(p, a, b) < 0.0f;
        bool b2 = sign(p, b, c) < 0.0f;
        bool b3 = sign(p, c, a) < 0.0f;
        return (b1 == b2) && (b2 == b3);
    };

    auto clamp_ball_speed = [&](Ball &ball) {
        float current_speed = std::sqrt(ball.speed_x * ball.speed_x + ball.speed_y * ball.speed_y);
        if (current_speed > ball_max_speed) {
            float scale_factor = ball_max_speed / current_speed;
            ball.speed_x *= scale_factor;
            ball.speed_y *= scale_factor;
        }
    };






    // Returns true if collision happened this frame.
    // On hit: corrects position to exact impact point, reflects velocity, pushes ball out slightly.
    auto circleVsSegment= [&](
        float cx, float cy,           // ball center this frame (before move)
        float px, float py,           // ball center previous frame
        float radius,
        Vector2D segA, Vector2D segB, // line segment endpoints (world coordinates)
        float& out_penetration,       // how deep we were (used for push-out)
        Vector2D& out_normal)         // collision normal (points away from segment)
    {
        Vector2D segVec = subtract(segB, segA);
        Vector2D toStart = subtract({cx, cy}, segA);

        float segLen = length(segVec);
        if (segLen < 0.001f) return false;  // degenerate segment

        Vector2D segDir = normalize(segVec);

        // Project ball center onto infinite line
        float proj = dot(toStart, segDir);

        // Closest point on the infinite line
        Vector2D closestInfinite = add(segA, scale(segDir, proj));

        // Distance from ball center to infinite line
        Vector2D toClosest = subtract({cx, cy}, closestInfinite);
        float distToLine = length(toClosest);

        // Early out if we're farther than radius and moving away
        if (distToLine > radius + 0.1f) {
            // Check if we're moving away from the line
            Vector2D vel = {cx - px, cy - py};
            if (dot(vel, toClosest) >= 0) return false;
        }

        // Find closest point on the actual segment
        float t = proj / segLen;
        t = fmaxf(0.0f, fminf(1.0f, t));  // clamp to segment
        Vector2D closest = add(segA, scale(segDir, t * segLen));

        Vector2D delta = subtract({cx, cy}, closest);
        float distance = length(delta);

        if (distance >= radius + 0.1f) return false;  // no collision

        // We have a hit!
        if (distance < 0.001f) distance = 0.001f;
        out_normal = normalize(delta);           // points from segment toward ball
        out_penetration = radius - distance;

        // Flip normal if ball is on the "wrong" side (optional, depends on triangle winding)
        // For now we keep it pointing outward from segment toward ball

        return true;
    };



    auto handle_paddle_input = [&]()
    {
        if(id(clockos_key_left_paddle).state)
        {
            left_paddle_angle -= 20.3;
        } else {
            left_paddle_angle += 18.3;
        }

        if (left_paddle_angle > paddle_min_angle) {
            left_paddle_angle = paddle_min_angle;
        }
        if (left_paddle_angle < paddle_max_angle) {
            left_paddle_angle = paddle_max_angle;
        }
        if(id(clockos_key_right_paddle).state)
        {
            right_paddle_angle -= 20.3;
        } else {
            right_paddle_angle += 18.3;
        }
        if (right_paddle_angle > paddle_min_angle) {
            right_paddle_angle = paddle_min_angle;
        }
        if (right_paddle_angle < paddle_max_angle) {
            right_paddle_angle = paddle_max_angle;
        }
    };

    auto update_ball = [&](Ball &ball)
    {
        // Handle user input
        float input_strength = 0.17f;
        // Knob input for testing
        if (id(clockos_global_knob_direction) != 0) {
            ball.speed_x += input_strength * id(clockos_global_knob_direction);
            ball.speed_y += input_strength * 2;
            id(clockos_global_knob_direction) = 0;
        }

        // Host input for testing
        /**/
        if (id(clockos_key_up).state)
        {
            ball.speed_y += input_strength;
        }
        if (id(clockos_key_down).state)
        {
            ball.speed_y -= input_strength;
        }
        if (id(clockos_key_left).state)
        {
            ball.speed_x -= input_strength;
        }
        if (id(clockos_key_right).state)
        {
            ball.speed_x += input_strength;
        }

        handle_paddle_input();
        // Gravity
        ball.speed_y -= ball_gravity;
        // Friction
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
        if (ball.y < 0)
        { // Bottom
            ball.y = 0;
            ball.speed_y = ball.speed_y * -0.8;
            collided_with_wall = true;
        }
        if (ball.y > max_level_height)
        { // Top
            ball.y = max_level_height - ball_size;
            ball.speed_y = ball.speed_y * -1;
            collided_with_wall = true;
        }

        if (collided_with_wall) {
            ball.speed_x *= wall_speed_factor;
            ball.speed_y *= wall_speed_factor;
        }

        // Check collision with bumpers
        // Written by AI, works fine so far
        for (int i = 0; i < max_bumpers; i++) {
            Bumper &bumper = bumpers[i];
            // TODO: Perhaps directly store the center of the ball to avoid calculating it
            float dx = ball.x + ball_size / 2.0f - bumper.x;
            float dy = ball.y + ball_size / 2.0f - bumper.y;
            float dist_sq = dx * dx + dy * dy;
            float min_dist = bumper.radius + ball_size / 2.0f;
            if (dist_sq < min_dist * min_dist) {
                float dist = sqrtf(dist_sq);
                // Normalize direction
                float nx = dx / (dist > 0 ? dist : 1.0f);
                float ny = dy / (dist > 0 ? dist : 1.0f);

                // Reflect ball velocity
                float dot = ball.speed_x * nx + ball.speed_y * ny;
                ball.speed_x = ball.speed_x - 2 * dot * nx * bumper_speed_factor;
                ball.speed_y = ball.speed_y - 2 * dot * ny * bumper_speed_factor;

                // Move ball out of bumper to avoid sticking
                float overlap = min_dist - dist;
                ball.x += nx * overlap;
                ball.y += ny * overlap;

                bumper.hit_frames = 6;
            }
        }



        // ——— SOLID ONE-SIDED TRIANGLE COLLISIONS ———
        Vector2D ballCenter = {ball.x + ball_size/2, ball.y + ball_size/2};
        Vector2D ballPrevCenter = {ball.prev_x + ball_size/2, ball.prev_y + ball_size/2};
        Vector2D vel = subtract(ballCenter, ballPrevCenter);

        float velLenSq = lengthSq(vel);
        if (velLenSq < 0.001f) {
            // No movement, skip to discrete check
        } else {
            // SWEPT CHECK: Find earliest collision time t along path
            float minT = 2.0f;  // Invalid (>1)
            Vector2D hitNormal = {0.0f, 0.0f};
            const std::vector<Vector2D>* hitTriangle = nullptr;
            for (const auto& tri : field_triangles) {
                for (int i = 0; i < 3; ++i) {
                    Vector2D a = tri[i];
                    Vector2D b = tri[(i + 1) % 3];
                    Vector2D edge = subtract(b, a);
                    float segLen = length(edge);
                    if (segLen < 0.001f) continue;
                    Vector2D segDir = scale(edge, 1.0f / segLen);
                    Vector2D outwardNormal = normalize({-edge.y, edge.x});
                    Vector2D toA0 = subtract(ballPrevCenter, a);
                    float d0 = dot(toA0, outwardNormal);
                    float dotVN = dot(vel, outwardNormal);
                    if (dotVN >= 0.0f) continue;  // Not approaching
                    if (d0 < ball_radius - 0.5f) continue;  // Already too inside (fallback to discrete)
                    float t = (ball_radius - d0) / dotVN;
                    if (t < 0.0f || t > 1.0f) continue;
                    // Check if projection at t is within segment bounds
                    Vector2D pt = add(ballPrevCenter, scale(vel, t));
                    Vector2D toAt = subtract(pt, a);
                    float proj = dot(toAt, segDir);
                    if (proj >= 0.0f && proj <= segLen) {
                        // Valid hit
                        if (t < minT) {
                            minT = t;
                            hitNormal = outwardNormal;
                            hitTriangle = &tri;
                        }
                    }
                }
            }
            if (minT < 2.0f) {
                // Resolve earliest hit
                Vector2D impactCenter = add(ballPrevCenter, scale(vel, minT));
                // Set position to impact point + small push out
                Vector2D push = scale(hitNormal, 0.3f);
                ball.x = impactCenter.x + push.x - ball_radius;
                ball.y = impactCenter.y + push.y - ball_radius;
                // Reflect velocity
                float dotVal = ball.speed_x * hitNormal.x + ball.speed_y * hitNormal.y;
                ball.speed_x -= 2.0f * dotVal * hitNormal.x * wall_speed_factor;
                ball.speed_y -= 2.0f * dotVal * hitNormal.y * wall_speed_factor;
                // Apply remaining movement (1 - minT) with new velocity
                ball.x += (1.0f - minT) * ball.speed_x;
                ball.y += (1.0f - minT) * ball.speed_y;
                // Update centers for discrete check below (if needed)
                ballCenter = {ball.x + ball_radius, ball.y + ball_radius};
            }
        }


        // DISCRETE CHECK: Your original code as fallback for any remaining penetration
        float deepestPenetration = 0.0f;
        Vector2D bestNormal = {0,0};
        bool anyHit = false;
        const std::vector<Vector2D>* hitTriangle = nullptr;
        // std::vector<std::vector<Vector2D>> triangles = field_triangles;


        // Add paddles
        float angle_rad = left_paddle_angle * (3.14159265f / 180.0f);
        float left_paddle_x = paddle_x_offset;
        float left_paddle_y = 20.0f;
        float end_x = left_paddle_x + paddle_length * std::cos(angle_rad);
        float end_y = left_paddle_y + paddle_length * std::sin(-angle_rad);

        field_triangles[0] = {
            {left_paddle_x, left_paddle_y + paddle_radius},
            {end_x, end_y + paddle_radius*2},
            {end_x, end_y + paddle_radius},
        };

        // ESP_LOGW("PINBALL", "Left paddle line: (%.1f, %.1f) to (%.1f, %.1f)", left_paddle_x, left_paddle_y, end_x, end_y);

        for (const auto& tri : field_triangles) {
            for (int i = 0; i < 3; ++i) {
                Vector2D a = tri[i];
                Vector2D b = tri[(i+1)%3];
                // Compute edge normal pointing OUTWARD (right-hand rule for CCW triangle)
                Vector2D edge = subtract(b, a);
                Vector2D outwardNormal = normalize({-edge.y, edge.x}); // rotate 90° CW
                // Distance from ball center to infinite line
                Vector2D toA = subtract({ballCenter.x, ballCenter.y}, a);
                float signedDist = dot(toA, outwardNormal);
                // Early reject: ball is on the outside and moving away
                if (signedDist >= ball_radius) {
                    Vector2D vel = {ballCenter.x - ballPrevCenter.x, ballCenter.y - ballPrevCenter.y};
                    if (dot(vel, outwardNormal) >= 0) continue;
                }
                // Closest point on segment
                float t = dot(toA, edge) / lengthSq(edge);
                t = fmaxf(0.0f, fminf(1.0f, t));
                Vector2D closest = add(a, scale(edge, t));
                Vector2D delta = subtract(ballCenter, closest);
                float dist = length(delta);
                float penetration = ball_radius - dist;
                if (penetration > 0.01f) { // actual collision
                    Vector2D normal = normalize(delta);
                    // Only accept collision if we're hitting from the OUTSIDE
                    if (dot(normal, outwardNormal) > 0.1f) { // >0.1 to avoid grazing edges
                        if (!anyHit || penetration > deepestPenetration) {
                            deepestPenetration = penetration;
                            bestNormal = normal;
                            hitTriangle = &tri;
                            anyHit = true;
                        }
                    }
                }
            }
        }
        if (anyHit) {
            // Final safety: if ball is actually inside the triangle, push it out along the best normal
            if (hitTriangle && pointInTriangle(ballCenter, hitTriangle->at(0), hitTriangle->at(1), hitTriangle->at(2))) {
                // Force outward normal using triangle centroid → ball
                Vector2D centroid = {
                    (hitTriangle->at(0).x + hitTriangle->at(1).x + hitTriangle->at(2).x) / 3.0f,
                    (hitTriangle->at(0).y + hitTriangle->at(1).y + hitTriangle->at(2).y) / 3.0f
                };
                bestNormal = normalize(subtract(ballCenter, centroid));
                // deepestPenetration = ball_radius + 1.0f; // big push
                deepestPenetration = ball_radius + 3.0f; // big push
            }
            // Push out
            float pushOffset = 1.2f; // extra push to avoid re-collision
            ball.x += bestNormal.x * (deepestPenetration + pushOffset);
            ball.y += bestNormal.y * (deepestPenetration + pushOffset);
            // Reflect velocity
            float dotVal = dot({ball.speed_x, ball.speed_y}, bestNormal);
            ball.speed_x -= 2.0f * dotVal * bestNormal.x * wall_speed_factor;
            ball.speed_y -= 2.0f * dotVal * bestNormal.y * wall_speed_factor;
        }

        ball.prev_x = ball.x;
        ball.prev_y = ball.y;

        // Clamp ball speed to max speed
        clamp_ball_speed(ball);
    };

    auto update_balls = [&]()
    {
        // Iterate over all balls and handle movement and collisions
        for (int b = 0; b < max_balls; b++)
        {
            Ball &ball = balls[b];
            if (!ball.alive)
                continue;

            update_ball(ball);
        }
    };

    auto update_bumpers = [&]()
    {
        for (int i = 0; i < max_bumpers; i++) {
            Bumper &bumper = bumpers[i];
            if (bumper.hit_frames > 0) {
                bumper.hit_frames--;
            }
        }
    };

    // Vertical scrolling logic
    auto update_vertical_scroll = [&]()
    {
        if (balls[0].y - vertical_scroll_offset > scroll_padding + ball_size)
        {
            vertical_scroll_offset = balls[0].y - (scroll_padding + ball_size);
            if (vertical_scroll_offset > max_level_height - screen_h) {
                vertical_scroll_offset = max_level_height - screen_h;
            }
        } else if (balls[0].y - vertical_scroll_offset < scroll_padding) {
            vertical_scroll_offset = balls[0].y - scroll_padding;
            if (vertical_scroll_offset < 0) {
                vertical_scroll_offset = 0;
            }
        }
    };

    // Rendering

    // Balls
    auto draw_balls = [&]()
    {
        for (int b = 0; b < max_balls; b++)
        {
            if (balls[b].alive)
            {
                // Calculate screen position with vertical scroll offset
                int ball_screen_x = int(balls[b].x);
                int ball_screen_y = int(screen_h - balls[b].y + vertical_scroll_offset - ball_size);
                if (ball_size == 5)
                {
                    // Ball
                    it.filled_rectangle(ball_screen_x, ball_screen_y+1, ball_size, ball_size-2);
                    it.filled_rectangle(ball_screen_x+1, ball_screen_y, ball_size-2, ball_size);
                    // Top left reflection
                    it.draw_pixel_at(ball_screen_x+1, ball_screen_y+1, COLOR_OFF);
                    it.draw_pixel_at(ball_screen_x+1, ball_screen_y+2, COLOR_OFF);
                    it.draw_pixel_at(ball_screen_x+2, ball_screen_y+1, COLOR_OFF);
                    // Bottom right reflection
                    it.draw_pixel_at(ball_screen_x+3, ball_screen_y+3, COLOR_OFF);
                }
                else {
                    // Larger ball rendering
                    it.filled_circle(ball_screen_x + ball_size / 2, ball_screen_y + ball_size / 2, ball_size / 2);
                }
            }
        }
    };

    auto draw_bumpers = [&]()
    {
        for (int i = 0; i < max_bumpers; i++)
        {
            int bumper_screen_x = int(bumpers[i].x);
            int bumper_screen_y = int(screen_h - bumpers[i].y + vertical_scroll_offset);
            if (bumper_screen_y < -64 || bumper_screen_y > screen_h + 64) {
                continue; // Skip drawing off-screen bumpers
            }
            it.circle(bumper_screen_x, bumper_screen_y, bumpers[i].radius);
            it.circle(bumper_screen_x, bumper_screen_y, bumpers[i].radius-2);

            if (bumpers[i].hit_frames) {
                // Draw inner highlight when hit
                it.filled_circle(bumper_screen_x, bumper_screen_y, bumpers[i].radius+2-bumpers[i].hit_frames);
            }
        }
    };

    auto draw_bounds_and_grid = [&]()
    {
        if (!vertical_scroll_offset)
        {
            // Draw ground line
            it.horizontal_line(0, screen_h - 1, screen_w);
        } else if (vertical_scroll_offset >= max_level_height - screen_h) {
            // Draw ceiling line
            it.horizontal_line(0, 0, screen_w);
        }
        it.vertical_line(0, 0, screen_h);
        it.vertical_line(screen_w - 1, 0, screen_h);

        int grid_spacing = 18;
        for (int y = 0; y < screen_h; y += grid_spacing) {
            int line_y = y + (vertical_scroll_offset % grid_spacing);
            if (line_y >= 0 && line_y < screen_h) {
                for (int x = 0; x < screen_w; x += grid_spacing) {
                    it.draw_pixel_at(x, line_y);
                }
            }
        }
    };

    auto draw_field_triangles = [&]() {
        for (const auto& poly : field_triangles) {
            int n = poly.size();
            for (int i = 0; i < n; i++) {
                int x1 = poly[i].x;
                int y1 = screen_h - poly[i].y + vertical_scroll_offset;
                int x2 = poly[(i + 1) % n].x;
                int y2 = screen_h - poly[(i + 1) % n].y + vertical_scroll_offset;
                it.line(x1, y1, x2, y2);
            }
        }
    };

    // Update
    update_balls();
    update_bumpers();
    update_vertical_scroll();

    // Render
    it.fill(COLOR_OFF);
    it.start_clipping(0, 0, screen_w, screen_h); // Work around SDL scaling bug when drawing outside bounds
    draw_balls();
    draw_bumpers();
    draw_bounds_and_grid();
    draw_field_triangles();


    static float t = 0.0f;
    t += 0.04f; // Adjust speed as needed

    auto draw_paddle = [&](float angle, float paddle_x, float paddle_y, bool right)
    {
        it.filled_circle(paddle_x, paddle_y, paddle_radius);

        // Calculate paddle end point based on angle and length
        float ang = angle;
        if (right) {
            ang = 180.0f - angle;
        }
        float angle_rad = ang * (3.14159265f / 180.0f);
        float end_x = paddle_x + paddle_length * std::cos(angle_rad);
        float end_y = paddle_y + paddle_length * std::sin(angle_rad);
        //it.line(paddle_x, lefzt_paddle_y, end_x, end_y);
        it.line(paddle_x, paddle_y-paddle_radius, end_x, end_y-paddle_radius);
        // it.line(paddle_x, paddle_y-paddle_radius+1, end_x, end_y-paddle_radius+1);
        // it.line(paddle_x, paddle_y-paddle_radius+2, end_x, end_y-paddle_radius+2);
        // it.line(paddle_x, paddle_y-paddle_radius+3, end_x, end_y-paddle_radius+3);

        it.line(paddle_x, paddle_y+paddle_radius, end_x, end_y);

        // End cap
        it.filled_circle(end_x, end_y-paddle_radius/2, paddle_radius/2);
        
    };

    
    // draw_paddle(left_paddle_angle, paddle_x_offset, screen_h - 20.0f + vertical_scroll_offset, false);
    draw_paddle(right_paddle_angle, screen_w-paddle_x_offset, screen_h - 20.0f + vertical_scroll_offset, true);
    // it.printf(screen_w / 2, -2, id(font_xxs), TextAlign::TOP_CENTER, "LVL%d", 10);
    // it.print(screen_w / 2, -2, id(font_xxs), TextAlign::TOP_CENTER, "Pinball Test");
    it.printf(screen_w - 2, -2, id(font_xxs), TextAlign::TOP_RIGHT, "Scroll: %d", vertical_scroll_offset);
    // it.printf(0, 0, id(font_xxs), TextAlign::TOP_LEFT, "x:%.1f y:%.1f", balls[0].x, balls[0].y);
