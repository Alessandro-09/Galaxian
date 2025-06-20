#ifndef HIGHSCORE_HPP
#define HIGHSCORE_HPP

#include <vector>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <utility>
#include "Init.hpp"

struct ScoreEntry {
    int score;
    std::string name;

    ScoreEntry() : score(0), name("---") {}
    ScoreEntry(int s, const std::string& n) : score(s), name(n) {}
};

class HighScore {
public:
    HighScore(ALLEGRO_FONT* font, int width, int height);
    ~HighScore();

    void addScore(int score, const std::string& name);
    void loadScores();
    void saveScores();
    std::vector<ScoreEntry> getTopScores(int count = 5);

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
    static const std::string SCORES_FILE;
};

#endif