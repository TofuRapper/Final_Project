#ifndef GAME_MAP_H
#define GAME_MAP_H

#include <vector>
#include "GameConfig.h"

struct Decoration {
    float x, y;
    float w, h; 
    int type;
};

struct Point { float x, y; };

struct Portal {
    float x, y;
    float w, h; 
    bool active;
};

class GameMap {
public:
    std::vector<Decoration> decorations;
    std::vector<Point> seabed;
    Portal darkPortal;
    Portal returnPortal;
    float camera_x;
    float camera_y;

    GameMap();
    void init();
    void update_camera();
    float getGroundY(float x);
    float getSolidSurfaceY(float x);
    
    // Helpers
    void add_rock(float x, float y, float w, float h);
    void add_grass(float x, float y, int type = 1);
    void add_bubble(float x, float y);
    void add_treasure(float x, float y, int value, int type = 0);
};

extern GameMap gameMap;

// Legacy globals (mapped to gameMap members via macros or references if possible, 
// but for C++ we might need to keep them as references or just update the code)
// To avoid massive refactoring right now, I will keep the globals as references in Map.cpp 
// OR I will update the code to use gameMap.
// Let's update the code. It's cleaner.

extern std::vector<Decoration>& decorations;
extern std::vector<Point>& seabed;
extern Portal& darkPortal;
extern Portal& returnPortal;
extern float& camera_x;
extern float& camera_y;

// Legacy functions
void init_map();
void update_camera();
float getGroundY(float x);
float getSolidSurfaceY(float x);

void init_map();
void update_camera();
float getGroundY(float x);
float getSolidSurfaceY(float x);

#endif