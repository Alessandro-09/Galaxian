#include "Game.hpp"
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
#include <cstdlib> 
#include "HighScore.hpp"

using namespace std;

// VARIABLES GLOBALES 
// Variables que controlan el estado general del juego
int nivel = 1;                    // Nivel actual del juego (comienza en 1)
bool derecha = false;             // Flag para controlar si el jugador se mueve a la derecha
bool izquierda = false;           // Flag para controlar si el jugador se mueve a la izquierda
double tiempoUltimoAtaque = 0;    // Cronómetro para controlar cuándo los enemigos pueden atacar

// ESTRUCTURAS DEL JUEGO 

// Estructura para representar las balas (tanto del jugador como de enemigos)
typedef struct Bala {
    float x, y;           // Posición de la bala en el espacio
    float velocidad;      // Velocidad de movimiento (positiva hacia arriba, negativa hacia abajo)
    Bala* siguiente;      // Puntero al siguiente elemento en la lista enlazada
}*ptr_bala;

// Estructura para representar la nave del jugador
typedef struct personaje {
    ALLEGRO_BITMAP* bitmap;        // Imagen normal de la nave
    ALLEGRO_BITMAP* disparobitmap; // Imagen de la nave cuando dispara
    int ancho, alto;               // Dimensiones de la nave
    float x, y;                    // Posición actual de la nave
    double tiempo;                 // Cronómetro para controlar animaciones
    int vida;                      // Vidas restantes del jugador
}*ptr_nave;

// Estructura para representar las naves enemigas
typedef struct navesenemigas {
    ALLEGRO_BITMAP* bitmap;        // Imagen del enemigo
    int col, fila;                 // Posición en la formación de enemigos (columna y fila)
    float origenx, origeny;        // Posición original en la formación
    int ancho, alto;               // Dimensiones del enemigo
    float x, y;                    // Posición actual del enemigo
    float dx, dy;                  // Velocidades de movimiento horizontal y vertical
    int ataque;                    // Estado de ataque (0=patrullando, 1=atacando)
    int estado;                    // Estado del comportamiento del enemigo
    int Disparo;                   // Cantidad de disparos restantes
    double Tiempo;                 // Cronómetro para controlar disparos
    int salio = 0;                 // Flag para controlar si el enemigo salió de su posición
    navesenemigas* Siguiente;      // Puntero al siguiente enemigo en la lista
}*ptr_est;

// LISTAS GLOBALES 
// Listas enlazadas que manejan todos los elementos dinámicos del juego
ptr_bala Balas = nullptr;         // Lista de todas las balas en pantalla
ptr_est enemigos = nullptr;       // Lista de todos los enemigos activos
ptr_nave nave = nullptr;          // Puntero a la nave del jugador

// Array de imágenes compartidas para los diferentes tipos de enemigos
ALLEGRO_BITMAP* enemy_bitmaps[4] = {nullptr};

// FUNCIONES AUXILIARES 

// Agrega un nuevo enemigo al final de la lista de enemigos
void agregaralfinal(ptr_est& lista, ptr_est Nuevo) {
    ptr_est Aux = lista;
    Nuevo->salio = 0;  // Inicializa el flag de salida
    
    if (Aux != NULL) {
        // Recorre hasta el final de la lista
        while (Aux->Siguiente != NULL) {
            Aux = Aux->Siguiente;
        }
        // Conecta el nuevo enemigo al final
        Aux->Siguiente = Nuevo;
    } else {
        // Si la lista está vacía, el nuevo elemento se convierte en el primero
        lista = Nuevo;
    }
}

// Agrega una nueva bala al final de la lista de balas
void agregarBala(ptr_bala& lista, ptr_bala Nuevo) {
    ptr_bala Aux = lista;
    
    if (Aux != NULL) {
        // Recorre hasta el final de la lista
        while (Aux->siguiente != NULL) {
            Aux = Aux->siguiente;
        }
        // Conecta la nueva bala al final
        Aux->siguiente = Nuevo;
    } else {
        // Si la lista está vacía, la nueva bala se convierte en la primera
        lista = Nuevo;
    }
}

// Elimina todos los enemigos de la lista y libera la memoria
void limpiarenemigos() {
    ptr_est aux = enemigos;
    while (aux != nullptr) {
        ptr_est temp = aux;
        aux = aux->Siguiente;
        delete temp;  // Libera la memoria del enemigo actual
    }
    enemigos = nullptr;  // Reinicia la lista
}

// Elimina todas las balas de la lista y libera la memoria
void limpiarbalas() {
    ptr_bala aux = Balas;
    while (aux != nullptr) {
        ptr_bala temp = aux;
        aux = aux->siguiente;
        delete temp;  // Libera la memoria de la bala actual
    }
    Balas = nullptr;  // Reinicia la lista
}

// Verifica si un enemigo puede atacar (no hay otros enemigos bloqueándolo por debajo)
bool puedeAtacar(ptr_est e) {
    ptr_est aux = enemigos;
    while (aux != nullptr) {
        // Comprueba si hay otro enemigo en una fila inferior que lo bloquee
        if (aux != e &&
            aux->x < e->x + 30 && aux->x + 30 > e->x &&
            aux->fila > e->fila) {
            return false;  // Está bloqueado
        }
        aux = aux->Siguiente;
    }
    return true;  // Puede atacar libremente
}

// IMPLEMENTACIÓN DE LA CLASE GAME

