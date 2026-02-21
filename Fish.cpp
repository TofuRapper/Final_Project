#include "Unit.h"
#include "GameConfig.h"
#include <cstdlib>
#include <cmath>
#include <iostream>

std::vector<Fish> fishes;

const int MAX_FISH_COUNT = 70;

Fish::Fish() {
    x = 0; y = 0;
    speed = 0; vy = 0;
    move_timer = 0;
    width = 0; height = 0;
    value = 0;
    type = FISH_SMALL;
    active = true;
    caught = false;
    attack_cooldown = 0;
    hp = 1; max_hp = 1;
}

void spawn_fish() {
    if (fishes.size() >= MAX_FISH_COUNT) return;

    Fish f;
    f.active = true;
    f.caught = false;
    f.attack_cooldown = 0; 

    f.vy = (rand() % 20 - 10) / 10.0f;
    f.move_timer = rand() % 60 + 30;
    
    int r = rand() % 1000; 

    // --- DETERMINE TYPE ---
    if (currentStage == STAGE_NORMAL) {
        if (r < 30) { 
            f.type = FISH_SHARK; f.value = 500; f.width = 150; f.height = 70; 
            f.speed = 1.5f; 
            f.vy = 0;
            f.move_timer = 200; 
            f.hp = 3; f.max_hp = 3; // Shark needs 3 hits
        }
        else if (r < 700) { f.type = FISH_SMALL; f.value = 50; f.width = 40; f.height = 30; f.speed = (rand() % 40 + 20) / 10.0f; f.hp = 1; f.max_hp = 1; } 
        else { f.type = FISH_BIG; f.value = 100; f.width = 60; f.height = 40; f.speed = (rand() % 20 + 10) / 10.0f; f.hp = 1; f.max_hp = 1; } 
    } 
    else { 
        if (r < 300) { f.type = FISH_ANGLER; f.value = 300; f.width = 50; f.height = 40; f.speed = (rand() % 15 + 10) / 10.0f; f.hp = 1; f.max_hp = 1; } 
        else if (r < 600) { f.type = FISH_DEEP_SEA; f.value = 200; f.width = 90; f.height = 70; f.speed = (rand() % 25 + 15) / 10.0f; f.hp = 1; f.max_hp = 1; }
        else { f.type = FISH_JELLY; f.value = 150; f.width = 30; f.height = 50; f.speed = (rand() % 10 + 5) / 10.0f; f.vy = (rand() % 30 - 15) / 10.0f; f.hp = 1; f.max_hp = 1; } 
    }

    // --- SPAWN POSITION ---
    bool initial_spawn = (fishes.size() < 20); 

    // Standard Fish Spawn Logic
    int attempts = 0;
    bool validSpot = false;
    
    while(!validSpot && attempts < 10) {
        if (initial_spawn) {
            f.x = rand() % MAP_W;
            f.speed = (rand() % 2 == 0) ? abs(f.speed) : -abs(f.speed);
        } else {
            bool spawnRight = rand() % 2 == 0;
            if (spawnRight) { f.x = camera_x + SCREEN_W + 50; f.speed = -abs(f.speed); } 
            else { f.x = camera_x - 50; f.speed = abs(f.speed);  }
        }

            if (f.x < 0) f.x = 0; 
            if (f.x > MAP_W) f.x = MAP_W;
            
            f.y = (rand() % (MAP_H - WATER_LEVEL - 100)) + WATER_LEVEL + 50;
            
            float hw = f.width / 2.0f;
            float hh = f.height / 2.0f;

            if (!check_rock_collision(f.x, f.y) &&
                !check_rock_collision(f.x - hw, f.y - hh) && 
                !check_rock_collision(f.x + hw, f.y - hh) &&
                !check_rock_collision(f.x - hw, f.y + hh) &&
                !check_rock_collision(f.x + hw, f.y + hh)) {
                validSpot = true;
            }
            attempts++;
        }
    
    if (validSpot) {
        fishes.push_back(f);
    }
}

