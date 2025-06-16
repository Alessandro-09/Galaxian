#ifndef MENU_HPP
#define MENU_HPP

#include <vector>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "Init.hpp"  // ✅ Esto es indispensable para que reconozca SystemResources

class Menu {
public:
    Menu(ALLEGRO_FONT* font, int width, int height);
    ~Menu();

    void processInput(ALLEGRO_EVENT& event);
    int getSelectedOption() const;
    void draw() const;
    int run(SystemResources& sys);  // ahora se reconoce bien

private:
    ALLEGRO_FONT* font;
    int width;
    int height;
    int selectedOption;
    std::vector<std::string> options;
    std::vector<std::pair<int, int>> stars;

    void generateStars();
};

#endif
