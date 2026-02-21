#include "Treasure.h"
#include <cstdio>
#include <algorithm> 

std::vector<Treasure> treasures;

Treasure::Treasure(float startX, float startY, int val, int tType) {
    x = startX;
    y = startY;
    width = 50;
    height = 50;
    value = val;
    type = tType;
    isOpened = false;
    active = true;
}

void spawn_treasure(float x, float y, int value, int type) {
    treasures.emplace_back(x, y, value, type);
}

void init_treasures() {
    treasures.clear();
    // Add some random treasures (optional, maybe remove if manual placement is enough)
    // spawn_treasure(500, 1400, 100, 0);
    // spawn_treasure(1500, 1450, 200, 0);
    // spawn_treasure(2500, 1400, 150, 0);
    // spawn_treasure(1000, 1400, 0, 1); // Oxygen
}

Treasure* get_nearby_treasure(float diverX, float diverY) {
    for (auto& t : treasures) {
        if (t.active && !t.isOpened) {
            float dx = t.x - diverX;
            float dy = t.y - diverY;
            float dist = dx*dx + dy*dy;
            if (dist < 3600) { // 60 pixels radius
                return &t;
            }
        }
    }
    return nullptr;
}

void Treasure::open(int& score, float& oxygen, float max_oxygen, char* rewardMsg) {
    if (!isOpened) {
        isOpened = true;
        
        if (type == 0) { // Money
            score += value;
            if (rewardMsg) snprintf(rewardMsg, 64, "+%d GOLD", value);
        } else if (type == 1) { // Oxygen
            oxygen += max_oxygen / 2.0f; 
            if (oxygen > max_oxygen) oxygen = max_oxygen;
            if (rewardMsg) snprintf(rewardMsg, 64, "OXYGEN REFILLED!");
        }
    }
}
