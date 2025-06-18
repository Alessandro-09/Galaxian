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

using namespace std;
int nivel=1;
bool derecha = false, izquierda = false;
typedef struct  Bala
{
    float x, y;
    float velocidad;
    Bala* siguiente;
}*ptr_bala;

typedef struct personaje
{
    ALLEGRO_BITMAP*bitmap;
    ALLEGRO_BITMAP* disparobitmap; 
    int ancho;
    int alto;
    float x,y;
    double tiempo;
}*ptr_nave;
typedef struct navesenemigas
{
    ALLEGRO_BITMAP* bitmap;
    int col;
    int fila;
    float origenx, origeny;
    int ancho;
    int alto;
    float x, y;
    float dx, dy;
    int ataque;
    int estado;

   navesenemigas* Siguiente; 
}*ptr_est;

ptr_bala Balas;
ptr_est enemigos;
ptr_nave nave;
void agregaralfinal(ptr_est& lista, ptr_est Nuevo)
{
	ptr_est Aux;//Puntero creado
	Aux = lista;//Puntero apunta a la misma dirección que Lista
	if (Aux != NULL)//si la lista no esta vacia
	{
		while (Aux->Siguiente != NULL)//Puntero siguiente no es null entonces
		{
			Aux = Aux->Siguiente;//el Aux apunta a lo que apuntaba el puntero siguiente
		}

		Aux->Siguiente = Nuevo;//el puntero siguiente del elemento ahora apunta al nuevo elemento
	}
	else//si la lista esta vacia
	{
		lista = Nuevo;//Lista apunta al nuevo elemento
	}
}
void agregarBala(ptr_bala& lista, ptr_bala Nuevo)
{
	ptr_bala Aux;//Puntero creado
	Aux = lista;//Puntero apunta a la misma dirección que Lista
	if (Aux != NULL)//si la lista no esta vacia
	{
		while (Aux->siguiente != NULL)//Puntero siguiente no es null entonces
		{
			Aux = Aux->siguiente;//el Aux apunta a lo que apuntaba el puntero siguiente
		}

		Aux->siguiente = Nuevo;//el puntero siguiente del elemento ahora apunta al nuevo elemento
	}
	else//si la lista esta vacia
	{
		lista = Nuevo;//Lista apunta al nuevo elemento
	}
}

Game::Game(ALLEGRO_FONT* font, int width, int height) 
    : font(font), width(width), height(height), 
      starSpeed(1.0f), speedMultiplier(1.0f), elapsedTime(0.0f) {  // Velocidad inicial 1.0
    crearnivel();
    crearnave();
    generateStars();
}
void Game::dibujarenemigos() const
{
    ptr_est aux = enemigos;
    while (aux != nullptr) {
        // Dibuja la imagen original escalada a 64x64 en (200, 100)
        al_draw_scaled_bitmap(aux->bitmap, 0, 0, aux->ancho, aux->alto,aux->x, aux->y,30, 30,0);
        aux = aux->Siguiente;
    }
}
void Game::crearnivel()
{
     ptr_est nuevo;
    int x=190;
    int y=40;
    int col =10;
	for (int fila = 0; fila < 5; fila++)//por 5 filas
	{
        const char* ruta;
        if( fila==0)
        {
            ruta="pictures/navenemiga1.png";
            x=250;
            col=7;
        }
        else if( fila==1)
        {
            ruta="pictures/navenemiga2.png";
            x=200;
            col=9;
        }
         else if( fila==2)
         {
            ruta="pictures/navenemiga4.png";
            col=11;
        }
         else
         {
            ruta="pictures/navenemiga3.png";
         }
		for (int i = 0; i < col; i++)//cantidad bloques
		{

            nuevo= new navesenemigas();
            nuevo->bitmap=al_load_bitmap(ruta);
            nuevo->alto= al_get_bitmap_height(nuevo->bitmap);
            nuevo->ancho=al_get_bitmap_width(nuevo->bitmap);
            nuevo->x=x;
            nuevo->y=y;
            nuevo->origenx=x;
            nuevo->origeny=y;
            nuevo->col=i;
            nuevo->fila=fila;
            nuevo->dx = 1;
            nuevo->dy = 0;
            nuevo->ataque=0;
            nuevo->estado=0;
            nuevo->Siguiente=nullptr;
            agregaralfinal(enemigos, nuevo);
            x+=40;
         }
         y+=60;
         x=170;

    }
    
}

