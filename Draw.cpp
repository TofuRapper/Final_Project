#include "Draw.h" // Include Draw.h first to see Renderer class
#include "GameConfig.h" 
#include "Unit.h" 
#include "Shop.h" 
#include "Leaderboard.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <iostream>
#include <cmath>
#include <vector>

extern std::string inputName; // Added extern for name tag
extern float dive_timer; // Added extern for dive timer

Renderer gameRenderer;

Renderer::Renderer() {
    img_diver1 = nullptr; 
    img_diver2 = nullptr; 
    img_diver3 = nullptr; 
    img_diver4 = nullptr; 
    img_diver5 = nullptr; 
    img_diver6 = nullptr; 
    img_diver7 = nullptr; 
    img_diver8 = nullptr; 
    img_diver9 = nullptr; 
    img_fish_small = nullptr;
    img_fish_big = nullptr;
    img_treasure = nullptr;
    img_harpoon = nullptr;
    img_shark = nullptr; 
    img_angler = nullptr;
    img_jelly = nullptr;
    img_deep_sea_fish = nullptr;
    img_boat = nullptr;
    img_button = nullptr;
    draw_font = nullptr;
}

void Renderer::init() {
    std::cout << "Loading images..." << std::endl;
    img_diver1 = al_load_bitmap("diver1.png");
    img_diver2 = al_load_bitmap("diver2.png");
    img_diver3 = al_load_bitmap("diver3.png");
    img_diver4 = al_load_bitmap("diver4.png"); 
    img_diver5 = al_load_bitmap("diver5.png"); 
    img_diver6 = al_load_bitmap("diver6.png"); 
    img_diver7 = al_load_bitmap("diver7.png"); 
    img_diver8 = al_load_bitmap("diver8.png"); 
    img_diver9 = al_load_bitmap("diver9.png"); 
    img_fish_small = al_load_bitmap("fish_small.png");
    img_fish_big = al_load_bitmap("fish_big.png");
    img_treasure = al_load_bitmap("treasure.png");
    img_harpoon = al_load_bitmap("harpoon.png");
    img_shark = al_load_bitmap("shark.png");
    img_angler = al_load_bitmap("angler.png");
    img_jelly = al_load_bitmap("jelly.png");
    img_deep_sea_fish = al_load_bitmap("deep_sea_fish.png");
    img_carbon_fins = al_load_bitmap("carbon_fins.png");
    img_oxygen_tank = al_load_bitmap("oxygen.png");
    img_boat = al_load_bitmap("boat.png");
    img_button = al_load_bitmap("button.png");
    draw_font = al_create_builtin_font();
    if (!img_diver1) std::cout << "Warning: diver1.png not found." << std::endl;
    if (!img_boat) std::cout << "Warning: boat.png not found." << std::endl;
    if (!img_button) std::cout << "Warning: button.png not found." << std::endl;
    if (!img_deep_sea_fish) std::cout << "Warning: deep_sea_fish.png not found." << std::endl;
    if (!img_carbon_fins) std::cout << "Warning: carbon_fins.png not found." << std::endl;
    if (!img_oxygen_tank) std::cout << "Warning: oxygen.png not found." << std::endl;
}

void Renderer::cleanup() {
    if (img_diver1) al_destroy_bitmap(img_diver1);
    if (img_diver2) al_destroy_bitmap(img_diver2);
    if (img_diver3) al_destroy_bitmap(img_diver3);
    if (img_diver4) al_destroy_bitmap(img_diver4);
    if (img_diver5) al_destroy_bitmap(img_diver5);
    if (img_diver6) al_destroy_bitmap(img_diver6);
    if (img_diver7) al_destroy_bitmap(img_diver7);
    if (img_diver8) al_destroy_bitmap(img_diver8);
    if (img_diver9) al_destroy_bitmap(img_diver9);
    if (img_fish_small) al_destroy_bitmap(img_fish_small);
    if (img_fish_big) al_destroy_bitmap(img_fish_big);
    if (img_treasure) al_destroy_bitmap(img_treasure);
    if (img_harpoon) al_destroy_bitmap(img_harpoon);
    if (img_shark) al_destroy_bitmap(img_shark);
    if (img_angler) al_destroy_bitmap(img_angler);
    if (img_jelly) al_destroy_bitmap(img_jelly);
    if (img_deep_sea_fish) al_destroy_bitmap(img_deep_sea_fish);
    if (img_carbon_fins) al_destroy_bitmap(img_carbon_fins);
    if (img_oxygen_tank) al_destroy_bitmap(img_oxygen_tank);
    if (img_boat) al_destroy_bitmap(img_boat);
    if (img_button) al_destroy_bitmap(img_button);
    if (draw_font) al_destroy_font(draw_font);
}

