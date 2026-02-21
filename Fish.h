#ifndef FISH_H
#define FISH_H

#include <vector>
#include "GameConfig.h"

class Fish {
public:
    float x, y;
    float speed; 
    float vy;    
    int move_timer;
    int width;
    int height;
    int value;
    ItemType type;
    bool active;
    bool caught; 
    float attack_cooldown; 
    int hp;      // Added HP
    int max_hp;  // Added Max HP

    Fish(); // Constructor
    void update(); // Update method
};

// Global Vector
extern std::vector<Fish> fishes;

// Functions
void spawn_fish();
void update_fish();

#endif