// Constructor: Inicializa todos los componentes del juego
Game::Game(ALLEGRO_FONT* font, int width, int height) 
    : font(font), smallFont(nullptr), width(width), height(height), 
      currentScore(0), highScore(0), gameOver(false), showExplosion(false),
      explosionTimer(0.0f), gameOverTimer(0.0f), starSpeed(1.0f),
      speedMultiplier(1.0f), elapsedTime(0.0f), explosionImg(nullptr),
      livesSprite(nullptr) {

    // Cargar las imágenes de los diferentes tipos de enemigos
    enemy_bitmaps[0] = al_load_bitmap("pictures/navenemiga1.png");
    enemy_bitmaps[1] = al_load_bitmap("pictures/navenemiga2.png");
    enemy_bitmaps[2] = al_load_bitmap("pictures/navenemiga3.png");
    enemy_bitmaps[3] = al_load_bitmap("pictures/navenemiga4.png");

    // Crear una fuente más pequeña para mostrar puntajes
    smallFont = al_load_ttf_font("assets/space_font.ttf", 18, 0);
    if (!smallFont) {
        smallFont = font;  // Si no se puede cargar, usa la fuente principal
    }

    // Cargar recursos gráficos adicionales
    livesSprite = al_load_bitmap("pictures/nave1.png");    // Imagen para mostrar vidas
    explosionImg = al_load_bitmap("pictures/explosion.png"); // Imagen de explosión

    // Inicializar los elementos del juego
    crearnivel();      // Crear la formación de enemigos
    crearnave();       // Crear la nave del jugador
    generateStars();   // Generar las estrellas del fondo

    // Cargar la puntuación más alta guardada
    HighScore hs(font, width, height);
    auto topScores = hs.getTopScores(1);
    if (!topScores.empty()) {
        highScore = topScores[0].score;
    }
}

// Destructor: Limpia todos los recursos al finalizar el juego
Game::~Game() {
    // Limpiar todas las listas dinámicas
    limpiarenemigos();
    limpiarbalas();

    // Liberar la nave del jugador
    if (nave) {
        if (nave->bitmap) al_destroy_bitmap(nave->bitmap);
        if (nave->disparobitmap && nave->disparobitmap != nave->bitmap) {
            al_destroy_bitmap(nave->disparobitmap);
        }
        delete nave;
        nave = nullptr;
    }

    // Liberar las imágenes compartidas de enemigos
    for (int i = 0; i < 4; ++i) {
        if (enemy_bitmaps[i]) {
            al_destroy_bitmap(enemy_bitmaps[i]);
            enemy_bitmaps[i] = nullptr;
        }
    }

    // Liberar otros recursos gráficos
    if (smallFont && smallFont != font) {
        al_destroy_font(smallFont);
    }
    
    if (explosionImg) {
        al_destroy_bitmap(explosionImg);
    }
    
    if (livesSprite) {
        al_destroy_bitmap(livesSprite);
    }
}

// Dibuja todos los enemigos en pantalla
void Game::dibujarenemigos() const
{
    ptr_est aux = enemigos;
    while (aux != nullptr) {
        // Dibuja cada enemigo reescalado a 30x30 píxeles
        al_draw_scaled_bitmap(aux->bitmap, 0, 0, aux->ancho, aux->alto,aux->x, aux->y,30, 30,0);
        aux = aux->Siguiente;
    }
}

