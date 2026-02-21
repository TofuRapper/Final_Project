#ifndef DIVER_H
#define DIVER_H

#include "GameConfig.h"

class Diver {
public:
    float x, y;
    int score;
    float speed;
    float oxygen;
    float max_oxygen;
    Direction facing; 
    Direction last_horizontal_facing;
    bool is_dead;
    
    // NEW: Invincibility Timer
    float hit_timer; 
    bool is_moving; // Added for animation

    Diver();
    void init();
    void update(bool k_up, bool k_down, bool k_left, bool k_right);
};

extern Diver playerDiver;

// Helper functions
bool check_rock_collision(float x, float y);
float get_dist(float x1, float y1, float x2, float y2); 

#endif