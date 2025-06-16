// src/Menu.cpp
#include "menu.hpp"
#include <allegro5/allegro_color.h>

menu::menu(ALLEGRO_FONT* f, int w, int h)
    : font(f), width(w), height(h), selectedOption(0)
{
    options = { "Start Game", "View High Scores", "Exit" };
}

menu::~menu() {
    // font is managed externally; no deletion here
}

void menu::processInput(ALLEGRO_EVENT& event) {
    if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event.keyboard.keycode) {
            case ALLEGRO_KEY_UP:
                selectedOption = (selectedOption - 1 + options.size()) % options.size();
                break;
            case ALLEGRO_KEY_DOWN:
                selectedOption = (selectedOption + 1) % options.size();
                break;
            default:
                break;
        }
    }
}

void menu::draw() const {
    al_clear_to_color(al_map_rgb(0, 0, 0));

    for (size_t i = 0; i < options.size(); ++i) {
        ALLEGRO_COLOR color = (i == selectedOption) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
        al_draw_text(font, color, width / 2, 150 + i * 50, ALLEGRO_ALIGN_CENTER, options[i].c_str());
    }

    al_flip_display();
}

int menu::getSelectedOption() const {
    return selectedOption;
}