// Crea la formación de enemigos según el nivel actual
void Game::crearnivel() {
    limpiarenemigos();  // Elimina enemigos anteriores

    ptr_est nuevo;
    int x, y = 40;  // Posición inicial
    int col;
    const float velocidadBase = 1.0f;
    float velocidad = velocidadBase * (1 + (nivel-1)*0.25f);  // Velocidad aumenta con el nivel

    switch (nivel) {
        case 1:
            // NIVEL 1: Formación básica
            // Fila 0 - 1 enemigo tipo 1 (más valioso)
            col = 1;
            x = 430;
            for (int i = 0; i < col; i++) {
                nuevo = new navesenemigas();
                nuevo->bitmap = enemy_bitmaps[0];
                nuevo->alto = al_get_bitmap_height(nuevo->bitmap);
                nuevo->ancho = al_get_bitmap_width(nuevo->bitmap);
                nuevo->x = x;
                nuevo->y = y;
                nuevo->origenx = x;     // Guarda posición original
                nuevo->origeny = y;
                nuevo->col = i;         // Columna en la formación
                nuevo->fila = 0;        // Fila en la formación
                nuevo->dx = velocidad;  // Velocidad horizontal
                nuevo->dy = 0;          // Velocidad vertical inicial
                nuevo->ataque = 0;      // No está atacando
                nuevo->estado = 0;      // Estado de patrullaje
                nuevo->Siguiente = nullptr;
                nuevo->Disparo = 1;     // Puede disparar
                nuevo->Tiempo = 0;      // Cronómetro de disparo
                nuevo->salio = 0;       // No ha salido de formación
                agregaralfinal(enemigos, nuevo);
                x += 40;  // Espacio entre enemigos
            }
            y += 60;  // Espacio entre filas

            // Fila 1 - 2 enemigos tipo 2
            col = 2;
            x = 405;
            for (int i = 0; i < col; i++) {
                nuevo = new navesenemigas();
                nuevo->bitmap = enemy_bitmaps[1];
                nuevo->alto = al_get_bitmap_height(nuevo->bitmap);
                nuevo->ancho = al_get_bitmap_width(nuevo->bitmap);
                nuevo->x = x;
                nuevo->y = y;
                nuevo->origenx = x;
                nuevo->origeny = y;
                nuevo->col = i;
                nuevo->fila = 1;
                nuevo->dx = velocidad;
                nuevo->dy = 0;
                nuevo->ataque = 0;
                nuevo->estado = 0;
                nuevo->Siguiente = nullptr;
                nuevo->Disparo = 1;
                nuevo->Tiempo = 0;
                nuevo->salio = 0;
                agregaralfinal(enemigos, nuevo);
                x += 40;
            }
            y += 60;

            // Fila 2 - 4 enemigos tipo 3
            col = 4;
            x = 380;
            for (int i = 0; i < col; i++) {
                nuevo = new navesenemigas();
                nuevo->bitmap = enemy_bitmaps[2];
                nuevo->alto = al_get_bitmap_height(nuevo->bitmap);
                nuevo->ancho = al_get_bitmap_width(nuevo->bitmap);
                nuevo->x = x;
                nuevo->y = y;
                nuevo->origenx = x;
                nuevo->origeny = y;
                nuevo->col = i;
                nuevo->fila = 2;
                nuevo->dx = velocidad;
                nuevo->dy = 0;
                nuevo->ataque = 0;
                nuevo->estado = 0;
                nuevo->Siguiente = nullptr;
                nuevo->Disparo = 1;
                nuevo->Tiempo = 0;
                nuevo->salio = 0;
                agregaralfinal(enemigos, nuevo);
                x += 40;
            }
            y += 60;

            // Filas 3 y 4 - 5 enemigos tipo 4 cada una (menos valiosos)
            col = 5;
            for (int fila = 3; fila < 5; fila++) {
                x = 380;
                for (int i = 0; i < col; i++) {
                    nuevo = new navesenemigas();
                    nuevo->bitmap = enemy_bitmaps[3];
                    nuevo->alto = al_get_bitmap_height(nuevo->bitmap);
                    nuevo->ancho = al_get_bitmap_width(nuevo->bitmap);
                    nuevo->x = x;
                    nuevo->y = y;
                    nuevo->origenx = x;
                    nuevo->origeny = y;
                    nuevo->col = i;
                    nuevo->fila = fila;
                    nuevo->dx = velocidad;
                    nuevo->dy = 0;
                    nuevo->ataque = 0;
                    nuevo->estado = 0;
                    nuevo->Siguiente = nullptr;
                    nuevo->Disparo = 1;
                    nuevo->Tiempo = 0;
                    nuevo->salio = 0;
                    agregaralfinal(enemigos, nuevo);
                    x += 40;
                }
                y += 60;
            }
            break;

            case 2:
            // NIVEL 2: Más enemigos y mayor velocidad 
            // Fila 0 - 2 enemigos tipo 1
            col = 2;
            x = 405;
            for (int i = 0; i < col; i++) {
                nuevo = new navesenemigas();
                nuevo->bitmap = enemy_bitmaps[0];
                nuevo->alto = al_get_bitmap_height(nuevo->bitmap);
                nuevo->ancho = al_get_bitmap_width(nuevo->bitmap);
                nuevo->x = x;
                nuevo->y = y;
                nuevo->origenx = x;
                nuevo->origeny = y;
                nuevo->col = i;
                nuevo->fila = 0;
                nuevo->dx = velocidad * 1.25f;  // 25% más rápido
                nuevo->dy = 0;
                nuevo->ataque = 0;
                nuevo->estado = 0;
                nuevo->Siguiente = nullptr;
                nuevo->Disparo = 1;
                nuevo->Tiempo = 0;
                nuevo->salio = 0;
                agregaralfinal(enemigos, nuevo);
                x += 40;
            }
            y += 60;
        
            // Fila 1 - 3 enemigos tipo 2
            col = 3;
            x = 380;
            for (int i = 0; i < col; i++) {
                nuevo = new navesenemigas();
                nuevo->bitmap = enemy_bitmaps[1];
                nuevo->alto = al_get_bitmap_height(nuevo->bitmap);
                nuevo->ancho = al_get_bitmap_width(nuevo->bitmap);
                nuevo->x = x;
                nuevo->y = y;
                nuevo->origenx = x;
                nuevo->origeny = y;
                nuevo->col = i;
                nuevo->fila = 1;
                nuevo->dx = velocidad * 1.25f;
                nuevo->dy = 0;
                nuevo->ataque = 0;
                nuevo->estado = 0;
                nuevo->Siguiente = nullptr;
                nuevo->Disparo = 1;
                nuevo->Tiempo = 0;
                nuevo->salio = 0;
                agregaralfinal(enemigos, nuevo);
                x += 40;
            }
            y += 60;
        
            // Fila 2 - 5 enemigos tipo 3
            col = 5;
            x = 355;
            for (int i = 0; i < col; i++) {
                nuevo = new navesenemigas();
                nuevo->bitmap = enemy_bitmaps[2];
                nuevo->alto = al_get_bitmap_height(nuevo->bitmap);
                nuevo->ancho = al_get_bitmap_width(nuevo->bitmap);
                nuevo->x = x;
                nuevo->y = y;
                nuevo->origenx = x;
                nuevo->origeny = y;
                nuevo->col = i;
                nuevo->fila = 2;
                nuevo->dx = velocidad * 1.25f;
                nuevo->dy = 0;
                nuevo->ataque = 0;
                nuevo->estado = 0;
                nuevo->Siguiente = nullptr;
                nuevo->Disparo = 1;
                nuevo->Tiempo = 0;
                nuevo->salio = 0;
                agregaralfinal(enemigos, nuevo);
                x += 40;
            }
            y += 60;
        
            // Filas 3 y 4 - 6 enemigos tipo 4 cada una
            col = 6;
            for (int fila = 3; fila < 5; fila++) {
                x = 355;
                for (int i = 0; i < col; i++) {
                    nuevo = new navesenemigas();
                    nuevo->bitmap = enemy_bitmaps[3];
                    nuevo->alto = al_get_bitmap_height(nuevo->bitmap);
                    nuevo->ancho = al_get_bitmap_width(nuevo->bitmap);
                    nuevo->x = x;
                    nuevo->y = y;
                    nuevo->origenx = x;
                    nuevo->origeny = y;
                    nuevo->col = i;
                    nuevo->fila = fila;
                    nuevo->dx = velocidad * 1.25f;
                    nuevo->dy = 0;
                    nuevo->ataque = 0;
                    nuevo->estado = 0;
                    nuevo->Siguiente = nullptr;
                    nuevo->Disparo = 1;
                    nuevo->Tiempo = 0;
                    nuevo->salio = 0;
                    agregaralfinal(enemigos, nuevo);
                    x += 40;
                }
                y += 60;
            }
            break;
        
        case 3:
            // NIVEL 3: Aún más enemigos y mayor velocidad 
            // Fila 0 - 3 enemigos tipo 1
            col = 3;
            x = 380;
            for (int i = 0; i < col; i++) {
                nuevo = new navesenemigas();
                nuevo->bitmap = enemy_bitmaps[0];
                nuevo->alto = al_get_bitmap_height(nuevo->bitmap);
                nuevo->ancho = al_get_bitmap_width(nuevo->bitmap);
                nuevo->x = x;
                nuevo->y = y;
                nuevo->origenx = x;
                nuevo->origeny = y;
                nuevo->col = i;
                nuevo->fila = 0;
                nuevo->dx = velocidad * 1.5f;  // 50% más rápido
                nuevo->dy = 0;
                nuevo->ataque = 0;
                nuevo->estado = 0;
                nuevo->Siguiente = nullptr;
                nuevo->Disparo = 1;
                nuevo->Tiempo = 0;
                nuevo->salio = 0;
                agregaralfinal(enemigos, nuevo);
                x += 40;
            }
            y += 60;
        
            // Fila 1 - 5 enemigos tipo 2
            col = 5;
            x = 355;
            for (int i = 0; i < col; i++) {
                nuevo = new navesenemigas();
                nuevo->bitmap = enemy_bitmaps[1];
                nuevo->alto = al_get_bitmap_height(nuevo->bitmap);
                nuevo->ancho = al_get_bitmap_width(nuevo->bitmap);
                nuevo->x = x;
                nuevo->y = y;
                nuevo->origenx = x;
                nuevo->origeny = y;
                nuevo->col = i;
                nuevo->fila = 1;
                nuevo->dx = velocidad * 1.5f;
                nuevo->dy = 0;
                nuevo->ataque = 0;
                nuevo->estado = 0;
                nuevo->Siguiente = nullptr;
                nuevo->Disparo = 1;
                nuevo->Tiempo = 0;
                nuevo->salio = 0;
                agregaralfinal(enemigos, nuevo);
                x += 40;
            }
            y += 60;
        
            // Fila 2 - 7 enemigos tipo 3
            col = 7;
            x = 330;
            for (int i = 0; i < col; i++) {
                nuevo = new navesenemigas();
                nuevo->bitmap = enemy_bitmaps[2];
                nuevo->alto = al_get_bitmap_height(nuevo->bitmap);
                nuevo->ancho = al_get_bitmap_width(nuevo->bitmap);
                nuevo->x = x;
                nuevo->y = y;
                nuevo->origenx = x;
                nuevo->origeny = y;
                nuevo->col = i;
                nuevo->fila = 2;
                nuevo->dx = velocidad * 1.5f;
                nuevo->dy = 0;
                nuevo->ataque = 0;
                nuevo->estado = 0;
                nuevo->Siguiente = nullptr;
                nuevo->Disparo = 1;
                nuevo->Tiempo = 0;
                nuevo->salio = 0;
                agregaralfinal(enemigos, nuevo);
                x += 40;
            }
            y += 60;
        
            // Filas 3 y 4 - 8 enemigos tipo 4 cada una
            col = 8;
            for (int fila = 3; fila < 5; fila++) {
                x = 330;
                for (int i = 0; i < col; i++) {
                    nuevo = new navesenemigas();
                    nuevo->bitmap = enemy_bitmaps[3];
                    nuevo->alto = al_get_bitmap_height(nuevo->bitmap);
                    nuevo->ancho = al_get_bitmap_width(nuevo->bitmap);
                    nuevo->x = x;
                    nuevo->y = y;
                    nuevo->origenx = x;
                    nuevo->origeny = y;
                    nuevo->col = i;
                    nuevo->fila = fila;
                    nuevo->dx = velocidad * 1.5f;
                    nuevo->dy = 0;
                    nuevo->ataque = 0;
                    nuevo->estado = 0;
                    nuevo->Siguiente = nullptr;
                    nuevo->Disparo = 1;
                    nuevo->Tiempo = 0;
                    nuevo->salio = 0;
                    agregaralfinal(enemigos, nuevo);
                    x += 40;
                }
                y += 60;
            }
            break;
        
        default:
            // NIVELES 4+: Dificultad máxima
            // Fila 0 - 4 enemigos tipo 1
            col = 4;
            x = 330;
            for (int i = 0; i < col; i++) {
                nuevo = new navesenemigas();
                nuevo->bitmap = enemy_bitmaps[0];
                nuevo->alto = al_get_bitmap_height(nuevo->bitmap);
                nuevo->ancho = al_get_bitmap_width(nuevo->bitmap);
                nuevo->x = x;
                nuevo->y = y;
                nuevo->origenx = x;
                nuevo->origeny = y;
                nuevo->col = i;
                nuevo->fila = 0;
                nuevo->dx = velocidad * 2.0f;  // Doble velocidad
                nuevo->dy = 0;
                nuevo->ataque = 0;
                nuevo->estado = 0;
                nuevo->Siguiente = nullptr;
                nuevo->Disparo = 1;
                nuevo->Tiempo = 0;
                nuevo->salio = 0;
                agregaralfinal(enemigos, nuevo);
                x += 40;
            }
            y += 60;
        
            // Fila 1 - 6 enemigos tipo 2
            col = 6;
            x = 305;
            for (int i = 0; i < col; i++) {
                nuevo = new navesenemigas();
                nuevo->bitmap = enemy_bitmaps[1];
                nuevo->alto = al_get_bitmap_height(nuevo->bitmap);
                nuevo->ancho = al_get_bitmap_width(nuevo->bitmap);
                nuevo->x = x;
                nuevo->y = y;
                nuevo->origenx = x;
                nuevo->origeny = y;
                nuevo->col = i;
                nuevo->fila = 1;
                nuevo->dx = velocidad * 2.0f;
                nuevo->dy = 0;
                nuevo->ataque = 0;
                nuevo->estado = 0;
                nuevo->Siguiente = nullptr;
                nuevo->Disparo = 1;
                nuevo->Tiempo = 0;
                nuevo->salio = 0;
                agregaralfinal(enemigos, nuevo);
                x += 40;
            }
            y += 60;
        
            // Fila 2 - 8 enemigos tipo 3
            col = 8;
            x = 280;
            for (int i = 0; i < col; i++) {
                nuevo = new navesenemigas();
                nuevo->bitmap = enemy_bitmaps[2];
                nuevo->alto = al_get_bitmap_height(nuevo->bitmap);
                nuevo->ancho = al_get_bitmap_width(nuevo->bitmap);
                nuevo->x = x;
                nuevo->y = y;
                nuevo->origenx = x;
                nuevo->origeny = y;
                nuevo->col = i;
                nuevo->fila = 2;
                nuevo->dx = velocidad * 2.0f;
                nuevo->dy = 0;
                nuevo->ataque = 0;
                nuevo->estado = 0;
                nuevo->Siguiente = nullptr;
                nuevo->Disparo = 1;
                nuevo->Tiempo = 0;
                nuevo->salio = 0;
                agregaralfinal(enemigos, nuevo);
                x += 40;
            }
            y += 60;
        
            // Filas 3 y 4 - 10 enemigos tipo 4 cada una
            col = 10;
            for (int fila = 3; fila < 5; fila++) {
                x = 280;
                for (int i = 0; i < col; i++) {
                    nuevo = new navesenemigas();
                    nuevo->bitmap = enemy_bitmaps[3];
                    nuevo->alto = al_get_bitmap_height(nuevo->bitmap);
                    nuevo->ancho = al_get_bitmap_width(nuevo->bitmap);
                    nuevo->x = x;
                    nuevo->y = y;
                    nuevo->origenx = x;
                    nuevo->origeny = y;
                    nuevo->col = i;
                    nuevo->fila = fila;
                    nuevo->dx = velocidad * 2.0f;
                    nuevo->dy = 0;
                    nuevo->ataque = 0;
                    nuevo->estado = 0;
                    nuevo->Siguiente = nullptr;
                    nuevo->Disparo = 1;
                    nuevo->Tiempo = 0;
                    nuevo->salio = 0;
                    agregaralfinal(enemigos, nuevo);
                    x += 40;
                }
                y += 60;
            }
            break;
        }
    }

