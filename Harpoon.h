#ifndef HARPOON_H
#define HARPOON_H

#include "GameConfig.h"

// Forward Declaration
class Fish; 

class Harpoon {
public:
    float x, y;
    float speed;
    float range;
    float distance_traveled;
    Direction dir;
    HarpoonState state; 
    Fish* caughtFish;   

    float struggle_timer;
    float struggle_max_time;
    int current_presses;
    int required_presses;

    float angle;           
    float aim_sweep_speed; 
    float aim_timer;       

    Harpoon(); // Constructor
    void init();
    void fire();    
    void release(); 
    void update();
    void mash_b(); 
    bool check_collision(Fish& f);
};

// Global Instance
extern Harpoon currentHarpoon;

#endif