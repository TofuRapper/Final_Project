#ifndef UNIT_H
#define UNIT_H

// --- THE MASTER HEADER ---
#include "GameConfig.h" 
#include "Diver.h"
#include "Fish.h"
#include "Harpoon.h"
#include "Map.h"
#include "Draw.h"
#include "Treasure.h" // Added Treasure support
#include <allegro5/allegro_audio.h> 

extern Diver playerDiver;
extern Harpoon currentHarpoon;

// General Functions
void init_game_objects();
void reset_game();

// --- AUDIO GLOBALS ---
extern ALLEGRO_SAMPLE* bgm_game;
extern ALLEGRO_SAMPLE* bgm_menu;
extern ALLEGRO_SAMPLE_INSTANCE* bgm_game_instance;
extern ALLEGRO_SAMPLE_INSTANCE* bgm_menu_instance;
extern ALLEGRO_SAMPLE_INSTANCE* current_bgm_instance;
extern ALLEGRO_SAMPLE* sfx_shoot;
extern ALLEGRO_SAMPLE* sfx_select;

extern float masterVolume;
extern bool musicEnabled;
extern bool sfxEnabled;
extern bool showFPS;
extern float chest_timer;
extern float hold_d_timer;

// --- REWARD DISPLAY ---
extern char rewardMessage[64];
extern float rewardTimer;

#endif