// Maneja todas las colisiones del juego
void Game::colisiones(SystemResources& sys)
{
    // COLISIONES ENTRE BALAS DEL JUGADOR Y ENEMIGOS 
    ptr_bala aux=Balas;        // Puntero para recorrer las balas
    ptr_bala aux2=nullptr;     // Puntero auxiliar para manejar la lista
 
    while (aux != nullptr)
    {
        ptr_est enemigo = enemigos;     // Puntero para recorrer enemigos
        ptr_est enemigo2 = nullptr;     // Puntero auxiliar para manejar lista de enemigos
        bool colision = false;          // Flag para detectar si hubo colisión

        while (enemigo != nullptr && !colision)
        {
            // Verifica si la bala y el enemigo se superponen
            // (negación de: están separados)
            if (!(aux->x + 30 < enemigo->x || aux->x > enemigo->x + 30 ||
                  aux->y + 30 < enemigo->y || aux->y > enemigo->y + 30))
            {
                // SISTEMA DE PUNTUACIÓN POR TIPO DE ENEMIGO 
                if (enemigo->fila == 0) currentScore += 100;      // Primera fila: 100 puntos
                else if (enemigo->fila == 1) currentScore += 75;  // Segunda fila: 75 puntos
                else if (enemigo->fila == 2) currentScore += 50;  // Tercera fila: 50 puntos
                else currentScore += 25;                           // Otras filas: 25 puntos

                // Reproducir sonido de impacto
                if (sys.hitEnemySound) {
                    al_play_sample(sys.hitEnemySound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);
                }
                
                // Eliminar enemigo de la lista
                if (enemigo2 == nullptr)          // Si es el primer enemigo de la lista
                    enemigos = enemigo->Siguiente; // La lista ahora apunta al siguiente
                else
                    enemigo2->Siguiente = enemigo->Siguiente; // Conecta el anterior con el siguiente

                delete enemigo;  // Libera la memoria del enemigo

                // Eliminar la bala de la lista
                ptr_bala borrar = aux;
                if (aux2 == nullptr)              // Si es la primera bala
                    Balas = aux->siguiente;
                else
                    aux2->siguiente = aux->siguiente; // Conecta la anterior con la siguiente

                aux = borrar->siguiente;
                delete borrar;     // Libera la memoria de la bala

                colision = true;   // Marca que hubo colisión
            }
            else
            {
                // No hubo colisión, continúa con el siguiente enemigo
                enemigo2 = enemigo;
                enemigo = enemigo->Siguiente;
            }
        }

        if (!colision)
        {
            // Si no hubo colisión, pasa a la siguiente bala
            aux2 = aux;
            aux = aux->siguiente;
        }
    }

    // COLISIONES ENTRE BALAS ENEMIGAS Y LA NAVE DEL JUGADOR
    aux = Balas;
    aux2 = nullptr;
    bool colision = false;

    while (aux != nullptr)
    {
        // Verifica si una bala enemiga impacta la nave del jugador
        if (!(aux->x + 30 < nave->x || aux->x > nave->x + 30 ||
            aux->y+15 < nave->y || aux->y > nave->y+15 ))
        {
            // Reproducir sonido de impacto en el jugador
            if (sys.hitPlayerSound) {
                al_play_sample(sys.hitPlayerSound, 1.5f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, nullptr);
            }

            // Eliminar la bala que impactó
            ptr_bala borrar = aux;
            if (aux2 == nullptr)
                Balas = aux->siguiente;
            else
                aux2->siguiente = aux->siguiente;

            aux = borrar->siguiente;
            delete borrar;
            nave->vida -= 1;    // Reducir vida del jugador
            colision = true;
            cout << "colision" << std::endl;
            continue; // Saltar incremento de aux2 ya que eliminamos la bala
        }

        aux2 = aux;
        aux = aux->siguiente;
    }

    // COLISIONES DIRECTAS ENTRE ENEMIGOS Y LA NAVE 
    ptr_est enemigo = enemigos;
    ptr_est enemigo2 = nullptr;

    while (enemigo != nullptr && !colision)
    {
        // Verifica si un enemigo toca directamente la nave
        if (!(enemigo->x + 30 < nave->x || enemigo->x > nave->x + 30 ||
            enemigo->y + 30 < nave->y || enemigo->y > nave->y + 30))
        {
            // Reproducir sonido de impacto
            if (sys.hitPlayerSound) {
                al_play_sample(sys.hitPlayerSound, 1.5f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, nullptr);
            }
            
            cout << "colision" << std::endl;
            nave->vida -= 1;    // Reducir vida del jugador
            
            // Eliminar el enemigo que colisionó
            ptr_est eliminar = enemigo;
            if (enemigo2 == nullptr)
                enemigos = enemigo->Siguiente;
            else
                enemigo2->Siguiente = enemigo->Siguiente;

            enemigo = eliminar->Siguiente;
            delete eliminar;
        }
        else
        {
            enemigo2 = enemigo;
            enemigo = enemigo->Siguiente;
        }
    }
}