void Fish::update() {
    if (!active) return;

    if (!caught) {
        // ===========================
        //       SHARK & ANGLER AI LOGIC
        // ===========================
        if (type == FISH_SHARK || type == FISH_ANGLER) {
            float dx = playerDiver.x - x;
            float dy = playerDiver.y - y;
            float dist_to_player = sqrt(dx*dx + dy*dy);
            
            float aggro_range = (type == FISH_SHARK) ? 200.0f : 150.0f;
            float chase_speed = (type == FISH_SHARK) ? 4.5f : 3.0f;
            float retreat_speed = (type == FISH_SHARK) ? 3.0f : 2.0f;
            int retreat_time = (type == FISH_SHARK) ? 100 : 60;

            // 1. CHECK IF RETREATING (Cooldown active)
            if (attack_cooldown > 0) {
                // RETREAT MODE: Move AWAY from player
                float angle = atan2(dy, dx);
                
                // Note the negative (-) sign to move away
                speed = -cos(angle) * retreat_speed; 
                vy = -sin(angle) * retreat_speed;
                
                attack_cooldown--; // Count down
            }
            // 2. CHECK IF ATTACKING
            else if (dist_to_player < aggro_range) {
                // ATTACK MODE
                
                // If VERY close to player, Trigger Retreat Logic next frame
                if (dist_to_player < 50.0f) {
                    attack_cooldown = retreat_time; 
                }

                float angle = atan2(dy, dx);
                
                speed = cos(angle) * chase_speed; 
                vy = sin(angle) * chase_speed;
                
                move_timer = 9999; // Keep chasing while in range
            } 
            // 3. WANDER MODE
            else {
                if (move_timer > 500) {
                    move_timer = 0; // Reset timer to pick new wander direction
                }
            }
        }

        // --- PHYSICS & MOVEMENT ---
        if (speed != 0 || vy != 0) {
            float next_x = x + (speed * game_speed);
            float next_y = y + (vy * game_speed);
            float hw = width / 2.0f;
            float hh = height / 2.0f;

            // Collision with rocks (Check 4 corners)
            bool colX = check_rock_collision(next_x - hw, y - hh) || 
                        check_rock_collision(next_x + hw, y - hh) ||
                        check_rock_collision(next_x - hw, y + hh) ||
                        check_rock_collision(next_x + hw, y + hh);
            
            if (colX) {
                speed *= -1; 
            } else {
                x = next_x; 
            }

            bool colY = check_rock_collision(x - hw, next_y - hh) || 
                        check_rock_collision(x + hw, next_y - hh) ||
                        check_rock_collision(x - hw, next_y + hh) ||
                        check_rock_collision(x + hw, next_y + hh);

            if (colY) {
                vy *= -1; 
            } else {
                y = next_y; 
            }

            // General Fish Timer (Only for wandering fish)
            move_timer--;
            
            if (move_timer <= 0) { 
                if (type == FISH_SHARK) {
                    // Shark specific wander
                    speed = (rand() % 2 == 0 ? 1.5f : -1.5f);
                    vy = (rand() % 20 - 10) / 20.0f; 
                    move_timer = rand() % 120 + 60; 
                } else {
                    // Normal fish wander
                    vy = (rand() % 30 - 15) / 10.0f;
                    move_timer = rand() % 90 + 30;
                    if (rand() % 10 == 0) speed += (rand() % 10 - 5) / 10.0f; 
                }
            }

            // Map Bounds
            if (y < WATER_LEVEL + 20) { y = WATER_LEVEL + 20; vy = abs(vy); }
            if (y > MAP_H - 20) { y = MAP_H - 20; vy = -abs(vy); }
            
            if (x < 20) { x = 20; speed = abs(speed); }
            if (x > MAP_W - 20) { x = MAP_W - 20; speed = -abs(speed); }
        }
    }

    if (type != TREASURE && type != FISH_SHARK && !caught) {
        if (x < camera_x - 1000 || x > camera_x + SCREEN_W + 1000) {
            if (x < -100 || x > MAP_W + 100) {
                active = false;
            }
        }
    }
}

void update_fish() {
    if (fishes.size() < 20) spawn_fish(); 
    else if (fishes.size() < MAX_FISH_COUNT) {
        if (rand() % 40 == 0) spawn_fish(); 
    }

    for (auto &f : fishes) {
        f.update();
    }

    for (size_t i = 0; i < fishes.size(); i++) {
        if (!fishes[i].active) {
            fishes.erase(fishes.begin() + i);
            i--;
        }
    }
}