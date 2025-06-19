#ifndef HIGHSCORE_HPP
#define HIGHSCORE_HPP

#include <vector>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <utility>  // Para std::pair
#include "Init.hpp"

struct ScoreEntry {
    int score;
    std::string date;
    
    // Constructor por defecto
    ScoreEntry() : score(0), date("") {}
    
    // Constructor con parámetros
    ScoreEntry(int s, const std::string& d) : score(s), date(d) {}
};

class HighScore {
public:
    HighScore(ALLEGRO_FONT* font, int width, int height);
    ~HighScore();
    
    // Métodos para manejar puntajes
    void addScore(int score);
    void loadScores();
    void saveScores();
    std::vector<ScoreEntry> getTopScores(int count = 5);
    
    // Métodos para la pantalla de high scores
    void run(SystemResources& sys);
    void draw() const;
    
private:
    ALLEGRO_FONT* font;
    int width;
    int height;
    std::vector<ScoreEntry> scores;
    std::vector<std::pair<float, float>> stars;
    
    void generateStars();
    void updateStars();
    std::string getCurrentDate();
    static const std::string SCORES_FILE;
};

#endif