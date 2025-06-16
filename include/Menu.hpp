// include/Menu.hpp
#ifndef MENU_HPP
#define MENU_HPP

#include <vector>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

class Menu {
public:
    Menu(ALLEGRO_FONT* font, int width, int height);
    ~Menu();

    void processInput(ALLEGRO_EVENT& event);
    void draw() const;
    int getSelectedOption() const;

private:
    std::vector<std::string> options;
    int selectedOption;
    ALLEGRO_FONT* font;
    int width;
    int height;
};

#endif // MENU_HPP
