#ifndef GAME_HPP
#define GAME_HPP

#include "Init.hpp"
#include <vector>

struct Star {
    float x;
    float y;
    int type;  
    
    Star(float x, float y, int type) : x(x), y(y), type(type) {}
};

class Game {
public:
    Game(ALLEGRO_FONT* font, int width, int height);
    int run(SystemResources& sys);
    void draw() const;

private:
    ALLEGRO_FONT* font;
    int width, height;
    std::vector<Star> stars;

    void generateStars();
    void update();
    float starSpeed;
    float speedMultiplier; 
    float elapsedTime;  
};

#endif