// Helper function to draw buttons with hover effect
void Renderer::draw_ui_button(float x, float y, float w, float h, const char* text) {

    // Check hover
    ALLEGRO_MOUSE_STATE ms;
    al_get_mouse_state(&ms);
    
    bool hovered = (ms.x >= x && ms.x <= x + w && ms.y >= y && ms.y <= y + h);
    bool clicked = hovered && (ms.buttons & 1); // Left mouse button

    float drawY = y;
    ALLEGRO_COLOR tint = al_map_rgb(255, 255, 255);

    if (hovered) {
        if (clicked) {
            drawY += 4; // Push down effect
            tint = al_map_rgb(200, 200, 200); // Darker when pressed
        } else {
            tint = al_map_rgb(255, 255, 220); // Slight yellow/bright tint on hover
        }
    }

    if (img_button) {
        al_draw_tinted_scaled_bitmap(img_button, tint, 0, 0, al_get_bitmap_width(img_button), al_get_bitmap_height(img_button),
            x, drawY, w, h, 0);
    } else {
        al_draw_filled_rectangle(x, drawY, x + w, drawY + h, hovered ? al_map_rgb(0, 150, 255) : al_map_rgb(0, 100, 200));
        al_draw_rectangle(x, drawY, x + w, drawY + h, al_map_rgb(255, 255, 255), 2);
    }

    // Draw Text (Scaled 2x)
    ALLEGRO_TRANSFORM transform, old_transform;
    al_copy_transform(&old_transform, al_get_current_transform()); 
    al_identity_transform(&transform);
    al_scale_transform(&transform, 2.0f, 2.0f); 
    al_use_transform(&transform);

    // Adjust coordinates for scaling
    al_draw_text(draw_font, al_map_rgb(255, 255, 255), (x + w/2)/2.0f, (drawY + h/2 - 10)/2.0f, ALLEGRO_ALIGN_CENTER, text);

    al_use_transform(&old_transform);

    if (hovered && img_harpoon) {
        // Draw harpoon pointing at button from the right
        int hw = al_get_bitmap_width(img_harpoon);
        int hh = al_get_bitmap_height(img_harpoon);
        
        // Scale to be larger (1.2x button height)
        float scale = (h * 1.2f) / hh;
        
        // Calculate position on the right side of the button
        // We want the left tip of the harpoon (assuming point is on left) to overlap the button by 60px
        float half_w = (hw * scale) / 2.0f;
        float draw_x = x + w + half_w - 60.0f;

        // Draw it to the right of the button, shifted down
        al_draw_scaled_rotated_bitmap(img_harpoon, hw/2, hh/2, draw_x, drawY + h/2 + 10, scale, scale, 0, 0);
    }
}

void Renderer::draw_menu() {
    al_clear_to_color(al_map_rgb(0, 20, 40)); 

    ALLEGRO_TRANSFORM transform, old_transform;
    al_copy_transform(&old_transform, al_get_current_transform()); 
    
    al_identity_transform(&transform);
    al_scale_transform(&transform, 5.0f, 5.0f); 
    al_use_transform(&transform);
    
    al_draw_text(draw_font, al_map_rgb(255, 255, 255), (SCREEN_W/2)/5.0f, 150/5.0f, ALLEGRO_ALIGN_CENTER, "DEEP OCEAN DIVER");

    al_use_transform(&old_transform); 

    float btnW = 300;
    float btnH = 80;
    float btnX = SCREEN_W/2 - btnW/2;
    
    float btnY = 200;
    draw_ui_button(btnX, btnY, btnW, btnH, "START GAME");

    float lbY = 300;
    draw_ui_button(btnX, lbY, btnW, btnH, "LEADERBOARD");

    float instY = 400;
    draw_ui_button(btnX, instY, btnW, btnH, "INSTRUCTIONS");

    al_flip_display();
}

void Renderer::draw_instructions() {
    al_clear_to_color(al_map_rgb(0, 20, 40));

    ALLEGRO_TRANSFORM transform, old_transform;
    al_copy_transform(&old_transform, al_get_current_transform()); 

    // Title
    al_identity_transform(&transform);
    al_scale_transform(&transform, 2.0f, 2.0f); 
    al_use_transform(&transform);
    al_draw_text(draw_font, al_map_rgb(255, 255, 255), (SCREEN_W/2)/2.0f, 40/2.0f, ALLEGRO_ALIGN_CENTER, "INSTRUCTIONS");

    // Text Body
    al_identity_transform(&transform);
    al_scale_transform(&transform, 1.4f, 1.4f); 
    al_use_transform(&transform);

    float scale = 1.4f;
    float centerX = (SCREEN_W/2) / scale;
    float startY = 130 / scale;
    float lineH = 35; 

    int i = 0;
    al_draw_text(draw_font, al_map_rgb(200, 200, 255), centerX, startY + (i++ * lineH), ALLEGRO_ALIGN_CENTER, "ARROW KEYS: Move Diver");
    al_draw_text(draw_font, al_map_rgb(200, 200, 255), centerX, startY + (i++ * lineH), ALLEGRO_ALIGN_CENTER, "SPACE: Shoot Harpoon");
    al_draw_text(draw_font, al_map_rgb(200, 200, 255), centerX, startY + (i++ * lineH), ALLEGRO_ALIGN_CENTER, "O: Open Treasure Chests");
    al_draw_text(draw_font, al_map_rgb(200, 200, 255), centerX, startY + (i++ * lineH), ALLEGRO_ALIGN_CENTER, "D: Dive from Boat");
    al_draw_text(draw_font, al_map_rgb(200, 200, 255), centerX, startY + (i++ * lineH), ALLEGRO_ALIGN_CENTER, "R: Return to Surface");
    al_draw_text(draw_font, al_map_rgb(200, 200, 255), centerX, startY + (i++ * lineH), ALLEGRO_ALIGN_CENTER, "Collect Treasures and Fish for Score!");
    al_draw_text(draw_font, al_map_rgb(255, 100, 100), centerX, startY + (i++ * lineH), ALLEGRO_ALIGN_CENTER, "Watch your Oxygen Level!");

    al_use_transform(&old_transform);

    float btnW = 150;
    float btnH = 90;
    float btnX = SCREEN_W/2 - btnW/2;
    float btnY = 500;
    draw_ui_button(btnX, btnY, btnW, btnH, "BACK");

    al_flip_display();
}