// Actualiza el comportamiento y movimiento de todos los enemigos
void Game::actualizarenemigos(SystemResources& sys)
{
    ptr_est enemigo = enemigos;
    float velocidad = 0.15f;    // Velocidad base de movimiento vertical
    int rango = 20;             // Rango de movimiento vertical desde la posición original

    while (enemigo != NULL) {
        if (enemigo->ataque != 1)   // Si el enemigo está en modo patrullaje
        {
            //  MÁQUINA DE ESTADOS PARA MOVIMIENTO VERTICAL
            switch (enemigo->estado)
            {
                case 0: // Movimiento hacia abajo
                    enemigo->y += velocidad;
                    if (enemigo->y >= enemigo->origeny + rango) // Si llegó al límite inferior
                    {
                        enemigo->estado = 1; // Cambia a volver hacia arriba
                    }
                    break;

                case 1: // Volviendo al origen desde abajo
                    enemigo->y -= velocidad;
                    if (enemigo->y <= enemigo->origeny)  // Si llegó al origen
                     {
                        enemigo->estado = 2; // Cambia a subir
                    }
                    break;

                case 2: // Movimiento hacia arriba
                    enemigo->y -= velocidad;
                    if (enemigo->y <= enemigo->origeny - rango) // Si llegó al límite superior
                    {
                        enemigo->estado = 3; // Cambia a volver al origen
                    }
                    break;

                case 3: // Volviendo al origen desde arriba
                    enemigo->y += velocidad;
                    if (enemigo->y >= enemigo->origeny)  // Si volvió al origen
                     {
                        enemigo->estado = 0; // Vuelve a bajar
                    }
                    break;
            }
            
            // Movimiento horizontal simultáneo
            enemigo->x += -1 * enemigo->dx;

            // Rebote en los límites horizontales
            if (enemigo->x >= enemigo->origenx + 300 ||
                enemigo->x <= enemigo->origenx - 300)
            {
                enemigo->dx *= -1; // Cambia de dirección
            }
        } 
        
        if (enemigo->ataque == 1) // Si el enemigo está en modo ataque
        {
            switch (enemigo->estado) 
            {
                case 100: // Fase de acercamiento inicial
                {
                    // Ajuste de velocidad según el nivel
                    float n=0;
                    if(nivel>3)
                    {
                        n=2;
                    }
                    else
                    {
                        n=nivel/2;
                    }
                    
                    // Desciende hasta la posición y = 340
                    if (fabs(enemigo->y - 340.0f) > 1.0f)
                        enemigo->y += 0.5f * ((340.0f > enemigo->y) ? 1+n : -2-n);

                    // Si llegó al punto de ataque, cambia de estado
                    if (fabs(enemigo->y - 340.0f) <= 1.0f) {
                        enemigo->estado = 101;
                    }
                    
                    // Mantiene movimiento horizontal mientras se acerca
                    enemigo->x += -1 * enemigo->dx;

                    if (enemigo->x >= enemigo->origenx + 300 ||
                        enemigo->x <= enemigo->origenx - 300)
                    {
                        enemigo->dx *= -1; // Cambia de dirección
                    }
                    break;
                }
                
                case 101: // Fase de ataque directo hacia el jugador
                {
                    // Verificar si colisionaría con otro enemigo
                    bool colisionaConOtro = false;
                    ptr_est otro = enemigos;

                    while (otro != nullptr) {
                        if (otro != enemigo && 
                            !(enemigo->x + 30 < otro->x || enemigo->x > otro->x + 30 ||
                            enemigo->y + 30 < otro->y || enemigo->y > otro->y + 30)) {
                            colisionaConOtro = true;
                            break;
                        }
                        otro = otro->Siguiente;
                    }

                    // Disparar si las condiciones se cumplen
                    if (enemigo->y > 340 && enemigo->Disparo != 0 && 
                        al_get_time() - enemigo->Tiempo >= 2 && puedeAtacar(enemigo)) {
                        enemigo->Disparo -= 1;
                        enemigo->Tiempo = al_get_time();
                        crearbala(-3, enemigo->x + 15, enemigo->y + 50, sys); // Dispara hacia abajo
                    }

                    // Ajuste de velocidad según nivel
                    float n=0;
                    if(nivel>3)
                    {
                        n=2;
                    }
                    else
                    {
                        n=nivel/2;
                    }
                    
                    // Movimiento vertical si no hay colisión
                    if (!colisionaConOtro)
                        enemigo->y += 1+n;

                    // Movimiento horizontal hacia el jugador evitando colisiones
                    bool puedeMoverX = true;
                    otro = enemigos;
                    int nuevoX = enemigo->x + ((nave->x > enemigo->x) ? 1+n : -1-n);

                    while (otro != nullptr) {
                        if (otro != enemigo &&
                            nuevoX < otro->x + 30 && nuevoX + 30 > otro->x &&
                            enemigo->y < otro->y + 30 && enemigo->y + 30 > otro->y) {
                            puedeMoverX = false;
                            break;
                        }
                        otro = otro->Siguiente;
                    }

                    if (puedeMoverX)
                        enemigo->x = nuevoX;

                    // Si llegó a la altura del jugador, pasa al siguiente estado
                    if (enemigo->y >= nave->y)
                        enemigo->estado = 102;

                    break;
                }
                
                case 102: // Fase de reciclaje del enemigo
                    if (enemigo->y<720) 
                {
                    // Reaparecer desde un costado de la pantalla
                    if (enemigo->x < 640)
                        enemigo->x = -30;         // Reaparece por la izquierda
                    else
                        enemigo->x = 1310;        // Reaparece por la derecha

                    enemigo->y = 100;             // Posición vertical fija

                    // Restaurar parámetros para un nuevo ataque
                    enemigo->estado = 101;
                    enemigo->Tiempo = al_get_time();
                    enemigo->Disparo = 3;         // Reiniciar disparos disponibles

                    // El enemigo no se elimina, se recicla para seguir atacando
                }
            }
    }

        enemigo = enemigo->Siguiente;
    }
}

