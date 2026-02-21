#include "Game.h"
#include "Unit.h"
#include <iostream>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_acodec.h>

// Define globals declared in GameConfig.h
float game_speed = 1.0f;
GameState gameState = STATE_MENU;
GameState previousState = STATE_MENU;
float masterVolume = 1.0f;
GameStage currentStage = STAGE_NORMAL;
int difficultyLevel = 1;

// Define globals declared in Unit.h
Diver playerDiver;
Harpoon currentHarpoon;
ALLEGRO_SAMPLE* bgm_game = nullptr;
ALLEGRO_SAMPLE* bgm_menu = nullptr;
ALLEGRO_SAMPLE_INSTANCE* bgm_game_instance = nullptr;
ALLEGRO_SAMPLE_INSTANCE* bgm_menu_instance = nullptr;
ALLEGRO_SAMPLE_INSTANCE* current_bgm_instance = nullptr;
ALLEGRO_SAMPLE* sfx_shoot = nullptr;
ALLEGRO_SAMPLE* sfx_select = nullptr;
bool musicEnabled = true;
bool sfxEnabled = true;
bool showFPS = false;
float chest_timer = 0.0f;
float hold_d_timer = 0.0f;
char rewardMessage[64] = "";
float rewardTimer = 0.0f;

// Define globals declared in other headers
// fishes is defined in Fish.cpp
// gameMap is defined in Map.cpp
Shop gameShop;
LeaderboardSystem gameLeaderboard;
std::vector<ScoreEntry> leaderboard; 
// treasures is defined in Treasure.cpp

// Define globals used in Draw.cpp
std::string inputName = "";
float dive_timer = 60.0f;
float loading_timer = 0.0f;
GameStage next_stage = STAGE_NORMAL;

// Renderer instance
// Renderer gameRenderer; // Defined in Draw.cpp

// Implement init_game_objects and reset_game
// init_treasures, spawn_treasure, get_nearby_treasure moved to Treasure.cpp

void init_game_objects() {
    // playerDiver.init(); // Assuming Diver has init or constructor does it
    // currentHarpoon.init();
    init_map();
    for(int i=0; i<15; i++) spawn_fish(); // Initial spawn loop
    // init_treasures(); // Handled by init_map
    gameShop.init();
}

void reset_game() {
    // playerDiver.reset();
    playerDiver.x = SCREEN_W/2;
    playerDiver.y = WATER_LEVEL;
    playerDiver.oxygen = playerDiver.max_oxygen;
    playerDiver.is_dead = false;
    
    currentHarpoon.state = READY;
    
    dive_timer = 60.0f;
    currentStage = STAGE_NORMAL;
    
    init_treasures();
    // Reset fishes?
    fishes.clear();
    for(int i=0; i<10; i++) spawn_fish();
}

Game::Game() : playerDiver(::playerDiver), gameRenderer(::gameRenderer), display(nullptr), timer(nullptr), event_queue(nullptr), running(false), redraw(false) {}

Game::~Game() {}

bool Game::init() {
    if (!al_init()) return false;
    if (!al_install_keyboard()) return false;
    if (!al_install_mouse()) return false;
    if (!al_init_image_addon()) return false;
    if (!al_init_primitives_addon()) return false;
    if (!al_init_font_addon()) return false;
    if (!al_init_ttf_addon()) return false;
    if (!al_install_audio()) return false;
    if (!al_init_acodec_addon()) return false;

    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    display = al_create_display(SCREEN_W, SCREEN_H);
    if (!display) return false;

    timer = al_create_timer(1.0 / FPS);
    event_queue = al_create_event_queue();

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());

    gameRenderer.init();
    load_audio();
    init_game_objects();
    gameLeaderboard.load();

    al_start_timer(timer);
    running = true;
    return true;
}

void Game::load_audio() {
    al_reserve_samples(10);
    
    bgm_game = al_load_sample("game_bgm.mp3");
    bgm_menu = al_load_sample("main_menu_bgm.mp3");
    sfx_shoot = al_load_sample("harpoon.mp3");
    sfx_select = al_load_sample("select_fixed.wav");
    if (!sfx_select) {
        printf("Failed to load select_fixed.wav, trying select.wav\n");
        sfx_select = al_load_sample("select.wav");
    }
    if (!sfx_select) printf("Failed to load select sound\n");
    
    if (bgm_game) bgm_game_instance = al_create_sample_instance(bgm_game);
    if (bgm_menu) bgm_menu_instance = al_create_sample_instance(bgm_menu);
    
    if (bgm_game_instance) al_attach_sample_instance_to_mixer(bgm_game_instance, al_get_default_mixer());
    if (bgm_menu_instance) al_attach_sample_instance_to_mixer(bgm_menu_instance, al_get_default_mixer());
    
    if (bgm_menu_instance) {
        al_set_sample_instance_playmode(bgm_menu_instance, ALLEGRO_PLAYMODE_LOOP);
        al_play_sample_instance(bgm_menu_instance);
        current_bgm_instance = bgm_menu_instance;
    }
}