void Renderer::draw_settings() {
    al_clear_to_color(al_map_rgb(0, 10, 20)); 
    al_draw_text(draw_font, al_map_rgb(255, 255, 255), SCREEN_W/2, 30, ALLEGRO_ALIGN_CENTER, "SETTINGS");

    float barX = SCREEN_W/2 - 150;
    float barY = 80;
    float barW = 300;
    float barH = 30;

    al_draw_text(draw_font, al_map_rgb(255, 255, 255), SCREEN_W/2, barY - 25, ALLEGRO_ALIGN_CENTER, "MASTER VOLUME");
    al_draw_filled_rectangle(barX, barY, barX + barW, barY + barH, al_map_rgb(50, 50, 50));
    al_draw_filled_rectangle(barX, barY, barX + (barW * masterVolume), barY + barH, al_map_rgb(0, 255, 0));
    al_draw_rectangle(barX, barY, barX + barW, barY + barH, al_map_rgb(255, 255, 255), 2);
    al_draw_textf(draw_font, al_map_rgb(255, 255, 255), SCREEN_W/2, barY + 8, ALLEGRO_ALIGN_CENTER, "%.0f%%", masterVolume * 100);

    float btnW = 300;
    float btnH = 70;
    float btnX = SCREEN_W/2 - btnW/2;

    // MUSIC TOGGLE
    float musY = 140;
    char musText[32]; sprintf(musText, "MUSIC: %s", musicEnabled ? "ON" : "OFF");
    draw_ui_button(btnX, musY, btnW, btnH, musText);

    // SFX TOGGLE
    float sfxY = 220;
    char sfxText[32]; sprintf(sfxText, "SFX: %s", sfxEnabled ? "ON" : "OFF");
    draw_ui_button(btnX, sfxY, btnW, btnH, sfxText);

    // SHOW FPS 
    float fpsY = 300;
    char fpsText[32]; sprintf(fpsText, "SHOW FPS: %s", showFPS ? "ON" : "OFF");
    draw_ui_button(btnX, fpsY, btnW, btnH, fpsText);

    float backY = 480;
    draw_ui_button(SCREEN_W/2 - 75, backY, 150, 90, "BACK");

    al_flip_display();
}

void Renderer::draw_leaderboard() {
    al_clear_to_color(al_map_rgb(0, 10, 20)); 
    
    // Scale Title
    ALLEGRO_TRANSFORM transform, old_transform;
    al_copy_transform(&old_transform, al_get_current_transform()); 
    al_identity_transform(&transform);
    al_scale_transform(&transform, 2.0f, 2.0f); 
    al_use_transform(&transform);

    al_draw_text(draw_font, al_map_rgb(255, 255, 255), (SCREEN_W/2)/2.0f, 50/2.0f, ALLEGRO_ALIGN_CENTER, "LEADERBOARD");

    al_use_transform(&old_transform);
    
    // Use transform to scale text
    al_copy_transform(&old_transform, al_get_current_transform()); 
    al_identity_transform(&transform);
    al_scale_transform(&transform, 2.0f, 2.0f); // 2x Scale
    al_use_transform(&transform);

    float startY = 100;
    for (size_t i = 0; i < gameLeaderboard.entries.size(); i++) {
        ALLEGRO_COLOR color = al_map_rgb(255, 255, 255);
        if (i == 0) color = al_map_rgb(255, 215, 0); // Gold
        else if (i == 1) color = al_map_rgb(192, 192, 192); // Silver
        else if (i == 2) color = al_map_rgb(205, 127, 50); // Bronze

        al_draw_textf(draw_font, color, (SCREEN_W/2)/2.0f, (startY + i * 40)/2.0f, ALLEGRO_ALIGN_CENTER, 
            "%d. %s - %d", (int)i + 1, gameLeaderboard.entries[i].name.c_str(), gameLeaderboard.entries[i].score);
    }

    al_use_transform(&old_transform);

    float backY = 500;
    draw_ui_button(SCREEN_W/2 - 75, backY, 150, 90, "BACK");

    al_flip_display();
}

void Renderer::draw_name_input(std::string currentName) {

    al_clear_to_color(al_map_rgb(0, 0, 0));
    
    al_draw_text(draw_font, al_map_rgb(255, 255, 255), SCREEN_W/2, 200, ALLEGRO_ALIGN_CENTER, "ENTER YOUR NAME:");
    
    // Input Box
    float boxX = SCREEN_W/2 - 150;
    float boxY = 250;
    al_draw_rectangle(boxX, boxY, boxX + 300, boxY + 50, al_map_rgb(255, 255, 255), 2);
    
    // Draw Name (Scaled)
    ALLEGRO_TRANSFORM transform, old_transform;
    al_copy_transform(&old_transform, al_get_current_transform()); 
    al_identity_transform(&transform);
    al_scale_transform(&transform, 2.0f, 2.0f); 
    al_use_transform(&transform);
    
    al_draw_text(draw_font, al_map_rgb(0, 255, 0), (SCREEN_W/2)/2.0f, (boxY + 10)/2.0f, ALLEGRO_ALIGN_CENTER, currentName.c_str());
    
    // FLASHING CURSOR
    if ((int)(al_get_time() * 2) % 2 == 0) {
        int textW = al_get_text_width(draw_font, currentName.c_str());
        float cursorX = (SCREEN_W/2)/2.0f + textW/2.0f + 2;
        float cursorY = (boxY + 10)/2.0f;
        al_draw_line(cursorX, cursorY, cursorX, cursorY + 10, al_map_rgb(0, 255, 0), 2);
    }

    al_use_transform(&old_transform);

    al_draw_text(draw_font, al_map_rgb(150, 150, 150), SCREEN_W/2, 320, ALLEGRO_ALIGN_CENTER, "PRESS ENTER TO SUBMIT");

    al_flip_display();
}