// Crea la nave del jugador
void Game::crearnave()
{
    nave= new personaje();   // Crear nueva instancia de la nave
    nave->bitmap=al_load_bitmap("pictures/nave1.png");        // Cargar imagen normal
    nave->disparobitmap=al_load_bitmap("pictures/nave2.png"); // Cargar imagen de disparo
    nave->alto=al_get_bitmap_height(nave->bitmap);            // Obtener altura de la imagen
    nave->ancho=al_get_bitmap_width(nave->bitmap);            // Obtener ancho de la imagen
    nave->x=900/2;                                            // Centrar horizontalmente
    nave->y= height - 80;                                     // Posicionar cerca del fondo
    nave->tiempo=0;                                           // Inicializar cronómetro
    nave->vida=3;                                             // Comenzar con 3 vidas
}

// Dibuja la nave del jugador en pantalla
void Game::dibujarnave() const
{
     al_draw_scaled_bitmap(nave->bitmap, 0, 0, nave->ancho, nave->alto,nave->x, nave->y,30, 30,0);
}

// Actualiza el movimiento de la nave según la entrada del jugador
void Game::actualizarNave(SystemResources& sys)
 {
    // Verificar límites de la pantalla y detener movimiento si es necesario
    if (nave->x + 30 > 900)    // Si llegó al borde derecho
		derecha = false;
	if (nave->x < 0)           // Si llegó al borde izquierdo
		izquierda = false;
		
    // Aplicar movimiento según las teclas presionadas
	if (derecha)               // Mover a la derecha
		nave->x += 4;
	if (izquierda)             // Mover a la izquierda
		nave->x -= 4;
}

