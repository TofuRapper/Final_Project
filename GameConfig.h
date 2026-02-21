#ifndef GAMECONFIG_H
#define GAMECONFIG_H

// --- CONFIGURATION ---
const int SCREEN_W = 800;
const int SCREEN_H = 600;
const int FPS = 60;

const int MAP_W = 3000; 
const int MAP_H = 1500;
const int WATER_LEVEL = 100;

// --- ENUMS ---
enum ItemType { FISH_SMALL, FISH_BIG, TREASURE, FISH_ANGLER, FISH_JELLY, FISH_SHARK, FISH_DEEP_SEA }; 
enum Direction { LEFT, RIGHT, UP, DOWN }; 
enum HarpoonState { READY, AIMING, FIRING, RETRACTING, STRUGGLING }; 
enum GameStage { STAGE_NORMAL, STAGE_DARK_OCEAN };

// ADDED: STATE_LOADING, STATE_BOAT, STATE_LEADERBOARD, STATE_NAME_INPUT
enum GameState { STATE_MENU, STATE_SETTINGS, STATE_PLAYING, STATE_LOADING, STATE_GAME_OVER, STATE_DARK_OCEAN, STATE_BOAT, STATE_LEADERBOARD, STATE_NAME_INPUT, STATE_INSTRUCTIONS };

// --- GLOBALS ---
// All globals moved to Game class
extern float game_speed; 
extern GameState gameState; 
extern float masterVolume;  
extern GameStage currentStage; 
extern int difficultyLevel; // 0=Easy, 1=Normal, 2=Hard 

#endif