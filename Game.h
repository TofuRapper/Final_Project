#ifndef GAME_H
#define GAME_H

#include "GameConfig.h"
#include "Diver.h"
#include "Draw.h"
#include "Shop.h"
#include "Leaderboard.h"
#include "Map.h"
#include "Fish.h"
#include "Harpoon.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>

class Game {
public:
    Game();
    ~Game();

    bool init();
    void run();
    void cleanup();

    // Public members accessed by other classes (references to globals)
    Diver& playerDiver;
    Renderer& gameRenderer;
    
private:
    ALLEGRO_DISPLAY* display;
    ALLEGRO_TIMER* timer;
    ALLEGRO_EVENT_QUEUE* event_queue;
    bool running;
    bool redraw;

    void process_input(ALLEGRO_EVENT& ev);
    void update();
    void render();
    void load_audio();
};

#endif