// Crea una nueva bala (del jugador o enemigos)
void Game::crearbala(int dy, int x, int y, SystemResources& sys) {
    ptr_bala nueva = new Bala();
    nueva->x = x;              // Posición horizontal inicial
    nueva->y = y;              // Posición vertical inicial
    nueva->velocidad = dy;     // Velocidad (positiva=arriba, negativa=abajo)
    nueva->siguiente = nullptr;
    agregarBala(Balas, nueva); // Agregar a la lista de balas

    // Reproducir sonido de disparo
    if (sys.shootSound) {
        al_play_sample(sys.shootSound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);
    }
}

// Dibuja todas las balas en pantalla
void Game::dibujarbala() const
{
    ptr_bala aux = Balas;
    while (aux != nullptr) {
        // Dibujar bala como un rectángulo amarillo
        al_draw_filled_rectangle(aux->x, aux->y, aux->x + 4, aux->y + 10, al_map_rgb(255, 255, 0));
        aux = aux->siguiente;
    }
}

// Actualiza la posición de todas las balas y elimina las que salen de pantalla
void Game::actualizarbala(SystemResources& sys)
{
    ptr_bala aux = Balas;
    ptr_bala anterior = nullptr;

    while (aux != nullptr) {
        // Mover la bala según su velocidad
        aux->y -= aux->velocidad;

        // Si la bala salió de la pantalla por arriba, eliminarla
        if (aux->y <= 0) {
            ptr_bala eliminar = aux;

            if (anterior == nullptr) 
            {
                // Es la primera bala de la lista
                Balas = aux->siguiente;
                aux = Balas;
            } else
             {
                // Conectar la anterior con la siguiente
                anterior->siguiente = aux->siguiente;
                aux = aux->siguiente;
            }

            delete eliminar;  // Liberar memoria
        } else {
            // Continuar con la siguiente bala
            anterior = aux;
            aux = aux->siguiente;
        }
    }
}

// Genera las estrellas del fondo espacial
void Game::generateStars() {
    stars.clear();
    for (int i = 0; i < 300; ++i) {
        stars.emplace_back(
            static_cast<float>(rand() % width),    // Posición X aleatoria
            static_cast<float>(rand() % height),   // Posición Y aleatoria
            rand() % 3 // Tipo de estrella: 0=azul, 1=roja, 2=amarilla
        );
    }
}

// Actualiza todos los elementos del juego
void Game::update(SystemResources& sys) {
    // Incrementar el tiempo transcurrido 
    elapsedTime += 1.0f/120.0f; // Asumiendo 120 FPS
    
    // Calcular multiplicador de velocidad (aumenta gradualmente la dificultad)
    speedMultiplier = 1.0f + std::min(elapsedTime / 80.0f, 4.0f); // Máximo 5x de velocidad
    
    // Actualizar estrellas del fondo
    for (auto& star : stars) {
        // Velocidades base diferentes según el tipo de estrella
        float speedBase;
        switch(star.type) {
            case 0: speedBase = 0.5f; break;  // Estrellas azules lentas
            case 1: speedBase = 1.0f; break;  // Estrellas rojas medias
            case 2: speedBase = 1.5f; break;  // Estrellas amarillas rápidas
        }
        
        star.y += speedBase * speedMultiplier;
        
        // Si la estrella sale por abajo, reaparecerla arriba
        if (star.y > height) {
            star.y = 0;
            star.x = rand() % width;
        }
    }
    
    // Manejar animación de la nave al disparar
    if (nave->bitmap == nave->disparobitmap) // Si está mostrando imagen de disparo
    {
        if (al_get_time() - nave->tiempo > 1.0) // Si ya pasó 1 segundo
        {
            nave->bitmap = al_load_bitmap("pictures/nave1.png"); // Volver a imagen normal
        }
    }
    
    // Sistema de ataques enemigos cada 4 segundos
    if (al_get_time() - tiempoUltimoAtaque >= 4)
    {
        ptr_est aux = enemigos;

        while (aux != nullptr) {
            // Buscar un enemigo que pueda atacar
            if (aux->ataque == 0 && puedeAtacar(aux)) 
            {
                aux->ataque = 1;      // Activar modo ataque
                aux->estado = 100;    // Estado inicial de ataque

                break; // Solo un atacante a la vez
            }
            aux = aux->Siguiente;
        }
    
    tiempoUltimoAtaque = al_get_time();
    }
    
    // Verificar si se completó el nivel (no quedan enemigos)
    if(enemigos==nullptr)
    {
        nivel++;                    // Avanzar al siguiente nivel
        limpiarbalas();            // Limpiar balas restantes
        crearnivel();              // Crear nueva formación de enemigos
        tiempoUltimoAtaque = al_get_time(); // Reiniciar cronómetro de ataques
    }
    
    // Actualizar todos los sistemas del juego
    actualizarenemigos(sys);
    colisiones(sys);
    actualizarNave(sys);
    actualizarbala(sys);
}