// --- BOAT SCENE ---
void Renderer::draw_boat(Game& game) {
    al_clear_to_color(al_map_rgb(135, 206, 235)); // Sky Blue

    // Draw Water with WAVES
    ALLEGRO_COLOR topColor = al_map_rgb(0, 100, 200); 
    ALLEGRO_COLOR botColor = al_map_rgb(0, 0, 20);    
    
    float time = al_get_time();
    int num_segments = 100; 
    float segment_width = (float)SCREEN_W / num_segments;
    float water_level = SCREEN_H - 200;
    
    std::vector<ALLEGRO_VERTEX> v;
    v.reserve((num_segments + 1) * 2);

    for (int i = 0; i <= num_segments; i++) {
        float x = i * segment_width;
        // Wave calculation
        float wave_y = water_level + sin(x * 0.02 + time * 2.0) * 5.0 + sin(x * 0.05 + time * 1.5) * 2.5;
        
        // Top vertex
        ALLEGRO_VERTEX v_top;
        v_top.x = x; v_top.y = wave_y; v_top.z = 0; v_top.color = topColor;
        v_top.u = 0; v_top.v = 0; 
        v.push_back(v_top);

        // Bottom vertex
        ALLEGRO_VERTEX v_bot;
        v_bot.x = x; v_bot.y = SCREEN_H; v_bot.z = 0; v_bot.color = botColor;
        v_bot.u = 0; v_bot.v = 0;
        v.push_back(v_bot);
    }
    
    al_draw_prim(v.data(), NULL, NULL, 0, v.size(), ALLEGRO_PRIM_TRIANGLE_STRIP);

    // Draw Boat
    float boatW = 300;
    float boatH = 250; 
    float boatCenterX = SCREEN_W/2;
    float boatCenterY = SCREEN_H - 400 + boatH/2;

    // Animation
    float t = al_get_time();
    float rockAngle = sin(t * 1.5) * 0.05; 
    float bobY = sin(t * 2.0) * 10;
    
    float currentBoatY = boatCenterY + bobY;

    if (img_boat) {
        int w = al_get_bitmap_width(img_boat);
        int h = al_get_bitmap_height(img_boat);
        float scaleX = boatW / w;
        float scaleY = boatH / h;
        al_draw_scaled_rotated_bitmap(img_boat, w/2, h/2, boatCenterX, currentBoatY, scaleX, scaleY, rockAngle, 0);
    } else {
        // Fallback to rectangle
        float bx = boatCenterX - boatW/2;
        float by = currentBoatY - boatH/2;
        al_draw_filled_rectangle(bx, by, bx + boatW, by + 100, al_map_rgb(139, 69, 19)); 
        al_draw_rectangle(bx, by, bx + boatW, by + 100, al_map_rgb(100, 50, 0), 3);
        al_draw_text(draw_font, al_map_rgb(0, 0, 0), boatCenterX, by + 40, ALLEGRO_ALIGN_CENTER, "ON THE BOAT");
    }

    // DRAW DIVER ON BOAT
    ALLEGRO_BITMAP* boatDiver = img_diver6; // Default to standing
    if (!boatDiver) boatDiver = img_diver1; // Fallback

    if (playerDiver.is_moving) {
         // double t = al_get_time(); // Already defined
         int frame = (int)(t * 10) % 4; 
         if (frame == 0) boatDiver = (img_diver6 ? img_diver6 : img_diver1);
         else if (frame == 1) boatDiver = (img_diver7 ? img_diver7 : img_diver1);
         else if (frame == 2) boatDiver = (img_diver8 ? img_diver8 : img_diver1);
         else boatDiver = (img_diver9 ? img_diver9 : img_diver1);
    }

    if (boatDiver) {
        int w = al_get_bitmap_width(boatDiver);
        int h = al_get_bitmap_height(boatDiver);
        float scale = 60.0f / w; 
        int flags = (playerDiver.last_horizontal_facing == LEFT) ? ALLEGRO_FLIP_HORIZONTAL : 0;
        
        // Adjust Y to be on deck. 
        // Boat Top is currentBoatY - boatH/2
        // Deck is roughly +160 from top
        float deckY = (currentBoatY - boatH/2) + 160; 
        
        // Simple rotation for diver to match boat? Maybe too complex for now.
        // Just bobbing is fine.
        
        al_draw_scaled_rotated_bitmap(boatDiver, w/2, h/2, playerDiver.x, deckY, scale, scale, rockAngle, flags);
        
        // DRAW NAME TAG
        al_draw_text(draw_font, al_map_rgb(255, 255, 255), playerDiver.x, deckY - 40, ALLEGRO_ALIGN_CENTER, inputName.c_str());

        // HOLD D INDICATOR
        if (hold_d_timer > 0) {
            float pct = hold_d_timer / 2.0f;
            if (pct > 1.0f) pct = 1.0f;
            al_draw_filled_rectangle(playerDiver.x - 50, deckY - 70, playerDiver.x + 50, deckY - 60, al_map_rgb(50, 50, 50));
            al_draw_filled_rectangle(playerDiver.x - 50, deckY - 70, playerDiver.x - 50 + (100 * pct), deckY - 60, al_map_rgb(0, 255, 255));
            al_draw_text(draw_font, al_map_rgb(255, 255, 255), playerDiver.x, deckY - 90, ALLEGRO_ALIGN_CENTER, "DIVING...");
        }
    }

    // SETTINGS BUTTON (Top Right)
    float setX = SCREEN_W - 230;
    float setY = 20;
    draw_ui_button(setX, setY, 210, 90, "SETTINGS");

    // MAIN MENU BUTTON (Top Left)
    float menuX = 20;
    float menuY = 20;
    draw_ui_button(menuX, menuY, 240, 90, "MAIN MENU");

    float boatLeft = boatCenterX - boatW/2;
    bool atEdge = (playerDiver.x <= boatLeft + 60 || playerDiver.x >= boatLeft + boatW - 60);

    if (atEdge) {
        al_draw_text(draw_font, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_H - 50, ALLEGRO_ALIGN_CENTER, "HOLD 'D' TO DIVE");
    }

    al_flip_display();
}

// --- LOADING SCREEN ---
void Renderer::draw_loading() {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    
    ALLEGRO_TRANSFORM transform, old_transform;
    al_copy_transform(&old_transform, al_get_current_transform()); 
    al_identity_transform(&transform);
    al_scale_transform(&transform, 3.0f, 3.0f); 
    al_use_transform(&transform);
    
    // Determine text based on where we are going
    // If currentStage is NORMAL, we are going to DARK OCEAN.
    // If currentStage is DARK OCEAN, we are going to NORMAL.
    const char* text = (currentStage == STAGE_NORMAL) ? "ENTERING DARK OCEAN..." : "RETURNING TO SURFACE...";
    al_draw_text(draw_font, al_map_rgb(255, 255, 255), (SCREEN_W/2)/3.0f, (SCREEN_H/2)/3.0f, ALLEGRO_ALIGN_CENTER, text);
    
    al_use_transform(&old_transform);
    al_flip_display();
}

