#include "Harpoon.h"
#include "Unit.h" // For playerDiver, fishes, sfx_shoot, etc.
#include <cmath>
#include <cstdlib>
#include <iostream>

Harpoon::Harpoon() {
    init();
}

void Harpoon::init() {
    x = 0; y = 0;
    speed = 12.0f;
    range = 400.0f;
    distance_traveled = 0;
    dir = RIGHT;
    state = READY;
    caughtFish = nullptr;
    struggle_timer = 0;
    struggle_max_time = 0;
    current_presses = 0;
    required_presses = 0;
    angle = 0;
    aim_sweep_speed = 0;
    aim_timer = 0;
}

bool Harpoon::check_collision(Fish& f) {
    // Simple box collision for the harpoon tip (approx 10x5)
    if (x >= f.x - f.width/2 && x <= f.x + f.width/2) {
        if (y >= f.y - f.height/2 && y <= f.y + f.height/2) {
            return true;
        }
    }
    return false;
}

void Harpoon::fire() {
    if (state == READY && !playerDiver.is_dead) {
        state = AIMING;
        x = playerDiver.x;
        y = playerDiver.y;
        dir = playerDiver.last_horizontal_facing; 
        
        angle = 0; 
        aim_sweep_speed = 2.0f; 
        aim_timer = 3.0f; 
        
        game_speed = 0.1f; 
    }
}

void Harpoon::release() {
    if (state == AIMING) {
        state = FIRING;
        distance_traveled = 0;
        caughtFish = nullptr;
        
        game_speed = 1.0f;
        if (sfx_shoot && sfxEnabled) al_play_sample(sfx_shoot, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
}

void Harpoon::mash_b() {
    if (state == STRUGGLING) {
        current_presses++;
        std::cout << "Mashed B! Presses: " << current_presses << "/" << required_presses << std::endl;
    } else {
        std::cout << "Mashed B but state is " << state << std::endl;
    }
}

void Harpoon::update() {
    if (state == READY) {
        x = playerDiver.x;
        y = playerDiver.y;
        return;
    }

    // AIMING
    if (state == AIMING) {
        dir = playerDiver.last_horizontal_facing;
        angle += aim_sweep_speed * (1.0f / FPS);
        
        float limit = 0.78f; 
        if (angle > limit) {
            angle = limit;
            aim_sweep_speed *= -1;
        } else if (angle < -limit) {
            angle = -limit;
            aim_sweep_speed *= -1;
        }

        aim_timer -= (1.0f / FPS); 
        if (aim_timer <= 0) {
            release();
        }
        
        x = playerDiver.x;
        y = playerDiver.y;
    }

    // FIRING
    else if (state == FIRING) {
        float dx = 0, dy = 0;
        
        float forward_speed = speed * cos(angle);
        float vertical_speed = speed * sin(angle);

        if (dir == RIGHT) {
            dx = forward_speed;
            dy = vertical_speed;
        } else { 
            dx = -forward_speed;
            dy = vertical_speed; 
        }

        x += dx;
        y += dy;
        distance_traveled += speed;

        // Rock check added
        if (distance_traveled >= range || 
            x < 0 || x > MAP_W ||
            y < WATER_LEVEL || y > MAP_H ||
            check_rock_collision(x, y)) {
            
            state = RETRACTING;
        }

        for (auto &f : fishes) {
            if (f.active && !f.caught && check_collision(f)) {
                
                // HIT LOGIC
                f.hp--;
                if (f.hp > 0) {
                    // Not caught yet, just hit
                    state = RETRACTING;
                    // Optional: Play hit sound?
                    break;
                }

                // CAUGHT!
                f.caught = true;
                caughtFish = &f;

                bool isStruggling = false;
                int chance = rand() % 100;

                if (f.type == FISH_SHARK) {
                    isStruggling = true; // 100%
                } else if (f.type == FISH_DEEP_SEA) {
                    if (chance < 70) isStruggling = true; // 70%
                } else if (f.type == FISH_ANGLER) {
                    if (chance < 50) isStruggling = true; // 50%
                } else if (f.type == FISH_BIG) {
                    if (chance < 50) isStruggling = true; // 50%
                } else if (f.type == FISH_SMALL || f.type == FISH_JELLY) {
                    if (chance < 20) isStruggling = true; // 20%
                }

                if (isStruggling) {
                    state = STRUGGLING;
                    current_presses = 0;
                    
                    // TIME LIMIT: 3.0s for everyone
                    struggle_max_time = 3.0f;

                    if (f.type == FISH_SHARK) {
                        required_presses = 15; 
                    } else if (f.type == FISH_DEEP_SEA) {
                        required_presses = 12; 
                    } else if (f.type == FISH_BIG || f.type == TREASURE || f.type == FISH_ANGLER) {
                        required_presses = 9; 
                    } else {
                        // Small Fish & Jellyfish
                        required_presses = 6;
                    }
                    struggle_timer = struggle_max_time;
                } else {
                    state = RETRACTING;
                }
                break;
            }
        }
    }
    // STRUGGLING
    else if (state == STRUGGLING) {
        struggle_timer -= 1.0f / FPS;

        if (current_presses >= required_presses) {
            state = RETRACTING; 
        }
        else if (struggle_timer <= 0) {
            if (caughtFish) {
                caughtFish->caught = false;
                caughtFish->active = true;
                caughtFish->speed = (rand() % 2 == 0 ? 10.0f : -10.0f); 
                caughtFish->vy = (rand() % 2 == 0 ? 5.0f : -5.0f);
                caughtFish = nullptr;
            }
            state = RETRACTING; 
        }

        if (caughtFish) {
            caughtFish->x = x;
            caughtFish->y = y;
            caughtFish->x += (rand() % 5 - 2);
            caughtFish->y += (rand() % 5 - 2);
        }
    }
    // RETRACTING
    else if (state == RETRACTING) {
        float dx = playerDiver.x - x;
        float dy = playerDiver.y - y;
        float dist = sqrt(dx*dx + dy*dy);

        if (dist > speed) {
            x += (dx / dist) * speed;
            y += (dy / dist) * speed;
        } else {
            state = READY;
            
            if (caughtFish != nullptr) {
                playerDiver.score += caughtFish->value;
                caughtFish->active = false; 
                caughtFish->caught = false;
                caughtFish = nullptr;
            }
        }

        if (caughtFish != nullptr) {
            caughtFish->x = x;
            caughtFish->y = y;
        }
    }
}