void Game::colisiones()
{
    ptr_bala aux=Balas;
    ptr_bala aux2=nullptr;
 
    while (aux != nullptr)
    {
        ptr_est enemigo = enemigos;
        ptr_est enemigo2 = nullptr;
        bool colision = false;

        while (enemigo != nullptr && !colision)
        {
            if (!(aux->x + 30 < enemigo->x || aux->x > enemigo->x + 30 ||
                  aux->y + 30 < enemigo->y || aux->y > enemigo->y + 30))
            {
                // Eliminar enemigo
                if (enemigo2 == nullptr)
                    enemigos = enemigo->Siguiente;
                else
                    enemigo2->Siguiente = enemigo->Siguiente;

                delete enemigo;

                // Eliminar bala
                ptr_bala borrar = aux;
                if (aux2 == nullptr)
                    Balas = aux->siguiente;
                else
                    aux2->siguiente = aux->siguiente;

                aux = borrar->siguiente;
                delete borrar;

                colision = true;
            }
            else
            {
                enemigo2 = enemigo;
                enemigo = enemigo->Siguiente;
            }
        }

        if (!colision)
        {
            aux2 = aux;
            aux = aux->siguiente;
        }
    }
}
void Game::actualizarenemigos()
{
    ptr_est enemigo = enemigos;
    float velocidad = 0.15f;
    int rango = 20;

    while (enemigo != NULL) {
        if (enemigo->ataque != 1) {
            switch (enemigo->estado) {
                case 0: // Bajando
                    enemigo->y += velocidad;
                    if (enemigo->y >= enemigo->origeny + rango) {
                        enemigo->estado = 1; // Cambia a volver al origen
                    }
                    break;

                case 1: // Volviendo al origen desde abajo
                    enemigo->y -= velocidad;
                    if (enemigo->y <= enemigo->origeny) {
                        enemigo->estado = 2; // Cambia a subir
                    }
                    break;

                case 2: // Subiendo
                    enemigo->y -= velocidad;
                    if (enemigo->y <= enemigo->origeny - rango) {
                        enemigo->estado = 3; // Cambia a volver al origen desde arriba
                    }
                    break;

                case 3: // Volviendo al origen desde arriba
                    enemigo->y += velocidad;
                    if (enemigo->y >= enemigo->origeny) {
                        enemigo->estado = 0; // Vuelve a bajar
                    }
                    break;
            }
            enemigo->x += -1 * enemigo->dx;

            if (enemigo->x >= enemigo->origenx + 100 ||
                enemigo->x <= enemigo->origenx - 100)
            {
                enemigo->dx *= -1; // Cambia de dirección
            }
        }

        enemigo = enemigo->Siguiente;
    }
}

void Game::crearnave()
{
    nave= new personaje();
    nave->bitmap=al_load_bitmap("pictures/nave1.png");
    nave->disparobitmap=al_load_bitmap("pictures/nave2.png");
    nave->alto=al_get_bitmap_height(nave->bitmap);
    nave->ancho=al_get_bitmap_width(nave->bitmap);
    nave->x=800/2;
    nave->y= 550;
    nave->tiempo=0;
}
void Game::dibujarnave() const
{
     al_draw_scaled_bitmap(nave->bitmap, 0, 0, nave->ancho, nave->alto,nave->x, nave->y,30, 30,0);
}
void Game::actualizarNave() {

    if (nave->x + 30 > 800)
		derecha = false;
	if (nave->x < 0)
		izquierda = false;
	if (derecha)
		nave->x += 4;
	if (izquierda)
		nave->x -= 4;
}

