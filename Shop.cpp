#include "Shop.h"
#include "Game.h"
#include <allegro5/allegro_primitives.h>
#include <cstdio>

Shop::Shop() {
    isOpen = false;
}

void Shop::init() {
    upgrades.clear();
    upgrades.push_back({ "Carbon Fins", "Swim faster underwater.", 100, 1, 5, 50 });
    upgrades.push_back({ "Double Tank", "Increases Max Oxygen by 500.", 150, 1, 5, 75 });
    upgrades.push_back({ "Pneumatic Gun", "Increases harpoon range & speed.", 200, 1, 3, 150 });
}

void Shop::draw(ALLEGRO_FONT* font, Game& game) {
    // Draw Shop Background
    al_draw_filled_rectangle(50, 50, SCREEN_W - 50, SCREEN_H - 50, al_map_rgba(0, 0, 20, 230));
    al_draw_rectangle(50, 50, SCREEN_W - 50, SCREEN_H - 50, al_map_rgb(255, 255, 255), 3);

    ALLEGRO_TRANSFORM t;
    
    // --- DRAW HEADER ---
    al_identity_transform(&t);
    al_scale_transform(&t, 2.0, 2.0); 
    al_use_transform(&t);
    al_draw_text(font, al_map_rgb(255, 215, 0), (SCREEN_W / 2) / 2.0, 70 / 2.0, ALLEGRO_ALIGN_CENTER, "--- DIVE SHOP ---");
    
    // --- DRAW GOLD ---
    al_identity_transform(&t);
    al_scale_transform(&t, 2.0, 2.0);
    al_use_transform(&t);
    al_draw_textf(font, al_map_rgb(255, 255, 255), (SCREEN_W / 2) / 2.0, 110 / 2.0, ALLEGRO_ALIGN_CENTER, "Gold: %d", game.playerDiver.score);

    // --- DRAW ITEMS ---
    int yPos = 180; // Starting Y Position (moved down slightly)

    for (size_t i = 0; i < upgrades.size(); i++) {
        Upgrade& u = upgrades[i];
        
        // Reset transform to draw Icon Box normally
        al_identity_transform(&t);
        al_use_transform(&t);

        // Icon Box
        al_draw_rectangle(80, yPos, 130, yPos + 50, al_map_rgb(255, 255, 255), 2);
        
        // Draw Icon
        if (i == 0 && game.gameRenderer.img_carbon_fins) { 
             al_draw_scaled_bitmap(game.gameRenderer.img_carbon_fins, 0, 0, al_get_bitmap_width(game.gameRenderer.img_carbon_fins), al_get_bitmap_height(game.gameRenderer.img_carbon_fins), 85, yPos + 5, 40, 40, 0);
        } else if (i == 1 && game.gameRenderer.img_oxygen_tank) { 
             al_draw_scaled_bitmap(game.gameRenderer.img_oxygen_tank, 0, 0, al_get_bitmap_width(game.gameRenderer.img_oxygen_tank), al_get_bitmap_height(game.gameRenderer.img_oxygen_tank), 85, yPos + 5, 40, 40, 0);
        } else if (i == 2 && game.gameRenderer.img_harpoon) { 
             al_draw_scaled_bitmap(game.gameRenderer.img_harpoon, 0, 0, al_get_bitmap_width(game.gameRenderer.img_harpoon), al_get_bitmap_height(game.gameRenderer.img_harpoon), 85, yPos + 5, 40, 40, 0);
        }

        ALLEGRO_COLOR color = (game.playerDiver.score >= u.cost && u.level < u.max_level) ? al_map_rgb(0, 255, 0) : al_map_rgb(150, 150, 150);
        if (u.level >= u.max_level) color = al_map_rgb(255, 100, 100);

        // --- DRAW ITEM TITLE (2x BIGGER) ---
        al_identity_transform(&t);
        al_scale_transform(&t, 2.0, 2.0); // Scale 2x (Reduced from 3x to fit)
        al_use_transform(&t);

        // We divide X and Y by 2.0 to account for the zoom
        if (u.level < u.max_level)
            al_draw_textf(font, color, 140 / 2.0, yPos / 2.0, 0, "[%d] %s (Lvl %d) - $%d", (int)i + 1, u.name.c_str(), u.level, u.cost);
        else
            al_draw_textf(font, color, 140 / 2.0, yPos / 2.0, 0, "[%d] %s - MAXED", (int)i + 1, u.name.c_str());

        // --- DRAW DESCRIPTION (1.2x BIGGER) ---
        al_identity_transform(&t);
        al_scale_transform(&t, 1.2, 1.2); // Scale 1.2x (Reduced from 1.5x)
        al_use_transform(&t);
        
        // Position description lower (yPos + 45) so it doesn't overlap the huge title
        float maxWidth = (SCREEN_W - 50 - 140) / 1.2f;
        float lineHeight = al_get_font_line_height(font) + 2;
        al_draw_multiline_text(font, al_map_rgb(200, 200, 200), 140 / 1.2f, (yPos + 45) / 1.2f, maxWidth, lineHeight, 0, u.description.c_str());

        // Increase spacing for next item (was 120, now 100)
        yPos += 100; 
    }

    // Footer Text
    al_identity_transform(&t);
    al_use_transform(&t);
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H - 60, ALLEGRO_ALIGN_CENTER, "Press 1-3 to Buy. Press 'S' to Exit.");
}

void Shop::try_buy(int index, Game& game) {
    if (index < 0 || index >= upgrades.size()) return;

    Upgrade& u = upgrades[index];
    if (u.level < u.max_level && game.playerDiver.score >= u.cost) {
        game.playerDiver.score -= u.cost;
        u.level++;
        u.cost += u.cost_multiplier;


        switch (index) {
            case 0: game.playerDiver.speed += 1.0f; break;        
            case 1: 
                game.playerDiver.max_oxygen += 500.0f; 
                game.playerDiver.oxygen += 500.0f; 
                break; 
            case 2: 
                currentHarpoon.speed += 3.0f;                
                currentHarpoon.range += 100.0f;
                break;
        }
    }
}