void Game::cleanup() {
    gameRenderer.cleanup();
    if (timer) al_destroy_timer(timer);
    if (display) al_destroy_display(display);
    if (event_queue) al_destroy_event_queue(event_queue);
    
    if (bgm_game) al_destroy_sample(bgm_game);
    if (bgm_menu) al_destroy_sample(bgm_menu);
    if (sfx_shoot) al_destroy_sample(sfx_shoot);
    if (sfx_select) al_destroy_sample(sfx_select);
    if (bgm_game_instance) al_destroy_sample_instance(bgm_game_instance);
    if (bgm_menu_instance) al_destroy_sample_instance(bgm_menu_instance);
}

void Game::run() {
    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            update();
            redraw = true;
        } else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } else if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
            al_acknowledge_resize(display);
        } else {
            process_input(ev);
        }

        if (redraw && al_is_event_queue_empty(event_queue)) {
            render();
            redraw = false;
        }
    }
}

void Game::process_input(ALLEGRO_EVENT& ev) {
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
        if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            if (gameState == STATE_PLAYING) {
                gameState = STATE_MENU;
                if (current_bgm_instance) al_stop_sample_instance(current_bgm_instance);
                if (bgm_menu_instance) {
                    al_play_sample_instance(bgm_menu_instance);
                    current_bgm_instance = bgm_menu_instance;
                }
            }
            else if (gameState == STATE_MENU) running = false;
        }
    }
    
    if (gameState == STATE_PLAYING) {
        // DEAD INPUT
        if (playerDiver.is_dead) {
            if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                int mx = ev.mouse.x;
                int my = ev.mouse.y;
                // MAIN MENU BUTTON: x = SCREEN_W/2 - 100, y = SCREEN_H/2 + 20, w = 200, h = 60
                if (mx >= SCREEN_W/2 - 100 && mx <= SCREEN_W/2 + 100 && 
                    my >= SCREEN_H/2 + 20 && my <= SCREEN_H/2 + 80) {
                    
                    if (sfxEnabled && sfx_select) al_play_sample(sfx_select, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    
                    reset_game();
                    gameState = STATE_MENU;
                    
                    if (current_bgm_instance) al_stop_sample_instance(current_bgm_instance);
                    if (bgm_menu_instance) {
                        al_play_sample_instance(bgm_menu_instance);
                        current_bgm_instance = bgm_menu_instance;
                    }
                }
            }
            return;
        }

        // SHOP INPUT
        if (gameShop.isOpen) {
            if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_S || ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    gameShop.isOpen = false;
                }
                else if (ev.keyboard.keycode == ALLEGRO_KEY_1) gameShop.try_buy(0, *this);
                else if (ev.keyboard.keycode == ALLEGRO_KEY_2) gameShop.try_buy(1, *this);
                else if (ev.keyboard.keycode == ALLEGRO_KEY_3) gameShop.try_buy(2, *this);
            }
            return; // Block other inputs
        }

        // Mouse firing removed to restore original controls
        
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int mx = ev.mouse.x;
            int my = ev.mouse.y;
            // SETTINGS BUTTON: x = SCREEN_W - 210, y = 10, w = 200, h = 60
            if (mx >= SCREEN_W - 210 && mx <= SCREEN_W - 10 && my >= 10 && my <= 70) {
                if (sfxEnabled && sfx_select) al_play_sample(sfx_select, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                previousState = STATE_PLAYING;
                gameState = STATE_SETTINGS;
            }
        }

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
                gameShop.isOpen = true;
            }
            if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                 if (currentHarpoon.state == READY) currentHarpoon.fire();
                 else if (currentHarpoon.state == AIMING) currentHarpoon.release();
            }
            if (ev.keyboard.keycode == ALLEGRO_KEY_O) {
                Treasure* t = get_nearby_treasure(playerDiver.x, playerDiver.y);
                if (t) {
                    chest_timer = 3.0f; // Start opening
                }
            }
            if (ev.keyboard.keycode == ALLEGRO_KEY_R) {
                if (playerDiver.y <= WATER_LEVEL + 60 && currentStage == STAGE_NORMAL) {
                    // Return to Surface -> Leaderboard
                    gameState = STATE_LEADERBOARD;
                    
                    // Save Score
                    gameLeaderboard.add(inputName, playerDiver.score);
                    gameLeaderboard.save();
                    
                    // Stop Game Music, Play Menu Music
                    if (current_bgm_instance) al_stop_sample_instance(current_bgm_instance);
                    if (bgm_menu_instance) {
                        al_play_sample_instance(bgm_menu_instance);
                        current_bgm_instance = bgm_menu_instance;
                    }
                }
            }
            if (ev.keyboard.keycode == ALLEGRO_KEY_B) {
                currentHarpoon.mash_b();
            }
        }
    } else if (gameState == STATE_MENU) {
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
             int mx = ev.mouse.x;
             int my = ev.mouse.y;
             if (mx >= SCREEN_W/2 - 150 && mx <= SCREEN_W/2 + 150) {
                 if (my >= 200 && my <= 280) {
                     if (sfxEnabled && sfx_select) {
                         al_play_sample(sfx_select, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                     }
                     gameState = STATE_NAME_INPUT; 
                     inputName = "";
                 }
                 else if (my >= 300 && my <= 380) {
                     if (sfxEnabled && sfx_select) {
                         al_play_sample(sfx_select, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                     }
                     gameState = STATE_LEADERBOARD;
                 }
                 else if (my >= 400 && my <= 480) {
                     if (sfxEnabled && sfx_select) {
                         al_play_sample(sfx_select, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                     }
                     gameState = STATE_INSTRUCTIONS;
                 }
             }
        }
    } else if (gameState == STATE_INSTRUCTIONS || gameState == STATE_LEADERBOARD) {
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int mx = ev.mouse.x;
            int my = ev.mouse.y;
            if (mx >= SCREEN_W/2 - 75 && mx <= SCREEN_W/2 + 75 && my >= 500 && my <= 590) {
                if (sfxEnabled && sfx_select) al_play_sample(sfx_select, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                gameState = STATE_MENU;
            }
        }
    } else if (gameState == STATE_NAME_INPUT) {
        if (ev.type == ALLEGRO_EVENT_KEY_CHAR) {
            if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                if (inputName.length() > 0) {
                    gameState = STATE_BOAT;
                    dive_timer = 60.0f; // Reset timer
                    playerDiver.x = SCREEN_W / 2; // Center diver on boat
                    
                    // Keep Menu Music Playing in Boat Scene
                    if (current_bgm_instance != bgm_menu_instance) {
                        if (current_bgm_instance) al_stop_sample_instance(current_bgm_instance);
                        if (bgm_menu_instance) {
                            al_play_sample_instance(bgm_menu_instance);
                            current_bgm_instance = bgm_menu_instance;
                        }
                    }
                }
            } else if (ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
                if (inputName.length() > 0) inputName.pop_back();
            } else {
                if (inputName.length() < 10) {
                    // Only allow printable characters
                    if (ev.keyboard.unichar >= 32 && ev.keyboard.unichar <= 126) {
                        inputName += (char)ev.keyboard.unichar;
                    }
                }
            }
        }
    } else if (gameState == STATE_BOAT) {
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int mx = ev.mouse.x;
            int my = ev.mouse.y;
            
            // SETTINGS BUTTON (Top Right)
            // x = SCREEN_W - 230, y = 20, w = 210, h = 90
            if (mx >= SCREEN_W - 230 && mx <= SCREEN_W - 20 && my >= 20 && my <= 110) {
                if (sfxEnabled && sfx_select) al_play_sample(sfx_select, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                previousState = STATE_BOAT;
                gameState = STATE_SETTINGS;
            }

            // MAIN MENU BUTTON (Top Left)
            // x = 20, y = 20, w = 240, h = 90
            if (mx >= 20 && mx <= 260 && my >= 20 && my <= 110) {
                if (sfxEnabled && sfx_select) al_play_sample(sfx_select, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                gameState = STATE_MENU;
                if (current_bgm_instance) al_stop_sample_instance(current_bgm_instance);
                if (bgm_menu_instance) {
                    if (musicEnabled) al_play_sample_instance(bgm_menu_instance);
                    current_bgm_instance = bgm_menu_instance;
                }
            }
        }
    } else if (gameState == STATE_SETTINGS) {
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int mx = ev.mouse.x;
            int my = ev.mouse.y;
            
            // Volume Bar: x = SCREEN_W/2 - 150, y = 80, w = 300, h = 30
            if (mx >= SCREEN_W/2 - 150 && mx <= SCREEN_W/2 + 150 && my >= 80 && my <= 110) {
                masterVolume = (float)(mx - (SCREEN_W/2 - 150)) / 300.0f;
                if (masterVolume < 0) masterVolume = 0;
                if (masterVolume > 1) masterVolume = 1;
                
                // Update mixer gain
                al_set_mixer_gain(al_get_default_mixer(), masterVolume);
            }
            
            float btnW = 300;
            float btnH = 70;
            float btnX = SCREEN_W/2 - btnW/2;

            // MUSIC TOGGLE: y = 140
            if (mx >= btnX && mx <= btnX + btnW && my >= 140 && my <= 140 + btnH) {
                musicEnabled = !musicEnabled;
                if (sfxEnabled && sfx_select) al_play_sample(sfx_select, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                
                if (!musicEnabled) {
                    if (current_bgm_instance) al_set_sample_instance_playing(current_bgm_instance, false);
                } else {
                    if (current_bgm_instance) al_set_sample_instance_playing(current_bgm_instance, true);
                }
            }

            // SFX TOGGLE: y = 220
            if (mx >= btnX && mx <= btnX + btnW && my >= 220 && my <= 220 + btnH) {
                sfxEnabled = !sfxEnabled;
                // Only play sound if we just turned it ON (or if it was already on, but we just toggled it... wait. 
                // If I click to turn OFF, sfxEnabled becomes false. So no sound. Correct.
                // If I click to turn ON, sfxEnabled becomes true. So sound. Correct.
                if (sfxEnabled && sfx_select) al_play_sample(sfx_select, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
            }

            // SHOW FPS: y = 300
            if (mx >= btnX && mx <= btnX + btnW && my >= 300 && my <= 300 + btnH) {
                showFPS = !showFPS;
                if (sfxEnabled && sfx_select) al_play_sample(sfx_select, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
            }

            // BACK BUTTON: x = SCREEN_W/2 - 75, y = 480, w = 150, h = 90
            if (mx >= SCREEN_W/2 - 75 && mx <= SCREEN_W/2 + 75 && my >= 480 && my <= 570) {
                if (sfxEnabled && sfx_select) al_play_sample(sfx_select, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                gameState = previousState;
            }
        }
        // Handle dragging for volume
        if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
            if (ev.mouse.pressure > 0 || (ev.mouse.button & 1)) { // If dragging
                 int mx = ev.mouse.x;
                 int my = ev.mouse.y;
                 if (mx >= SCREEN_W/2 - 150 && mx <= SCREEN_W/2 + 150 && my >= 80 && my <= 110) {
                    masterVolume = (float)(mx - (SCREEN_W/2 - 150)) / 300.0f;
                    if (masterVolume < 0) masterVolume = 0;
                    if (masterVolume > 1) masterVolume = 1;
                    al_set_mixer_gain(al_get_default_mixer(), masterVolume);
                 }
            }
        }
    }
}

void Game::update() {
    if (gameState == STATE_PLAYING) {
        // PAUSE IF SHOP IS OPEN
        if (gameShop.isOpen) return;

        ALLEGRO_KEYBOARD_STATE ks;
        al_get_keyboard_state(&ks);
        playerDiver.update(
            al_key_down(&ks, ALLEGRO_KEY_UP),
            al_key_down(&ks, ALLEGRO_KEY_DOWN),
            al_key_down(&ks, ALLEGRO_KEY_LEFT),
            al_key_down(&ks, ALLEGRO_KEY_RIGHT)
        );
        currentHarpoon.update();
        update_fish(); // Singular from Fish.h
        update_camera(); // From Map.h
        
        // Update treasures
        if (chest_timer > 0) {
            chest_timer -= 1.0f/FPS;
            if (chest_timer <= 0) {
                Treasure* t = get_nearby_treasure(playerDiver.x, playerDiver.y);
                if (t) {
                    t->open(playerDiver.score, playerDiver.oxygen, playerDiver.max_oxygen, rewardMessage);
                    rewardTimer = 2.0f;
                }
            }
        }
        if (rewardTimer > 0) rewardTimer -= 1.0f/FPS;
        
        dive_timer -= 1.0f / FPS;
        if (dive_timer <= 0) {
            // Time up logic
            playerDiver.is_dead = true;
            // gameState = STATE_GAME_OVER; // Or handle death
        }

        // Portal Collision
        if (currentStage == STAGE_NORMAL && gameMap.darkPortal.active) {
            // Widen collision box
            if (playerDiver.x > gameMap.darkPortal.x - gameMap.darkPortal.w/2 - 50 && 
                playerDiver.x < gameMap.darkPortal.x + gameMap.darkPortal.w/2 + 50 &&
                playerDiver.y > gameMap.darkPortal.y - gameMap.darkPortal.h/2 - 50 && 
                playerDiver.y < gameMap.darkPortal.y + gameMap.darkPortal.h/2 + 50) {
                
                // Enter Dark Ocean
                gameState = STATE_LOADING;
                loading_timer = 2.0f;
                next_stage = STAGE_DARK_OCEAN;
            }
        }
        else if (currentStage == STAGE_DARK_OCEAN && gameMap.returnPortal.active) {
            if (playerDiver.x > gameMap.returnPortal.x - gameMap.returnPortal.w/2 - 50 && 
                playerDiver.x < gameMap.returnPortal.x + gameMap.returnPortal.w/2 + 50 &&
                playerDiver.y > gameMap.returnPortal.y - gameMap.returnPortal.h/2 - 50 && 
                playerDiver.y < gameMap.returnPortal.y + gameMap.returnPortal.h/2 + 50) {
                
                // Return to Surface
                gameState = STATE_LOADING;
                loading_timer = 2.0f;
                next_stage = STAGE_NORMAL;
            }
        }
    }
    else if (gameState == STATE_BOAT) {
        ALLEGRO_KEYBOARD_STATE ks;
        al_get_keyboard_state(&ks);
        
        // Boat Movement
        bool moving = false;
        if (al_key_down(&ks, ALLEGRO_KEY_LEFT)) {
             playerDiver.x -= 2.0f;
             playerDiver.last_horizontal_facing = LEFT;
             moving = true;
        }
        if (al_key_down(&ks, ALLEGRO_KEY_RIGHT)) {
             playerDiver.x += 2.0f;
             playerDiver.last_horizontal_facing = RIGHT;
             moving = true;
        }
        playerDiver.is_moving = moving;

        // Clamp to boat deck
        if (playerDiver.x < SCREEN_W/2 - 140) playerDiver.x = SCREEN_W/2 - 140;
        if (playerDiver.x > SCREEN_W/2 + 140) playerDiver.x = SCREEN_W/2 + 140;

        if (al_key_down(&ks, ALLEGRO_KEY_D)) {
            float boatLeft = SCREEN_W/2 - 150;
            bool atEdge = (playerDiver.x <= boatLeft + 60 || playerDiver.x >= boatLeft + 300 - 60);
            
            if (atEdge) {
                hold_d_timer += 1.0f/FPS;
                if (hold_d_timer >= 2.0f) {
                    gameState = STATE_PLAYING;
                    playerDiver.x = SCREEN_W/2;
                    playerDiver.y = WATER_LEVEL + 50;
                    hold_d_timer = 0;

                    // Switch to Game Music
                    if (current_bgm_instance) al_stop_sample_instance(current_bgm_instance);
                    if (bgm_game_instance) {
                        al_set_sample_instance_playmode(bgm_game_instance, ALLEGRO_PLAYMODE_LOOP);
                        if (musicEnabled) al_play_sample_instance(bgm_game_instance);
                        current_bgm_instance = bgm_game_instance;
                    }
                }
            } else {
                hold_d_timer = 0;
            }
        } else {
            hold_d_timer = 0;
        }
    }
    else if (gameState == STATE_LOADING) {
        loading_timer -= 1.0f/FPS;
        if (loading_timer <= 0) {
            currentStage = next_stage;
            gameState = STATE_PLAYING;
            
            // Re-init map for new stage
            init_map();
            // Re-spawn fish for new stage
            fishes.clear();
            for(int i=0; i<15; i++) spawn_fish();

            // Reset position based on stage
            if (currentStage == STAGE_DARK_OCEAN) {
                playerDiver.x = 150; // Above return portal
                playerDiver.y = WATER_LEVEL + 100;
            } else {
                playerDiver.x = MAP_W - 150; // Center of portal
                playerDiver.y = MAP_H - 350; // Above the portal and its frame
            }
            update_camera();
        }
    }
}

void Game::render() {
    switch (gameState) {
        case STATE_MENU: gameRenderer.draw_menu(); break;
        case STATE_PLAYING: gameRenderer.draw_game(*this); break;
        case STATE_SETTINGS: gameRenderer.draw_settings(); break;
        case STATE_INSTRUCTIONS: gameRenderer.draw_instructions(); break;
        case STATE_LEADERBOARD: gameRenderer.draw_leaderboard(); break;
        case STATE_NAME_INPUT: gameRenderer.draw_name_input(inputName); break;
        case STATE_LOADING: gameRenderer.draw_loading(); break;
        case STATE_BOAT: gameRenderer.draw_boat(*this); break;
        default: break;
    }
}
