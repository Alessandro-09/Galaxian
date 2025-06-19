#ifndef MENU_HPP
#define MENU_HPP

#include <vector>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "Init.hpp"  

class Menu {
public:
    Menu(ALLEGRO_FONT* font, int width, int height);
    ~Menu();

    void processInput(ALLEGRO_EVENT& event);
    int getSelectedOption() const;
    void draw() const;
    int run(SystemResources& sys); 

private:
    ALLEGRO_FONT* font;
    int width;
    int height;
    size_t selectedOption;
    std::vector<std::string> options;
    std::vector<std::pair<float, float>> stars;

    void generateStars();
    void updateStars();
};

#endif
