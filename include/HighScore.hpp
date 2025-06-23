#ifndef HIGHSCORE_HPP
#define HIGHSCORE_HPP

#include <vector>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <utility>
#include "Init.hpp"

// Estructura que representa una entrada en la tabla de puntuaciones
struct ScoreEntry {
   int score;              // La puntuación obtenida por el jugador
   std::string name;       // El nombre del jugador

   // Constructor por defecto: crea una entrada vacía con puntuación 0 y nombre "---"
   ScoreEntry() : score(0), name("---") {}
   
   // Constructor con parámetros: crea una entrada con puntuación y nombre específicos
   ScoreEntry(int s, const std::string& n) : score(s), name(n) {}
};

// Clase que maneja todo el sistema de puntuaciones altas del juego
class HighScore {
public:
   // Constructor: inicializa el sistema de puntuaciones con fuente y dimensiones de pantalla
   HighScore(ALLEGRO_FONT* font, int width, int height);
   
   // Destructor: limpia recursos cuando se destruye el objeto
   ~HighScore();

   // Añade una nueva puntuación a la lista (se ordena automáticamente)
   void addScore(int score, const std::string& name);
   
   // Carga las puntuaciones guardadas desde el archivo
   void loadScores();
   
   // Guarda las puntuaciones actuales en el archivo
   void saveScores();
   
   // Devuelve las mejores puntuaciones (por defecto las 5 mejores)
   std::vector<ScoreEntry> getTopScores(int count = 5);

   // Ejecuta la pantalla de puntuaciones altas (bucle principal)
   void run(SystemResources& sys);
   
   // Dibuja la pantalla de puntuaciones en pantalla
   void draw() const;

private:
   // Fuente para mostrar el texto de las puntuaciones
   ALLEGRO_FONT* font;
   
   // Dimensiones de la pantalla
   int width;
   int height;
   
   // Vector que almacena todas las entradas de puntuaciones
   std::vector<ScoreEntry> scores;
   
   // Vector de estrellas del fondo (cada par representa coordenadas x, y)
   std::vector<std::pair<float, float>> stars;

   // Genera las estrellas que se muestran en el fondo de la pantalla
   void generateStars();
   
   // Actualiza la posición de las estrellas para crear efecto de movimiento
   void updateStars();
   
   // Nombre del archivo donde se guardan las puntuaciones
   static const std::string SCORES_FILE;
};

#endif