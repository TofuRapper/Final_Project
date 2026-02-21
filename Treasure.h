#ifndef TREASURE_H
#define TREASURE_H

#include "GameConfig.h"
#include <vector>

class Treasure {
public:
    float x, y;
    int width;
    int height;
    int value;
    int type; // 0 = Money, 1 = Oxygen
    bool isOpened;
    bool active;

    Treasure(float x, float y, int value, int type = 0);
    void open(int& score, float& oxygen, float max_oxygen, char* rewardMsg);
};

extern std::vector<Treasure> treasures;

void init_treasures();
void spawn_treasure(float x, float y, int value, int type = 0);
Treasure* get_nearby_treasure(float diverX, float diverY);
// void open_treasure(Treasure* t, int& score, float& oxygen, float max_oxygen); // Removed, use method


#endif
