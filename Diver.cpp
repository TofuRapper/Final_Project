#include "Unit.h" // Includes Diver.h, Fish.h, Map.h, GameConfig.h
#include <cmath>
#include <algorithm> // For std::max/min

Diver::Diver() {
    init();
}

void Diver::init() {
    x = MAP_W / 2.0f;
    y = (float)WATER_LEVEL + 50;
    score = 0;
    speed = 5.0f;
    oxygen = 2000.0f;
    max_oxygen = 2000.0f;
    facing = RIGHT;
    last_horizontal_facing = RIGHT;
    is_dead = false;
    hit_timer = 0.0f;
    is_moving = false;
}

float get_dist(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

// Helper: Check if point is inside a rectangle (for rocks)
bool check_rock_collision(float x, float y) {
    // Diver dimensions (approx)
    float dw = 40; 
    float dh = 60;

    for (const auto& d : decorations) {
        if (d.type == 2 || d.type == 3) { // Solid rocks
            float left = d.x - d.w/2;
            float right = d.x + d.w/2;
            float top = d.y - d.h + 15; // Allow sinking 15px into the visual top
            float bottom = d.y;
            
            // FIX: For Big Rocks (Type 3), extend collision downwards to match drawing
            if (d.type == 3) {
                bottom = MAP_H + 1000; // Extend to bottom of map
            }
            
            // Check overlap between Diver Box and Rock Box
            // Diver Box: [x-dw/2, x+dw/2] x [y-dh/2, y+dh/2]
            // Rock Box: [left, right] x [top, bottom]
            
            if (x + dw/2 > left && x - dw/2 < right &&
                y + dh/2 > top && y - dh/2 < bottom) {
                return true;
            }
        }
    }
    return false;
}

void Diver::update(bool k_up, bool k_down, bool k_left, bool k_right) {
    // 0. CHECK HARPOON STATE
    if (currentHarpoon.state != READY) {
        is_moving = false;
        return; // Cannot move while aiming/firing
    }

    // 1. MOVEMENT LOGIC
    float dx = 0, dy = 0;
    float moveSpeed = speed;
    
    is_moving = (k_up || k_down || k_left || k_right);

    if (k_up) dy -= moveSpeed;
    if (k_down) dy += moveSpeed;
    if (k_left) { dx -= moveSpeed; facing = LEFT; last_horizontal_facing = LEFT; }
    if (k_right) { dx += moveSpeed; facing = RIGHT; last_horizontal_facing = RIGHT; }

    // Check future position for rocks
    if (!check_rock_collision(x + dx, y)) {
        x += dx;
    }
    if (!check_rock_collision(x, y + dy)) {
        y += dy;
    }

    // Map Boundaries
    if (x < 0) x = 0;
    if (x > MAP_W) x = MAP_W;
    // Keep whole body underwater (assuming height ~60px, so center needs to be +30)
    if (y < WATER_LEVEL + 30) y = WATER_LEVEL + 30; 
    
    // Check seabed collision
    float groundY = getSolidSurfaceY(x);
    
    // Only snap to ground if we are close to it (prevent teleporting to top of rocks from side)
    // If we are significantly below the "ground" (rock top), we assume we are at the side.
    // In that case, we just clamp to the absolute seabed.
    if (y > groundY - 30) {
        // If we are colliding with a rock (wall), check_rock_collision should have stopped us.
        // But if we are here, we might be "inside" the vertical column of a rock.
        // If we are DEEP below the top, we shouldn't snap up.
        
        // Check if we are actually inside a rock right now (which shouldn't happen if collision works)
        // If we are not inside a rock, but below groundY, it means we are in open water below a rock overhang?
        // Or we are just at the bottom.
        
        // Let's trust check_rock_collision for walls.
        // For floor, we only snap if we are ABOVE the rock top or slightly below it.
        
        if (y - (groundY - 30) < 20.0f) { // Reduced snap tolerance
             y = groundY - 30;
        } else {
            // We are deep below the reported surface.
            // This happens when we are at the side of a tall rock.
            // getSolidSurfaceY returns the top of that rock.
            // We don't want to snap up.
            
            // Instead, check against the absolute floor (sand)
            float absoluteFloor = MAP_H - 50;
            if (y > absoluteFloor - 30) y = absoluteFloor - 30;
        }
    }

    // 2. INVINCIBILITY TIMER
    if (hit_timer > 0) {
        hit_timer -= 1.0f / FPS; // Count down
    }

    // 3. SHARK/JELLYFISH COLLISION DAMAGE
    // Only check if we are NOT currently invincible
    if (hit_timer <= 0) {
        for (const auto& f : fishes) {
            if (!f.active || f.caught) continue;

            // Simple Box Collision
            if (x > f.x - f.width/2 && x < f.x + f.width/2 &&
                y > f.y - f.height/2 && y < f.y + f.height/2) {
                
                if (f.type == FISH_SHARK) {
                    // --- SHARK HIT ---
                    oxygen -= 200; // Big Damage
                    hit_timer = 2.0f; // 2 Seconds Immunity
                    
                    // Knockback
                    float pushDir = (x < f.x) ? -1.0f : 1.0f;
                    x += pushDir * 50; 
                } 
                else if (f.type == FISH_ANGLER) {
                    // --- ANGLER HIT ---
                    oxygen -= 100; // Medium Damage
                    hit_timer = 1.5f; // 1.5 Seconds Immunity
                    
                    // Knockback
                    float pushDir = (x < f.x) ? -1.0f : 1.0f;
                    x += pushDir * 30; 
                }
                else if (f.type == FISH_JELLY) {
                    // --- JELLY HIT ---
                    oxygen -= 50;
                    hit_timer = 1.0f; // 1 Second Immunity
                }
            }
        }
    }

    // 4. OXYGEN DRAIN
    // Always drain oxygen when underwater
    float drainRate = 0.5f;
    
    oxygen -= drainRate; 

    // 5. DEATH CHECK
    if (oxygen <= 0) {
        oxygen = 0;
        is_dead = true;
    }
}