void Game::crearbala()
{
    ptr_bala nueva=new Bala();
    nueva->x=nave->x+15;
    nueva->y=nave->y+30;
    nueva->velocidad=3;
    nueva->siguiente=nullptr;
    agregarBala(Balas,nueva);

}
void Game::dibujarbala() const
{
    ptr_bala aux = Balas;
    while (aux != nullptr) {
        // Dibuja la imagen original escalada a 64x64 en (200, 100)
        al_draw_filled_rectangle(aux->x, aux->y, aux->x + 4, aux->y + 10, al_map_rgb(255, 255, 0));
        aux = aux->siguiente;
    }
}
void Game::actualizarbala()
{
    ptr_bala aux = Balas;
    ptr_bala anterior = nullptr;

    while (aux != nullptr) {
        // Mover la bala
        aux->y -= aux->velocidad;

        if (aux->y <= 0) {
            ptr_bala eliminar = aux;

            if (anterior == nullptr) {
                // Es la primera bala
                Balas = aux->siguiente;
                aux = Balas;
            } else {
                anterior->siguiente = aux->siguiente;
                aux = aux->siguiente;
            }

            delete eliminar;
        } else {
            // Solo avanzamos si no eliminamos
            anterior = aux;
            aux = aux->siguiente;
        }
    }
}

void Game::generateStars() {
    stars.clear();
    for (int i = 0; i < 300; ++i) {
        stars.emplace_back(
            static_cast<float>(rand() % width),
            static_cast<float>(rand() % height),
            rand() % 3 // Tipo de estrella: 0=azul, 1=roja, 2=amarilla
        );
    }
}

void Game::update() {
    // Aumentar el tiempo transcurrido 
    elapsedTime += 1.0f/120.0f; // 120 FPS
    
    // AJUSTE DE VELOCIDAD (1): Velocidad inicial 1.0x, llega a 5x en 320 segundos
    speedMultiplier = 1.0f + std::min(elapsedTime / 80.0f, 4.0f); // Máximo 5x de velocidad
    
    for (auto& star : stars) {
        // AJUSTE DE VELOCIDAD (2): Velocidades base por tipo de estrella
        float speedBase;
        switch(star.type) {
            case 0: speedBase = 0.5f; break;  // Estrellas azules lentas
            case 1: speedBase = 1.0f; break;  // Estrellas rojas medias
            case 2: speedBase = 1.5f; break;  // Estrellas amarillas rápidas
        }
        
        star.y += speedBase * speedMultiplier;
        
        if (star.y > height) {
            star.y = 0;
            star.x = rand() % width;
        }
        
    }
    if (nave->bitmap == nave->disparobitmap) 
    {
        if (al_get_time() - nave->tiempo > 1.0) 
        {
            nave->bitmap = al_load_bitmap("pictures/nave1.png");
        }
    }
    actualizarenemigos();
    colisiones();
    actualizarNave();
    actualizarbala();
}

void Game::draw() const {
    al_clear_to_color(al_map_rgb(5, 2, 10)); // Fondo oscuro del espacio
    dibujarenemigos();
    dibujarnave();
    dibujarbala();
    float t = al_get_time(); // Tiempo actual para efectos de animación

    for (const auto& star : stars) {
        float twinkle = 0.6f + 0.4f * std::sin(t * 2 + star.x * 0.1f);  // Efecto de parpadeo
        
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

    al_flip_display();
}

int Game::run(SystemResources& sys) {
    ALLEGRO_EVENT event;
    bool running = true;

    // Detener cualquier música que esté sonando al entrar al juego
    if (sys.menuMusic && al_get_audio_stream_playing(sys.menuMusic)) {
        al_set_audio_stream_playing(sys.menuMusic, false);
    }
    if (sys.instructionsMusic && al_get_audio_stream_playing(sys.instructionsMusic)) {
        al_set_audio_stream_playing(sys.instructionsMusic, false);
    }

    al_start_timer(sys.timer);

    while (running) {
       al_wait_for_event(sys.eventQueue, &event);
        if (event.type == ALLEGRO_EVENT_KEY_DOWN)
	    {
			if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT)
			{
				derecha = true;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_LEFT)
			{
				izquierda = true;
			}
	    }
			// si el teclado ya no esta presionado
	    if (event.type == ALLEGRO_EVENT_KEY_UP)
	    {
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT)
            {
                        derecha = false;
            }
            if (event.keyboard.keycode == ALLEGRO_KEY_LEFT)
            {
                        izquierda = false;
            }
    	}
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false; 
        } 
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                running = false;
            }
        } 
        
        if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) 
        {
            if (!(nave->bitmap == nave->disparobitmap)) 
            {
                nave->bitmap = nave->disparobitmap;
                nave->tiempo = al_get_time(); // guardar tiempo del cambio
                crearbala();
            }
        }

        else if (event.type == ALLEGRO_EVENT_TIMER) {
            update();
            draw();
        }
        

    }

    return 0;
}