void Renderer::draw_game(Game& game) {
    // 1. CLEAR SCREEN BACKGROUND
    if (currentStage == STAGE_DARK_OCEAN) {
        al_clear_to_color(al_map_rgb(5, 5, 10)); // Deep dark ocean
    } else {
        al_clear_to_color(al_map_rgb(135, 206, 235)); // Sky blue
    }

    // 2. APPLY CAMERA
    ALLEGRO_TRANSFORM transform;
    al_identity_transform(&transform);
    al_translate_transform(&transform, -camera_x, -camera_y);
    al_use_transform(&transform);

    // 3. DRAW WATER BACKGROUND (GRADIENT)
    if (currentStage == STAGE_DARK_OCEAN) {
         al_draw_filled_rectangle(0, WATER_LEVEL, MAP_W, MAP_H, al_map_rgb(0, 0, 20));
    }
    else {
        // Gradient for Normal Stage with WAVES
        ALLEGRO_COLOR topColor = al_map_rgb(0, 100, 200); 
        ALLEGRO_COLOR botColor = al_map_rgb(0, 0, 20);    
        
        float time = al_get_time();
        int num_segments = 100; 
        float segment_width = (float)MAP_W / num_segments;
        
        std::vector<ALLEGRO_VERTEX> v;
        v.reserve((num_segments + 1) * 2);

        for (int i = 0; i <= num_segments; i++) {
            float x = i * segment_width;
            // Wave calculation: Amplitude 5, Frequency 0.02, Speed 2.0
            float wave_y = WATER_LEVEL + sin(x * 0.02 + time * 2.0) * 5.0 + sin(x * 0.05 + time * 1.5) * 2.5;
            
            // Top vertex
            ALLEGRO_VERTEX v_top;
            v_top.x = x; v_top.y = wave_y; v_top.z = 0; v_top.color = topColor;
            v_top.u = 0; v_top.v = 0; 
            v.push_back(v_top);

            // Bottom vertex
            ALLEGRO_VERTEX v_bot;
            v_bot.x = x; v_bot.y = MAP_H; v_bot.z = 0; v_bot.color = botColor;
            v_bot.u = 0; v_bot.v = 0;
            v.push_back(v_bot);
        }
        
        al_draw_prim(v.data(), NULL, NULL, 0, v.size(), ALLEGRO_PRIM_TRIANGLE_STRIP);
    }
    
    // 4. DRAW SEABED
    ALLEGRO_COLOR rockColor = (currentStage == STAGE_NORMAL) ? al_map_rgb(40, 40, 50) : al_map_rgb(20, 20, 25);
    for (size_t i = 0; i < seabed.size() - 1; i++) {
        float vertices[] = {
            seabed[i].x, seabed[i].y,
            seabed[i+1].x, seabed[i+1].y,
            seabed[i+1].x, (float)MAP_H,
            seabed[i].x, (float)MAP_H
        };
        al_draw_filled_polygon(vertices, 4, rockColor);
    }

    // 5. DRAW AIMING LINE
    if (currentHarpoon.state == AIMING) {
        float aimLength = 100.0f;
        float aimX = 0, aimY = 0;
        float forwardScale = cos(currentHarpoon.angle);
        float upScale = sin(currentHarpoon.angle); 
        
        if (currentHarpoon.dir == RIGHT) { aimX = 100 * forwardScale; aimY = 100 * upScale; }
        else { aimX = -100 * forwardScale; aimY = 100 * upScale; } 

        al_draw_line(playerDiver.x, playerDiver.y, playerDiver.x + aimX, playerDiver.y + aimY, al_map_rgb(255, 0, 0), 2);
        al_draw_circle(playerDiver.x + aimX, playerDiver.y + aimY, 3, al_map_rgb(255, 0, 0), 2);
    }

    // 6. DRAW HARPOON
    if (currentHarpoon.state == FIRING || currentHarpoon.state == RETRACTING || currentHarpoon.state == STRUGGLING) {
        // al_draw_line(playerDiver.x, playerDiver.y, currentHarpoon.x, currentHarpoon.y, al_map_rgb(0,0,0), 2); // Removed head line
        
        float angle = 0;
        if (currentHarpoon.dir == LEFT) angle = ALLEGRO_PI;

        if (img_harpoon) {
            int w = al_get_bitmap_width(img_harpoon);
            int h = al_get_bitmap_height(img_harpoon);
            float scale = 0.15f; // Small size (Reduced from 0.3f)
            
            // Pivot at the tip (Right Center of image)
            float cx = w; 
            float cy = h / 2.0f;

            // Use currentHarpoon.angle if available, otherwise fallback to direction
            float drawAngle = currentHarpoon.angle;
            if (currentHarpoon.dir == LEFT) {
                drawAngle = ALLEGRO_PI - currentHarpoon.angle;
            }

            int flags = 0;
            float rotation = 0;

            // Image points LEFT naturally (Tip at 0, Tail at w)
            if (currentHarpoon.dir == LEFT) {
                cx = 0; // Pivot at Tip
                flags = 0;
                rotation = drawAngle - ALLEGRO_PI;
            } else {
                cx = w; // Pivot at Tip (which is now at w due to flip)
                flags = ALLEGRO_FLIP_HORIZONTAL;
                rotation = drawAngle;
            }

            al_draw_scaled_rotated_bitmap(img_harpoon, cx, cy, currentHarpoon.x, currentHarpoon.y, scale, scale, rotation, flags);

            // Calculate Tail Position for line attachment
            // Since (x,y) is the Tip, the tail is backwards along the vector
            float length = w * scale;
            float tailX = currentHarpoon.x - cos(drawAngle) * length;
            float tailY = currentHarpoon.y - sin(drawAngle) * length;
            
            // Redraw line to tail instead of tip
            al_draw_line(playerDiver.x, playerDiver.y, tailX, tailY, al_map_rgb(0,0,0), 2);
        } else {
            al_draw_filled_circle(currentHarpoon.x, currentHarpoon.y, 5, al_map_rgb(255, 0, 0));
            al_draw_line(playerDiver.x, playerDiver.y, currentHarpoon.x, currentHarpoon.y, al_map_rgb(0,0,0), 2);
        }

        // --- STRUGGLE TEXT (SCALED 3x) ---
        if (currentHarpoon.state == STRUGGLING) {
            float hx = currentHarpoon.x;
            float hy = currentHarpoon.y - 40; 
            
            // SAVE VIEW
            ALLEGRO_TRANSFORM camTrans, textTrans;
            al_copy_transform(&camTrans, al_get_current_transform());

            // ZOOM IN
            al_identity_transform(&textTrans);
            al_scale_transform(&textTrans, 3.0f, 3.0f); // 3x Bigger
            al_use_transform(&textTrans);

            // CALC SCREEN POS
            float screenX = hx - camera_x;
            float screenY = hy - camera_y;

            // DRAW TEXT
            al_draw_text(draw_font, al_map_rgb(255, 0, 0), screenX/3.0f, (screenY - 20)/3.0f, ALLEGRO_ALIGN_CENTER, "PRESS 'B'!");

            // RESTORE VIEW
            al_use_transform(&camTrans);

            // DRAW BARS
            al_draw_filled_rectangle(hx - 20, hy, hx + 20, hy + 6, al_map_rgb(50, 50, 50));
            float pct = (float)currentHarpoon.current_presses / currentHarpoon.required_presses;
            if (pct > 1.0f) pct = 1.0f;
            al_draw_filled_rectangle(hx - 20, hy, hx - 20 + (40 * pct), hy + 6, al_map_rgb(255, 215, 0));
            float timePct = currentHarpoon.struggle_timer / currentHarpoon.struggle_max_time;
            al_draw_filled_rectangle(hx - 20, hy + 8, hx - 20 + (40 * timePct), hy + 12, al_map_rgb(255, 50, 50));
        }
    }

    // 7. DRAW DECORATIONS
    for (const auto& d : decorations) {
        if (d.type == 0) { // Bubble
             al_draw_circle(d.x, d.y, d.w, al_map_rgba(255, 255, 255, 50), 1);
        }
        else if (d.type == 1) { // Seaweed
            // Simple Seaweed
            al_draw_filled_triangle(d.x - 5, d.y, d.x + 5, d.y, d.x, d.y - d.h, al_map_rgb(34, 139, 34));
        }
        else if (d.type == 4) { // Tall Kelp
            // Wavy line
            float time = al_get_time();
            float wave = sin(time * 2.0f + d.x * 0.1f) * 10.0f;
            al_draw_filled_rectangle(d.x - 2, d.y - d.h, d.x + 2, d.y, al_map_rgb(0, 100, 0));
            al_draw_filled_circle(d.x + wave, d.y - d.h, 8, al_map_rgb(0, 120, 0));
        }
        else if (d.type == 5) { // Coral
            // Branching structure (simplified)
            al_draw_filled_rectangle(d.x - 5, d.y - 20, d.x + 5, d.y, al_map_rgb(255, 127, 80)); // Base
            al_draw_filled_circle(d.x, d.y - 25, 10, al_map_rgb(255, 160, 122)); // Top
            al_draw_filled_circle(d.x - 10, d.y - 15, 8, al_map_rgb(255, 160, 122)); // Left
            al_draw_filled_circle(d.x + 10, d.y - 15, 8, al_map_rgb(255, 160, 122)); // Right
        }
        else if (d.type == 2) { // Small Rock
             al_draw_filled_rectangle(d.x - d.w/2, d.y - d.h, d.x + d.w/2, d.y, al_map_rgb(105, 105, 105));
        }
        else if (d.type == 3) { // Big Rock
            ALLEGRO_COLOR boulderColor = (currentStage == STAGE_NORMAL) ? al_map_rgb(80, 80, 80) : al_map_rgb(40, 40, 40);
            al_draw_filled_rectangle(d.x - d.w/2, d.y - d.h, d.x + d.w/2, d.y + 1500, boulderColor);
            al_draw_rectangle(d.x - d.w/2, d.y - d.h, d.x + d.w/2, d.y + 1500, al_map_rgb(60, 60, 60), 2);
        }
    }

    // 8. DRAW PORTAL
    if (currentStage == STAGE_NORMAL && darkPortal.active) {
        float x1 = darkPortal.x - darkPortal.w/2;
        float y1 = darkPortal.y - darkPortal.h/2;
        float x2 = darkPortal.x + darkPortal.w/2;
        
        // Draw Thick Line at Top (Purple for Dark Ocean)
        al_draw_line(x1, y1, x2, y1, al_map_rgb(180, 50, 255), 15);
        // Glow
        al_draw_line(x1, y1, x2, y1, al_map_rgba(180, 50, 255, 100), 30);
    }
    else if (currentStage == STAGE_DARK_OCEAN && returnPortal.active) {
        float x1 = returnPortal.x - returnPortal.w/2;
        float y1 = returnPortal.y - returnPortal.h/2;
        float x2 = returnPortal.x + returnPortal.w/2;

        // Draw Thick Line at Top (Blue/Cyan for Return)
        al_draw_line(x1, y1, x2, y1, al_map_rgb(100, 200, 255), 15);
        // Glow
        al_draw_line(x1, y1, x2, y1, al_map_rgba(100, 200, 255, 100), 30);
    }

    // 9. DRAW FISH
    for (auto &f : fishes) {
        if (!f.active) continue;
        ALLEGRO_BITMAP* spriteToDraw = nullptr;
        switch (f.type) {
            case FISH_SMALL: spriteToDraw = img_fish_small; break;
            case FISH_BIG:   spriteToDraw = img_fish_big; break;
            // TREASURE removed from Fish draw
            case FISH_ANGLER: spriteToDraw = img_angler; break; 
            case FISH_JELLY:  spriteToDraw = img_jelly; break; 
            case FISH_SHARK:  spriteToDraw = img_shark; break;
            case FISH_DEEP_SEA: spriteToDraw = img_deep_sea_fish; break;
        }

        if (spriteToDraw) {
            int w = al_get_bitmap_width(spriteToDraw);
            int h = al_get_bitmap_height(spriteToDraw);
            int flags = (f.speed < 0) ? ALLEGRO_FLIP_HORIZONTAL : 0;
            float angle = f.vy * 0.1f; 
            al_draw_scaled_rotated_bitmap(spriteToDraw, w/2, h/2, f.x, f.y, (float)f.width/w, (float)f.height/h, angle, flags);
        } else {
            if (f.type == FISH_ANGLER) {
                al_draw_filled_circle(f.x, f.y, f.width/2, al_map_rgb(100, 50, 0)); 
                al_draw_line(f.x, f.y, f.x + 20, f.y - 20, al_map_rgb(255, 255, 255), 1);
                al_draw_filled_circle(f.x + 20, f.y - 20, 3, al_map_rgb(255, 255, 0));
            } else if (f.type == FISH_JELLY) {
                al_draw_filled_ellipse(f.x, f.y, f.width/2, f.height/2, al_map_rgba(200, 100, 255, 150));
            } else if (f.type == FISH_SHARK) {
                ALLEGRO_COLOR sharkColor = al_map_rgb(150, 150, 150);
                if (f.speed < 0) al_draw_filled_triangle(f.x + f.width/2, f.y, f.x - f.width/2, f.y - f.height/2, f.x - f.width/2, f.y + f.height/2, sharkColor);
                else al_draw_filled_triangle(f.x - f.width/2, f.y, f.x + f.width/2, f.y - f.height/2, f.x + f.width/2, f.y + f.height/2, sharkColor);
            } else {
                al_draw_filled_ellipse(f.x, f.y, f.width/2, f.height/2, al_map_rgb(255,0,0));
            }
        }

        // Draw Health Bar for Sharks
        if (f.type == FISH_SHARK && f.hp < f.max_hp && f.hp > 0) {
            float barW = 40;
            float barH = 5;
            float barX = f.x - barW / 2;
            float barY = f.y - f.height / 2 - 15;
            
            // Background (Red)
            al_draw_filled_rectangle(barX, barY, barX + barW, barY + barH, al_map_rgb(255, 0, 0));
            
            // Health (Green)
            float hpPct = (float)f.hp / f.max_hp;
            al_draw_filled_rectangle(barX, barY, barX + (barW * hpPct), barY + barH, al_map_rgb(0, 255, 0));
            
            // Border
            al_draw_rectangle(barX, barY, barX + barW, barY + barH, al_map_rgb(0, 0, 0), 1);
        }
    }

    // TREASURES
    for (const auto& t : treasures) {
        if (t.active && !t.isOpened) {
            if (img_treasure) {
                int w = al_get_bitmap_width(img_treasure);
                int h = al_get_bitmap_height(img_treasure);
                al_draw_scaled_bitmap(img_treasure, 0, 0, w, h, t.x - t.width/2, t.y - t.height/2, t.width, t.height, 0);
            } else {
                al_draw_filled_rectangle(t.x - t.width/2, t.y - t.height/2, t.x + t.width/2, t.y + t.height/2, al_map_rgb(255, 215, 0));
            }
        }
    }

    // 10. DRAW DIVER (With Flashing I-Frames)
    ALLEGRO_BITMAP* currentDiverSprite = img_diver1;
    
    // Animation Logic
    if (img_diver1 && img_diver2 && img_diver3 && img_diver4 && img_diver5) {
        double t = al_get_time();
        
        if (playerDiver.is_moving) {
            // Moving: 1 -> 5
            int frame = (int)(t * 5) % 5; 
            if (frame == 0) currentDiverSprite = img_diver1;
            else if (frame == 1) currentDiverSprite = img_diver2;
            else if (frame == 2) currentDiverSprite = img_diver3;
            else if (frame == 3) currentDiverSprite = img_diver4;
            else currentDiverSprite = img_diver5;
        } else {
            // Idle: 2 -> 3 -> 4
            int frame = (int)(t * 3) % 3; 
            if (frame == 0) currentDiverSprite = img_diver2;
            else if (frame == 1) currentDiverSprite = img_diver3;
            else currentDiverSprite = img_diver4;
        }
    }

    if (currentDiverSprite) {
        bool drawIt = true;
        int w = al_get_bitmap_width(currentDiverSprite);
        int h = al_get_bitmap_height(currentDiverSprite);
        float scale = 60.0f / w; 
        int flags = (playerDiver.last_horizontal_facing == LEFT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

        // FLICKER EFFECT
        if (playerDiver.hit_timer > 0) {
            int blink = (int)(playerDiver.hit_timer * 100) % 10;
            if (blink < 5) {
                // Tint Red
                al_draw_tinted_scaled_rotated_bitmap(currentDiverSprite, al_map_rgb(255, 100, 100), 
                    w/2, h/2, 
                    playerDiver.x, playerDiver.y, 
                    scale, scale, 
                    0, 
                    flags);
                drawIt = false; 
            }
        }

        if (drawIt) {
            al_draw_scaled_rotated_bitmap(currentDiverSprite, w/2, h/2, playerDiver.x, playerDiver.y, scale, scale, 0, flags);
        }
        
        // DRAW NAME TAG
        al_draw_text(draw_font, al_map_rgb(255, 255, 255), playerDiver.x, playerDiver.y - 40, ALLEGRO_ALIGN_CENTER, inputName.c_str());

    } else {
        al_draw_filled_rectangle(playerDiver.x - 10, playerDiver.y - 20, playerDiver.x + 10, playerDiver.y + 10, al_map_rgb(255, 165, 0));
        // DRAW NAME TAG (Fallback)
        al_draw_text(draw_font, al_map_rgb(255, 255, 255), playerDiver.x, playerDiver.y - 40, ALLEGRO_ALIGN_CENTER, inputName.c_str());
    }

    // 11. DRAW UI
    al_identity_transform(&transform); // Reset transform
    al_use_transform(&transform);

    // CHEST OPENING BAR
    if (chest_timer > 0) {
        float pct = chest_timer / 3.0f;
        if (pct > 1.0f) pct = 1.0f;
        al_draw_filled_rectangle(SCREEN_W/2 - 50, SCREEN_H/2 - 60, SCREEN_W/2 + 50, SCREEN_H/2 - 50, al_map_rgb(50, 50, 50));
        al_draw_filled_rectangle(SCREEN_W/2 - 50, SCREEN_H/2 - 60, SCREEN_W/2 - 50 + (100 * pct), SCREEN_H/2 - 50, al_map_rgb(255, 215, 0));
        al_draw_text(draw_font, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_H/2 - 80, ALLEGRO_ALIGN_CENTER, "OPENING...");
    }

    // REWARD MESSAGE
    if (rewardTimer > 0) {
        al_draw_text(draw_font, al_map_rgb(255, 255, 0), SCREEN_W/2, SCREEN_H/2 - 100, ALLEGRO_ALIGN_CENTER, rewardMessage);
    }

    // RETURN TO BOAT PROMPT
    if (playerDiver.y <= WATER_LEVEL + 55 && currentStage == STAGE_NORMAL) {
        al_draw_text(draw_font, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_H - 50, ALLEGRO_ALIGN_CENTER, "PRESS 'R' TO RETURN");
    }

    // OPEN TREASURE PROMPT
    Treasure* nearbyT = get_nearby_treasure(playerDiver.x, playerDiver.y);
    if (nearbyT && !nearbyT->isOpened) {
         al_draw_text(draw_font, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_H - 50, ALLEGRO_ALIGN_CENTER, "PRESS 'O' TO OPEN");
    }

    if (currentHarpoon.state == AIMING) {
        float timePct = currentHarpoon.aim_timer / 3.0f;
        al_draw_filled_rectangle(SCREEN_W/2 - 100, SCREEN_H - 50, SCREEN_W/2 + 100, SCREEN_H - 30, al_map_rgb(50, 50, 50));
        al_draw_filled_rectangle(SCREEN_W/2 - 100, SCREEN_H - 50, SCREEN_W/2 - 100 + (200 * timePct), SCREEN_H - 30, al_map_rgb(0, 255, 0));
        al_draw_text(draw_font, al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H - 70, ALLEGRO_ALIGN_CENTER, "AIMING... PRESS SPACE!");
    }

    float oxPct = playerDiver.oxygen / playerDiver.max_oxygen;
    al_draw_filled_rectangle(10, 50, 10 + (200 * oxPct), 70, al_map_rgb(0, 255, 255));
    al_draw_rectangle(10, 50, 210, 70, al_map_rgb(255, 255, 255), 2);
    al_draw_text(draw_font, al_map_rgb(0,0,0), 20, 55, 0, "OXYGEN");

    // SCALE SCORE AND DEPTH
    ALLEGRO_TRANSFORM t_ui, old_t_ui;
    al_copy_transform(&old_t_ui, al_get_current_transform());
    al_identity_transform(&t_ui);
    al_scale_transform(&t_ui, 2.0f, 2.0f);
    al_use_transform(&t_ui);

    al_draw_textf(draw_font, al_map_rgb(255, 255, 255), 10/2.0f, 10/2.0f, 0, "SCORE: %d", playerDiver.score);
    
    // Move DEPTH below Settings button (Settings is y=10 to y=60)
    // So we put DEPTH at y=70 (scaled y=35)
    al_draw_textf(draw_font, al_map_rgb(255, 255, 255), (SCREEN_W - 10)/2.0f, 70/2.0f, ALLEGRO_ALIGN_RIGHT, "DEPTH: %.0fm", (playerDiver.y - WATER_LEVEL) / 25.0f);

    if (showFPS) {
        // Move FPS below DEPTH (y=90, scaled y=45)
        al_draw_text(draw_font, al_map_rgb(0, 255, 0), (SCREEN_W - 10)/2.0f, 90/2.0f, ALLEGRO_ALIGN_RIGHT, "FPS: 60");
    }

    al_use_transform(&old_t_ui);

    // SETTINGS BUTTON (Top Right)
    float setX = SCREEN_W - 210;
    float setY = 10;
    draw_ui_button(setX, setY, 200, 60, "SETTINGS");

    ALLEGRO_MOUSE_STATE ms;
    al_get_mouse_state(&ms);
    float worldMX = ms.x + camera_x;
    float worldMY = ms.y + camera_y;
    al_draw_textf(draw_font, al_map_rgb(255, 255, 0), 10, 90, 0, "MOUSE (W): %.0f, %.0f", worldMX, worldMY);

    if (playerDiver.is_dead) {
        al_draw_filled_rectangle(0, 0, SCREEN_W, SCREEN_H, al_map_rgba(0, 0, 0, 180));
        
        ALLEGRO_TRANSFORM t, old_t;
        al_copy_transform(&old_t, al_get_current_transform());
        al_identity_transform(&t);
        al_scale_transform(&t, 3.0f, 3.0f);
        al_use_transform(&t);
        al_draw_text(draw_font, al_map_rgb(255, 50, 50), (SCREEN_W/2)/3.0f, (SCREEN_H/2 - 100)/3.0f, ALLEGRO_ALIGN_CENTER, "YOU DROWNED!");
        al_use_transform(&old_t);

        draw_ui_button(SCREEN_W/2 - 100, SCREEN_H/2 + 20, 200, 60, "MAIN MENU");
    }

    if (gameShop.isOpen) gameShop.draw(draw_font, game);

    al_flip_display();
}