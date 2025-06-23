#ifndef GAME_HPP
#define GAME_HPP

#include "Init.hpp"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <algorithm>
#include <random>
#include <string>

// Estructura para representar las estrellas del fondo
struct Star {
   float x;        // Posición horizontal de la estrella
   float y;        // Posición vertical de la estrella
   int type;       // Tipo de estrella (probablemente para diferentes tamaños o colores)
   
   // Constructor para inicializar una estrella con posición y tipo
   Star(float x, float y, int type) : x(x), y(y), type(type) {}
};

// Clase principal del juego que maneja toda la lógica del gameplay
class Game {
public:
   // Constructor: inicializa el juego con la fuente, ancho y alto de pantalla
   Game(ALLEGRO_FONT* font, int width, int height);
   
   // Destructor: limpia recursos cuando se destruye el objeto
   ~Game(); 
   
   // Función principal que ejecuta el bucle del juego
   int run(SystemResources& sys);
   
   // Dibuja todos los elementos del juego en pantalla
   void draw() const;
   
   // Dibuja específicamente los enemigos
   void dibujarenemigos() const;
   
   // Dibuja la nave del jugador
   void dibujarnave() const;
   
   // Dibuja las balas disparadas
   void dibujarbala() const;
   
   // Devuelve la puntuación actual del jugador
   int getCurrentScore() const;

private:
   // Fuentes para el texto del juego
   ALLEGRO_FONT* font;         // Fuente principal
   ALLEGRO_FONT* smallFont;    // Fuente más pequeña
   
   // Dimensiones de la pantalla
   int width, height;
   
   // Sistema de puntuación
   int currentScore;           // Puntos actuales del jugador
   int highScore;              // Mejor puntuación registrada

   // Variables para Game Over y explosión - REORDENADAS
   bool gameOver;              // Indica si el juego ha terminado
   bool showExplosion;         // Controla si se muestra la animación de explosión
   float explosionTimer;       // Cronómetro para la duración de la explosión
   float gameOverTimer;        // Cronómetro para la pantalla de game over
   float starSpeed;            // Velocidad de movimiento de las estrellas del fondo
   float speedMultiplier;      // Multiplicador de velocidad para aumentar dificultad
   float elapsedTime;          // Tiempo transcurrido en el juego - MOVIDO DESPUÉS DE speedMultiplier
   
   // Imágenes/sprites del juego
   ALLEGRO_BITMAP* explosionImg;   // Imagen de la explosión
   ALLEGRO_BITMAP* livesSprite;    // Sprite para mostrar las vidas restantes

   // Vector que contiene todas las estrellas del fondo
   std::vector<Star> stars;
   
   // Funciones privadas para la lógica del juego
   void crearnivel();                              // Crea/inicializa un nuevo nivel
   void crearbala(int dy, int x, int y, SystemResources& sys);  // Crea una nueva bala
   void colisiones(SystemResources& sys);          // Detecta y maneja colisiones
   void actualizarenemigos(SystemResources& sys);  // Actualiza posición y estado de enemigos
   void actualizarbala(SystemResources& sys);      // Actualiza posición y estado de balas
   void actualizarNave(SystemResources& sys);      // Actualiza la nave del jugador
   void crearnave();                               // Inicializa la nave del jugador
   void generateStars();                           // Genera las estrellas del fondo
   void update(SystemResources& sys);              // Actualiza todo el estado del juego
};

#endif