// Dibuja todos los elementos del juego en pantalla
void Game::draw() const {
    al_clear_to_color(al_map_rgb(5, 2, 10)); // Fondo oscuro del espacio
    
    // INTERFACE DE USUARIO
    // Preparar textos de puntuación
    std::string scoreText = "SCORE: " + std::to_string(currentScore);
    std::string highScoreText = "HIGH SCORE: " + std::to_string(highScore);
    
    // Dibujar puntaje actual (izquierda, blanco)
    al_draw_text(font, al_map_rgb(255, 255, 255), 20, 10, 
                 ALLEGRO_ALIGN_LEFT, scoreText.c_str());
    
    // Dibujar puntuación más alta (derecha, rojo)
    al_draw_text(font, al_map_rgb(255, 0, 0), width - 20, 10, 
                 ALLEGRO_ALIGN_RIGHT, highScoreText.c_str());
    
    //  BANNER DE VIDAS
    int bannerHeight = 40;                    // Altura del banner
    int bannerY = height - bannerHeight;      // Posición Y del banner
    
    // Dibujar fondo del banner (negro semi-transparente)
    al_draw_filled_rectangle(0, bannerY, width, height, 
                           al_map_rgba(0, 0, 0, 200));
    
    // Dibujar iconos de vidas como pequeñas naves
    int livesX = 30;                          // Margen izquierdo
    int livesY = bannerY + 8;                 // Centrado en el banner
    int spacing = 35;                         // Espacio entre naves
    
    for (int i = 0; i < nave->vida; ++i) {
        if (livesSprite) {
            al_draw_scaled_bitmap(livesSprite, 
                                0, 0, 
                                al_get_bitmap_width(livesSprite), 
                                al_get_bitmap_height(livesSprite),
                                livesX + (i * spacing), livesY, 
                                22, 22, // Tamaño pequeño
                                0);
        }
    }
    
    // ELEMENTOS DEL JUEGO 
    dibujarenemigos();
    dibujarnave();
    dibujarbala();
    
    // ESTRELLAS DEL FONDO 
    float t = al_get_time(); // Tiempo actual para efectos de animación

    for (const auto& star : stars) {
        // Efecto de parpadeo basado en el tiempo y posición
        float twinkle = 0.6f + 0.4f * std::sin(t * 2 + star.x * 0.1f);
        
        if (star.type == 0) {
            // Estrellas azules
            al_draw_pixel(star.x, star.y, al_map_rgb(
                50 + 50 * twinkle,
                150 + 50 * twinkle,
                200 + 55 * twinkle
            ));
        } else if (star.type == 1) {
            // Estrellas rojas  
            al_draw_pixel(star.x, star.y, al_map_rgb(
                200 + 55 * twinkle,
                50 + 50 * twinkle,
                30 * twinkle
            ));
        } 
        else {
            // Estrellas amarillas
            al_draw_pixel(star.x, star.y, al_map_rgb(
                200 + 55 * twinkle,
                180 + 75 * twinkle,
                100 + 50 * twinkle
            ));
        }
    }

    al_flip_display(); // Mostrar todo en pantalla
}

// Bucle principal del juego
int Game::run(SystemResources& sys) {
    ALLEGRO_EVENT event;
    bool running = true;

    al_start_timer(sys.timer); // Iniciar el cronómetro del juego

    while (running) {
       al_wait_for_event(sys.eventQueue, &event); // Esperar eventos
       
        // MANEJO DE TECLAS PRESIONADAS
        if (event.type == ALLEGRO_EVENT_KEY_DOWN)
	    {
			if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT)
			{
				derecha = true;  // Activar movimiento a la derecha
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_LEFT)
			{
				izquierda = true; // Activar movimiento a la izquierda
			}
	    }
	    
        //MANEJO DE TECLAS LIBERADAS 
	    if (event.type == ALLEGRO_EVENT_KEY_UP)
	    {
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT)
            {
                        derecha = false; // Detener movimiento a la derecha
            }
            if (event.keyboard.keycode == ALLEGRO_KEY_LEFT)
            {
                        izquierda = false; // Detener movimiento a la izquierda
            }
    	}
    	
        //MANEJO DE CIERRE DE VENTANA
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            // Limpiar recursos y salir
            limpiarbalas();
            limpiarenemigos();
            nivel=1;
            running = false;
        } 
        // MANEJO DE TECLA ESCAPE 
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                // Limpiar recursos y salir
                limpiarbalas();
                limpiarenemigos();
                nivel=1;
                running = false;
            }
        } 
        
        //  MANEJO DE DISPARO 
        if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) 
        {
            // Solo disparar si no está ya en animación de disparo
            if (!(nave->bitmap == nave->disparobitmap)) 
            {
                nave->bitmap = nave->disparobitmap;  // Cambiar a imagen de disparo
                nave->tiempo = al_get_time();        // Guardar tiempo del cambio
                crearbala(3, nave->x+15, nave->y-30, sys); // Crear bala del jugador
            }
        }
        
        // VERIFICAR GAME OVER
        if(nave->vida<=0)
        {
            // Limpiar todo y terminar el juego
            limpiarbalas();
            limpiarenemigos();
            nivel=1;
            running=false;
        }
        // ACTUALIZACIÓN DEL JUEGO 
        else if (event.type == ALLEGRO_EVENT_TIMER) {
            update(sys);  // Actualizar lógica del juego
            draw();       // Dibujar todo en pantalla
        }
    }

    return 0; // Retornar código de salida
}

// Obtiene la puntuación actual del jugador
int Game::getCurrentScore() const {
    return currentScore;
}