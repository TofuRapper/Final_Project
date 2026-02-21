#ifndef DRAW_H
#define DRAW_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <string>
#include "GameConfig.h"

class Game; // Forward declaration

class Renderer {
public:
    ALLEGRO_BITMAP* img_diver1;
    ALLEGRO_BITMAP* img_diver2;
    ALLEGRO_BITMAP* img_diver3;
    ALLEGRO_BITMAP* img_diver4;
    ALLEGRO_BITMAP* img_diver5;
    ALLEGRO_BITMAP* img_diver6;
    ALLEGRO_BITMAP* img_diver7;
    ALLEGRO_BITMAP* img_diver8;
    ALLEGRO_BITMAP* img_diver9;
    ALLEGRO_BITMAP* img_fish_small;
    ALLEGRO_BITMAP* img_fish_big;
    ALLEGRO_BITMAP* img_treasure;
    ALLEGRO_BITMAP* img_harpoon;
    ALLEGRO_BITMAP* img_shark;
    ALLEGRO_BITMAP* img_angler;
    ALLEGRO_BITMAP* img_jelly;
    ALLEGRO_BITMAP* img_deep_sea_fish;
    ALLEGRO_BITMAP* img_carbon_fins;
    ALLEGRO_BITMAP* img_oxygen_tank;
    ALLEGRO_BITMAP* img_boat;
    ALLEGRO_BITMAP* img_button;
    ALLEGRO_FONT* draw_font;

    Renderer();
    void init();
    void cleanup();
    void draw_game(Game& game);
    void draw_menu();
    void draw_settings();
    void draw_instructions();
    void draw_leaderboard();
    void draw_name_input(std::string currentName);
    void draw_loading();
    void draw_boat(Game& game);
    void draw_ui_button(float x, float y, float w, float h, const char* text);
};

extern Renderer gameRenderer;

// Legacy globals for compatibility (if needed, but we should try to remove them)
// extern ALLEGRO_BITMAP* img_diver1; // ...


#endif