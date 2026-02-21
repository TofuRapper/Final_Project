#ifndef SHOP_H
#define SHOP_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <string>
#include <vector>

class Game; // Forward declaration

struct Upgrade {
    std::string name;
    std::string description;
    int cost;
    int level;
    int max_level;
    int cost_multiplier; 
};

class Shop {
public:
    bool isOpen;
    std::vector<Upgrade> upgrades;

    Shop();
    void init();
    void draw(ALLEGRO_FONT* font, Game& game);
    void try_buy(int index, Game& game);
};

extern Shop gameShop;



#endif