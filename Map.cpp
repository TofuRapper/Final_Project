#include "Map.h"
#include "Unit.h"
#include "Treasure.h" // Added for manual treasure placement
#include <cstdlib>
#include <cmath>
#include <ctime>

GameMap gameMap;

// References for backward compatibility
std::vector<Decoration>& decorations = gameMap.decorations;
std::vector<Point>& seabed = gameMap.seabed;
Portal& darkPortal = gameMap.darkPortal;
Portal& returnPortal = gameMap.returnPortal;
float& camera_x = gameMap.camera_x;
float& camera_y = gameMap.camera_y;

GameMap::GameMap() {
    camera_x = 0;
    camera_y = 0;
}

void GameMap::add_rock(float x, float y, float w, float h) {
    Decoration d;
    d.type = 3; // Rock
    d.x = x;
    d.y = y;
    d.w = w;
    d.h = h;
    decorations.push_back(d);
}

void GameMap::add_grass(float x, float y, int type) {
    Decoration d;
    d.type = type; // 1 = Triangle Grass, 4 = Kelp, 5 = Coral
    d.x = x;
    d.y = y;
    if (type == 1) { d.w = 0; d.h = 40; }
    else if (type == 4) { d.w = 10; d.h = 80; } // Tall Kelp
    else if (type == 5) { d.w = 40; d.h = 40; } // Coral
    else { d.w = 30; d.h = 20; }
    decorations.push_back(d);
}

void GameMap::add_bubble(float x, float y) {
    Decoration d;
    d.type = 0; // Bubble
    d.x = x;
    d.y = y;
    d.w = (rand() % 6) + 2;
    d.h = 0;
    decorations.push_back(d);
}

void GameMap::add_treasure(float x, float y, int value, int type) {
    spawn_treasure(x, y, value, type);
}

float GameMap::getGroundY(float x) {
    if (seabed.empty()) return MAP_H - 50;
    if (x < 0) x = 0;
    if (x > MAP_W) x = MAP_W;
    
    for (size_t i = 0; i < seabed.size() - 1; i++) {
        if (x >= seabed[i].x && x <= seabed[i+1].x) {
            float t = (x - seabed[i].x) / (seabed[i+1].x - seabed[i].x);
            return seabed[i].y + t * (seabed[i+1].y - seabed[i].y);
        }
    }
    return seabed.back().y;
}

float GameMap::getSolidSurfaceY(float x) {
    float highestY = getGroundY(x);

    for (const auto& d : decorations) {
        if (d.type == 2 || d.type == 3) {
            float left = d.x - d.w/2;
            float right = d.x + d.w/2;
            
            if (x >= left && x <= right) {
                float rockTop = d.y - d.h + 15; // Allow sinking 15px into the visual top
                if (rockTop < highestY) {
                    highestY = rockTop;
                }
            }
        }
    }
    return highestY;
}

void GameMap::init() {
    seabed.clear();
    decorations.clear();
    init_treasures(); // Clear old treasures

    // 1. DEFINE SEABED (Flat bottom for manual placement)
    // We make it deep so we can stack rocks on top
    float floorY = MAP_H - 50;
    seabed.push_back({0, floorY});
    seabed.push_back({(float)MAP_W, floorY});

    // 2. SETUP PORTALS
    darkPortal.x = MAP_W - 150; 
    darkPortal.y = floorY - 30; // Lowered further (Top ~1300)
    darkPortal.w = 140; 
    darkPortal.h = 240; 
    darkPortal.active = true;

    returnPortal.x = 150; 
    returnPortal.y = WATER_LEVEL + 225; // Moved UP to align with rocks (Top ~250)
    returnPortal.w = 100; 
    returnPortal.h = 150; 
    returnPortal.active = true;

    if (currentStage == STAGE_NORMAL) {
        // 3. MANUAL MAP DESIGN (NORMAL)
        // Place rocks from left to right
        
        // Left Wall (Near Return Portal) - Filled to top
        add_rock(50, floorY, 100, MAP_H); 
        // Add rocks under/around return portal
        // Pillar under portal
        add_rock(150, floorY, 150, MAP_H - (WATER_LEVEL + 150 + 75)); 
        // Side rocks for return portal (Top at 250)
        add_rock(80, WATER_LEVEL + 250, 60, 100); // Left side
        add_rock(220, WATER_LEVEL + 250, 60, 100); // Right side

        // Bridge Gap 1
        add_rock(300, floorY, 120, 80);

        // Some random rocks on the floor
        add_rock(300, floorY, 200, 150);
        add_grass(300, floorY - 150, 4); // Tall Kelp
        add_treasure(300, floorY - 150 - 25, 500); // Treasure on top

        // Bridge Gap 2
        add_rock(450, floorY, 150, 100);
        add_grass(450, floorY - 100, 5); // Coral

        add_rock(600, floorY, 150, 300);
        add_grass(600, floorY - 300, 1); // Grass

        // Bridge Gap 3
        add_rock(750, floorY, 150, 120);
        add_grass(750, floorY - 120, 4); // Tall Kelp

        add_rock(900, floorY, 300, 100);
        add_rock(900, floorY - 100, 200, 100); // Stacked rock
        add_grass(900, floorY - 200, 5); // Coral

        // Bridge Gap 4
        add_rock(1100, floorY, 150, 150);

        add_rock(1300, floorY, 250, 400);
        add_treasure(1300, floorY - 400 - 25, 1000);
        add_grass(1300, floorY - 400, 4); // Tall Kelp

        // Bridge Gap 5
        add_rock(1450, floorY, 150, 200);
        add_grass(1450, floorY - 200, 5); // Coral

        add_rock(1600, floorY, 150, 150);

        // Bridge Gap 6
        add_rock(1750, floorY, 200, 100);

        // NEW AREA (Since MAP_W is now 3000)
        add_rock(2000, floorY, 150, 250);
        add_treasure(2000, floorY - 250 - 25, 0, 1); // OXYGEN TANK!
        add_grass(2000, floorY - 250, 4); // Tall Kelp

        add_rock(2200, floorY, 300, 100);
        add_grass(2300, floorY - 100, 5); // Coral

        add_rock(2600, floorY, 150, 400);
        add_treasure(2600, floorY - 400 - 25, 1000);
        add_grass(2600, floorY - 400, 1); // Grass

        // Portal Structure (Manual)
        // Right Wall
        add_rock(MAP_W - 50, floorY, 100, 1000);

        // Portal Frame (Full Frame)
        // Left Pillar
        add_rock(darkPortal.x - 120, darkPortal.y + 100, 100, 300);
        // Right Pillar
        add_rock(darkPortal.x + 120, darkPortal.y + 100, 100, 300);
        // Bottom Threshold (Taller to reach portal line)
        add_rock(darkPortal.x, darkPortal.y + 150, 340, 270);
    } 
    else {
        // 3. MANUAL MAP DESIGN (DARK OCEAN)
        // More open, spiky rocks, different layout
        
        // Left Wall (Near Return Portal) - Filled to top
        add_rock(50, floorY, 100, MAP_H); 
        // Add rocks under/around return portal
        // Pillar under portal
        add_rock(150, floorY, 150, MAP_H - (WATER_LEVEL + 150 + 75)); 
        // Side rocks for return portal (Top at 250)
        add_rock(80, WATER_LEVEL + 250, 60, 100); // Left side
        add_rock(220, WATER_LEVEL + 250, 60, 100); // Right side

        // Spiky floor
        for (int i = 300; i < MAP_W - 300; i += 200) {
            int h = rand() % 300 + 100;
            add_rock(i, floorY, 100, h);
            if (rand() % 2 == 0) add_grass(i, floorY - h, 5); // Coral
        }

        // Floating platforms?
        add_rock(1000, 800, 200, 50);
        add_treasure(1000, 800 - 50 - 25, 500);

        add_rock(2000, 600, 300, 50);
        add_treasure(2000, 600 - 50 - 25, 1000);

        // Right Wall
        add_rock(MAP_W - 50, floorY, 100, 1000);
    }

    // 4. ADD BUBBLES (Randomly placed for atmosphere)
    for (int i = 0; i < 50; i++) {
        add_bubble(rand() % MAP_W, (rand() % (int)(MAP_H - WATER_LEVEL)) + WATER_LEVEL);
    }
}

void GameMap::update_camera() {
    float target_x = playerDiver.x - SCREEN_W / 2;
    float target_y = playerDiver.y - SCREEN_H / 2;

    if (target_x < 0) target_x = 0;
    if (target_x > MAP_W - SCREEN_W) target_x = MAP_W - SCREEN_W;
    if (target_y < 0) target_y = 0;
    if (target_y > MAP_H - SCREEN_H) target_y = MAP_H - SCREEN_H;

    camera_x += (target_x - camera_x) * 0.1f;
    camera_y += (target_y - camera_y) * 0.1f;
}

// Legacy wrappers
void init_map() { gameMap.init(); }
void update_camera() { gameMap.update_camera(); }
float getGroundY(float x) { return gameMap.getGroundY(x); }
float getSolidSurfaceY(float x) { return gameMap.getSolidSurfaceY(x); }
