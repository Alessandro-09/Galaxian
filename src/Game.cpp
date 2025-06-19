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
int nivel=1; //variable para nivel
bool derecha = false, izquierda = false;//comprobadores de bandera para el movimiento de la nave
double tiempoUltimoAtaque = 0;//se toma para su tiempo de ataque de las naves enemigas
typedef struct  Bala// estrutura para la lista enlazada de las balas
{
    float x, y;         //se creean su atributos x, y para posición
    float velocidad;    //se crea su atributo de velocidad para su movimiento
    Bala* siguiente;    //puntero siguiente para unir la lista enlazada
}*ptr_bala;             // puntero a la estructura

typedef struct personaje//se crea una estructura para la nave
{
    ALLEGRO_BITMAP*bitmap; //se crea la variable bitmap para el uso de la imagen
    ALLEGRO_BITMAP* disparobitmap; //atribuyo para el cambio al disparar
    int ancho;             //atributo que va a contener el ancho
    int alto;              //atributo que va a contener el alto
    float x,y;             //atributo que va llevar la posición de la nave
    double tiempo;         //atributo para el tiempo de cambio entre las imagenes de la nave
    int vida;
}*ptr_nave;

typedef struct navesenemigas//se crea una estructura para los enemigos
{
    ALLEGRO_BITMAP* bitmap;//se crea la variable bitmap para el uso de la imagen
    int col;               //se guarda la columna a la que pertenece
    int fila;              //se guarda la fila a la que pertenece
    float origenx, origeny;//se guarda la posición original
    int ancho;             //atributo que va a contener el ancho
    int alto;              //atributo que va a contener el alto
    float x, y;            //atributo que va llevar la posición de la nave
    float dx, dy;          //se crea atributos para la velocidad
    int ataque;            //se crea atributo para el ataque entre 1 y 0
    int estado;            //se crea atributo para el estado de movimiento con codigos 0,1,2,3,100,101,102 hay que agregar más estados usen codigos 
    int Disparo;
    double Tiempo;
    int salio=0;
   navesenemigas* Siguiente; 
}*ptr_est;

ptr_bala Balas;           //lista que va a contener las balas
ptr_est enemigos;         //lista que va a contener los enemigos
ptr_nave nave;            //lista que va a contener la nave
void agregaralfinal(ptr_est& lista, ptr_est Nuevo)
{
	ptr_est Aux;//Puntero creado
    Nuevo->salio=0;
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
void limpiarenemigos() // se encarga de limpiar la cola de enemigos
{
    ptr_est aux = enemigos;
    while (aux != nullptr)
    {
        ptr_est temp = aux;
        aux = aux->Siguiente;
        delete temp;
    }
    enemigos = nullptr; // Importante para evitar punteros colgantes
}
void limpiarbalas()
{
    ptr_bala aux = Balas;
    while (aux != nullptr)
    {
        ptr_bala temp = aux;
        aux = aux->siguiente;
        delete temp;
    }
    Balas = nullptr; // Importante para evitar punteros colgantes
}


bool puedeAtacar(ptr_est e) {
    ptr_est aux = enemigos;

    while (aux != nullptr) {
        if (aux != e &&
        aux->x < e->x + 30 && aux->x + 30 > e->x && // colisión horizontal
        aux->fila > e->fila) 
            return false; // Hay alguien debajo
        aux = aux->Siguiente;
    }

    return true; // Nadie debajo, puede atacar
}

Game::Game(ALLEGRO_FONT* font, int width, int height) 
    : font(font), width(width), height(height), 
      currentScore(0), highScore(0),
      starSpeed(1.0f), speedMultiplier(1.0f), elapsedTime(0.0f) {

    // Crear fuente pequeña para puntajes 
    smallFont = al_load_ttf_font("assets/space_font.ttf", 18, 0);
    if (!smallFont) {
    // Si no se puede cargar, usar la fuente normal
        smallFont = font;
    }

    // Cargar sprite específico para las vidas (siempre nave1.png)
    livesSprite = al_load_bitmap("pictures/nave1.png");


    crearnivel();
    crearnave();
    generateStars();
    
    // Cargar high score
    HighScore hs(font, width, height);
    auto topScores = hs.getTopScores(1);
    if (!topScores.empty()) {
        highScore = topScores[0].score;
    }
}

Game::~Game() {
    // Liberar la fuente pequeña solo si es diferente de la principal
    if (smallFont && smallFont != font) {
        al_destroy_font(smallFont);
    }
    // Liberar imagen de explosión
    if (explosionImg) {
        al_destroy_bitmap(explosionImg);
    }
    
    // Liberar sprite de vidas
    if (livesSprite) {
        al_destroy_bitmap(livesSprite);
    }
}

void Game::dibujarenemigos() const
{
    ptr_est aux = enemigos;
    while (aux != nullptr) {
        //se toma ancho y alto para poder reescalar la imagen
        al_draw_scaled_bitmap(aux->bitmap, 0, 0, aux->ancho, aux->alto,aux->x, aux->y,30, 30,0);
        aux = aux->Siguiente;
    }
}

void Game::crearnivel() //para creación de nivel
{
    ptr_est nuevo; //se crea un punteroa nuevo elmento de tipo enemigos
    int x=190;                                  //se establece un x fijo para acomodar las naves
    int y=40;                                   //se crea el y para el comienzo
    int col =10;                                //se crea la cantidad de columnas
    const char* ruta1="pictures/navenemiga1.png";
    const char* ruta2="pictures/navenemiga2.png";
    const char* ruta3="pictures/navenemiga3.png";
    const char* ruta4="pictures/navenemiga4.png";
    const char* ruta;
    switch (nivel)
    {
    case 1:
        for (int fila = 0; fila < 5; fila++)        //por 5 filas
        {
            switch (fila)
            {
            case 0:
                ruta=ruta1;
                col=1;
                x=250;
                break;
           case 1:
           {
                ruta=ruta2;
                col=2;
                x=225;
                break;
            }
            case 2:
                ruta=ruta3;
                col=4;
                x=200;
                break;
            default:
                ruta=ruta4;
                break;
             }
             for (int i = 0; i < col; i++)           //cantidad naves 
            {

                nuevo= new navesenemigas();         // se ccea nuevo elemento de navesenemigas
                nuevo->bitmap=al_load_bitmap(ruta); //se carga la imagen
                nuevo->alto= al_get_bitmap_height(nuevo->bitmap); //se carga su alto y ancho segun la imagen
                nuevo->ancho=al_get_bitmap_width(nuevo->bitmap);
                nuevo->x=x;                         //se pasa el x de dibujo
                nuevo->y=y;                         //se pasas el y
                nuevo->origenx=x;                   //se pasa para guadar posición de origen 
                nuevo->origeny=y;                   //se para guardar posición de origen y
                nuevo->col=i+4-col;                // se calcula su colomna
                nuevo->fila=fila;                   //se pasa su fila 
                nuevo->dx = 1;                      //se le da una velocidad lateral
                nuevo->dy = 0;                      //se pone y en 0
                nuevo->ataque=0;                    //se le pone estado de no ataque
                nuevo->estado=0;                    //se pone estadode no movimiento
                nuevo->Siguiente=nullptr;
                nuevo->Disparo=1;
                nuevo->Tiempo=0;
                nuevo->salio=0;
                agregaralfinal(enemigos, nuevo);    //se pasa par agregar a la lista
                x+=40;
            }
            y+=60;
            x=200;
        }
        break;
    case 2:
        for (int fila = 0; fila < 5; fila++)        //por 5 filas
        {
            switch (fila)
            {
            case 0:
                ruta=ruta1;
                col=2;
                x=250;
                break;
           case 1:
           {
                ruta=ruta2;
                col=3;
                x=225;
                break;
            }
            case 2:
                ruta=ruta3;
                col=5;
                x=200;
                break;
            default:
                ruta=ruta4;
                break;
             }
             for (int i = 0; i < col; i++)           //cantidad naves 
            {

                nuevo= new navesenemigas();         // se ccea nuevo elemento de navesenemigas
                nuevo->bitmap=al_load_bitmap(ruta); //se carga la imagen
                nuevo->alto= al_get_bitmap_height(nuevo->bitmap); //se carga su alto y ancho segun la imagen
                nuevo->ancho=al_get_bitmap_width(nuevo->bitmap);
                nuevo->x=x;                         //se pasa el x de dibujo
                nuevo->y=y;                         //se pasas el y
                nuevo->origenx=x;                   //se pasa para guadar posición de origen 
                nuevo->origeny=y;                   //se para guardar posición de origen y
                nuevo->col=i+5-col;                // se calcula su colomna
                nuevo->fila=fila;                   //se pasa su fila 
                nuevo->dx = 1.25;                      //se le da una velocidad lateral
                nuevo->dy = 0;                      //se pone y en 0
                nuevo->ataque=0;                    //se le pone estado de no ataque
                nuevo->estado=0;                    //se pone estadode no movimiento
                nuevo->Siguiente=nullptr;
                nuevo->Disparo=1;
                nuevo->Tiempo=0; 
                agregaralfinal(enemigos, nuevo);    //se pasa par agregar a la lista
                x+=40;
            }
            y+=60;
            x=200;
        }
        break;
    case 3:
        for (int fila = 0; fila < 5; fila++)        //por 5 filas
        {
            switch (fila)
            {
            case 0:
                ruta=ruta1;
                col=3;
                x=250;
                break;
           case 1:
           {
                ruta=ruta2;
                col=5;
                x=225;
                break;
            }
            case 2:
                ruta=ruta3;
                col=7;
                x=200;
                break;
            default:
                ruta=ruta4;
                break;
             }
             for (int i = 0; i < col; i++)           //cantidad naves 
            {

                nuevo= new navesenemigas();         // se ccea nuevo elemento de navesenemigas
                nuevo->bitmap=al_load_bitmap(ruta); //se carga la imagen
                nuevo->alto= al_get_bitmap_height(nuevo->bitmap); //se carga su alto y ancho segun la imagen
                nuevo->ancho=al_get_bitmap_width(nuevo->bitmap);
                nuevo->x=x;                         //se pasa el x de dibujo
                nuevo->y=y;                         //se pasas el y
                nuevo->origenx=x;                   //se pasa para guadar posición de origen 
                nuevo->origeny=y;                   //se para guardar posición de origen y
                nuevo->col=i+7-col;                // se calcula su colomna
                nuevo->fila=fila;                   //se pasa su fila 
                nuevo->dx = 1.5;                      //se le da una velocidad lateral
                nuevo->dy = 0;                      //se pone y en 0
                nuevo->ataque=0;                    //se le pone estado de no ataque
                nuevo->estado=0;                    //se pone estadode no movimiento
                nuevo->Siguiente=nullptr;
                nuevo->Disparo=1;
                nuevo->Tiempo=0;
                agregaralfinal(enemigos, nuevo);    //se pasa par agregar a la lista
                x+=40;
            }
            y+=60;
            x=200;
        }
        break;
    default:
        for (int fila = 0; fila < 5; fila++)        //por 5 filas
        {
            switch (fila)
            {
            case 0:
                ruta=ruta1;
                col=4;
                x=250;
                break;
           case 1:
           {
                ruta=ruta2;
                col=6;
                x=225;
                break;
            }
            case 2:
                ruta=ruta3;
                col=8;
                x=200;
                break;
            default:
                ruta=ruta4;
                break;
             }
            for (int i = 0; i < col; i++)           //cantidad naves 
            {

                nuevo= new navesenemigas();         // se ccea nuevo elemento de navesenemigas
                nuevo->bitmap=al_load_bitmap(ruta); //se carga la imagen
                nuevo->alto= al_get_bitmap_height(nuevo->bitmap); //se carga su alto y ancho segun la imagen
                nuevo->ancho=al_get_bitmap_width(nuevo->bitmap);
                nuevo->x=x;                         //se pasa el x de dibujo
                nuevo->y=y;                         //se pasas el y
                nuevo->origenx=x;                   //se pasa para guadar posición de origen 
                nuevo->origeny=y;                   //se para guardar posición de origen y
                nuevo->col=i+8-col;                // se calcula su colomna
                nuevo->fila=fila;                   //se pasa su fila 
                nuevo->dx = 2;                      //se le da una velocidad lateral
                nuevo->dy = 0;                      //se pone y en 0
                nuevo->ataque=0;                    //se le pone estado de no ataque
                nuevo->estado=0;                    //se pone estadode no movimiento
                nuevo->Siguiente=nullptr;
                nuevo->Disparo=1;
                nuevo->Tiempo=0;
                agregaralfinal(enemigos, nuevo);    //se pasa par agregar a la lista
                x+=40;
            }
            y+=60;
            x=200;

    }
    break;
    }
	
    
}

void Game::colisiones(SystemResources& sys)                        //encargada de ciertas colisiones
{
    ptr_bala aux=Balas;                        //puntero que apunta a lo mismo que la lista
    ptr_bala aux2=nullptr;                     //puntero auxiliar
 
    while (aux != nullptr)
    {
        ptr_est enemigo = enemigos;           //puntero que apunta a lo mismo que lista enemigos y luego su auxiliar
        ptr_est enemigo2 = nullptr;
        bool colision = false;                //flag de colisión

        while (enemigo != nullptr && !colision)//se verifica si hay colision y se se acabo la lista
        {
            if (!(aux->x + 30 < enemigo->x || aux->x > enemigo->x + 30 ||
                  aux->y + 30 < enemigo->y || aux->y > enemigo->y + 30))//todo negado se calcula segun lo siguiente si la bala se encuentra entre su posición +30  y si el enemigo más 30 es mayor
            {
                // Agregar puntos según el tipo de enemigo
                if (enemigo->fila == 0) currentScore += 100;      //Primera fila: 100 puntos
                else if (enemigo->fila == 1) currentScore += 75;  //Segunda fila: 75 puntos
                else if (enemigo->fila == 2) currentScore += 50;  //Tercera fila: 50 puntos
                else currentScore += 25;                           //Otras filas: 25 puntos


                // Reproducir sonido de impacto
                if (sys.hitEnemySound) {
                    al_play_sample(sys.hitEnemySound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);
                }
                
                // Eliminar enemigo
                if (enemigo2 == nullptr)// se toma si el auxiliar esta en blanco
                    enemigos = enemigo->Siguiente;// entonces la lista pasa al siguiente
                else
                    enemigo2->Siguiente = enemigo->Siguiente;//si no enemigo 2 siguiente va a ser el siguiente del enemigo cambe destacar que enemigo 2 es el anterior

                delete enemigo;  //se elimina

                // se hace lo mismo con la bala
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
                enemigo2 = enemigo;// si no enemigo 2 toma el actual
                enemigo = enemigo->Siguiente;// y enemigo pasa al siguiente
            }
        }

        if (!colision)
        {
            aux2 = aux;
            aux = aux->siguiente;
        }
    }
    aux = Balas;
    aux2 = nullptr;
    bool colision = false; // Flag de colisión

    // Colisiones entre balas y la nave
    while (aux != nullptr)
    {
        if (!(aux->x + 30 < nave->x || aux->x > nave->x + 30 ||
            aux->y+15 < nave->y || aux->y > nave->y+15 ))
        {
            if (sys.hitPlayerSound) {
                al_play_sample(sys.hitPlayerSound, 1.5f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, nullptr);
            }

            ptr_bala borrar = aux;
            if (aux2 == nullptr)
                Balas = aux->siguiente;
            else
                aux2->siguiente = aux->siguiente;

            aux = borrar->siguiente;
            delete borrar;
            nave->vida -= 1;
            colision = true; // ← Se marca que hubo colisión
            cout << "colision" << std::endl;
            continue; // Saltar incremento de aux2 (ya lo hicimos al eliminar)
        }

        aux2 = aux;
        aux = aux->siguiente;
    }

    // Colisiones entre enemigos y la nave
    ptr_est enemigo = enemigos;
    ptr_est enemigo2 = nullptr;

    while (enemigo != nullptr && !colision) // ← Aquí colision ahora sí tiene efecto
    {
        if (!(enemigo->x + 30 < nave->x || enemigo->x > nave->x + 30 ||
            enemigo->y + 30 < nave->y || enemigo->y > nave->y + 30))
        {
            if (sys.hitPlayerSound) {
                al_play_sample(sys.hitPlayerSound, 1.5f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, nullptr);
            }
            
            cout << "colision" << std::endl;
            nave->vida -= 1;
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
void Game::actualizarenemigos(SystemResources& sys)
{
    ptr_est enemigo = enemigos;
    float velocidad = 0.15f;//se pone velocidad
    int rango = 20;         //desde un rango de 20 

    while (enemigo != NULL) {
        if (enemigo->ataque != 1) //si no esta atacando
        {
            switch (enemigo->estado) //se lee su estado
            {
                case 0: //caso de moviento vertical abajo
                    enemigo->y += velocidad; //se le suma la velocidad a y
                    if (enemigo->y >= enemigo->origeny + rango) //y si llego al rango regresa
                    {
                        enemigo->estado = 1; // Cambia el estado
                    }
                    break;

                case 1: // Volviendo al origen desde abajo
                    enemigo->y -= velocidad;
                    if (enemigo->y <= enemigo->origeny)//si llego al origen
                     {
                        enemigo->estado = 2; // Cambia a subir
                    }
                    break;

                case 2: // Subiendo
                    enemigo->y -= velocidad;
                    if (enemigo->y <= enemigo->origeny - rango) //si su movimiento llego al limite
                    {
                        enemigo->estado = 3; // Cambia a volver al origen desde arriba
                    }
                    break;

                case 3: // Volviendo al origen desde arriba
                    enemigo->y += velocidad;
                    if (enemigo->y >= enemigo->origeny)//volvio al origen
                     {
                        enemigo->estado = 0; // Vuelve a bajar
                    }
                    break;
            }
            enemigo->x += -1 * enemigo->dx; //se hace al mismo tiempo movimiento horizontal con dx definido

            if (enemigo->x >= enemigo->origenx + 150 ||
                enemigo->x <= enemigo->origenx - 150) //si se llego aciertas posiciones
            {
                enemigo->dx *= -1; // Cambia de dirección
            }
        } 
        if (enemigo->ataque == 1) //en proceso ataque 
        {
            switch (enemigo->estado) 
            {
                case 100: // Salida inicial (baja hasta y = 340)
                {
                    float n=0;
                    if(nivel>3)
                    {
                        n=2;
                    }
                    else
                    {
                        n=nivel/2;
                    }
                    if (fabs(enemigo->y - 340.0f) > 1.0f)
                        enemigo->y += 0.5f * ((340.0f > enemigo->y) ? 1+n : -2-n);

                    // Llegó al punto de ataque
                    if (fabs(enemigo->y - 340.0f) <= 1.0f) {
                        enemigo->estado = 101;
                    }
                    enemigo->x += -1 * enemigo->dx; //se hace al mismo tiempo movimiento horizontal con dx definido

                    if (enemigo->x >= enemigo->origenx + 150 ||
                        enemigo->x <= enemigo->origenx - 150) //si se llego aciertas posiciones
                    {
                        enemigo->dx *= -1; // Cambia de dirección
                    }
                    break;
                }
                case 101: // Descenso hacia nave->x
                {
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

                    // Disparo si condiciones se cumplen
                    if (enemigo->y > 340 && enemigo->Disparo != 0 && al_get_time() - enemigo->Tiempo >= 2 && puedeAtacar(enemigo)) {
                        enemigo->Disparo -= 1;
                        enemigo->Tiempo = al_get_time();
                        crearbala(-3, enemigo->x + 15, enemigo->y + 50, sys); // Dispara bala
                    }

                    // Movimiento vertical si no colisiona
                    float n=0;
                    if(nivel>3)
                    {
                        n=2;
                    }
                    else
                    {
                        n=nivel/2;
                    }
                    if (!colisionaConOtro)
                        enemigo->y += 1+n;

                    // Movimiento horizontal evitando colisión
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

                    // Chequeo final
                    if (enemigo->y >= nave->y)
                        enemigo->estado = 102;

                    break;
                }
                case 102: // modificar por aparición de arriba a origen 
                    if (enemigo->y<720) 
                {
                    // Reaparecer desde un costado
                    if (enemigo->x < 640)
                        enemigo->x = -30;         // Reaparece por la izquierda con algo de variación
                    else
                        enemigo->x = 1310;   // Reaparece por la derecha (pantalla 800px, enemigo ~30px ancho)

                    enemigo->y = 100;                      // Posición vertical fija

                    // Restaurar parámetros del enemigo
                    enemigo->estado = 101;
                    enemigo->Tiempo = al_get_time();
                    enemigo->Disparo = 3;                 // Si querés reiniciar el número de disparos, opcional

                    // No se elimina; se recicla
                }

            }
    }

        enemigo = enemigo->Siguiente;
    }
}



void Game::crearnave()
{
    nave= new personaje();   //se crea puntero a struct
    nave->bitmap=al_load_bitmap("pictures/nave1.png"); //se carga imagen principal
    nave->disparobitmap=al_load_bitmap("pictures/nave2.png"); //se carga imagen de disparo
    nave->alto=al_get_bitmap_height(nave->bitmap);            //se carga su ancho y alto segun imagen
    nave->ancho=al_get_bitmap_width(nave->bitmap);  
    nave->x=800/2;                                            //se toma el tamaño de la pantalla y se divide en 2
    nave->y= height - 80;                                          //se carga posición en y
    nave->tiempo=0;                                           // se establece tiempo de refresco
    nave->vida=3;
}
void Game::dibujarnave() const
{
     al_draw_scaled_bitmap(nave->bitmap, 0, 0, nave->ancho, nave->alto,nave->x, nave->y,30, 30,0);//se dibuja reescalada la nave
}
void Game::actualizarNave(SystemResources& sys)
 {

    if (nave->x + 30 > 800)//se mueve si no ha llegado al limite x
		derecha = false;
	if (nave->x < 0)
		izquierda = false;
	if (derecha)//A la derecha 
		nave->x += 4;
	if (izquierda) // a la izquiersa
		nave->x -= 4;
}

void Game::crearbala(int dy, int x, int y, SystemResources& sys) {
    ptr_bala nueva = new Bala();
    nueva->x = x;
    nueva->y = y;
    nueva->velocidad = dy;
    nueva->siguiente = nullptr;
    agregarBala(Balas, nueva);

    if (sys.shootSound) {
        al_play_sample(sys.shootSound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);
    }
}

void Game::dibujarbala() const
{
    ptr_bala aux = Balas;
    while (aux != nullptr) {
        // se rescala y se suma 10 y 4 a x para que aparezca encima de la nave
        al_draw_filled_rectangle(aux->x, aux->y, aux->x + 4, aux->y + 10, al_map_rgb(255, 255, 0));
        aux = aux->siguiente;
    }
}

void Game::actualizarbala(SystemResources& sys)
{
    ptr_bala aux = Balas;
    ptr_bala anterior = nullptr;

    while (aux != nullptr) {
        // Mover la bala
        aux->y -= aux->velocidad;

        if (aux->y <= 0) {
            ptr_bala eliminar = aux;

            if (anterior == nullptr) 
            {
                //esa la primera entonces paselo a balas
                Balas = aux->siguiente;
                aux = Balas;
            } else
             {
                anterior->siguiente = aux->siguiente;// si no siga agregando
                aux = aux->siguiente;
            }

            delete eliminar;
        } else {
            //pasamos a la siguiente
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

void Game::update(SystemResources& sys) {
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
    if (nave->bitmap == nave->disparobitmap) //si esta en la imagen de disparo
    {
        if (al_get_time() - nave->tiempo > 1.0) // SE cumplió en el tiempo?
        {
            nave->bitmap = al_load_bitmap("pictures/nave1.png");// cambie de imagen
        }
    }
    if (al_get_time() - tiempoUltimoAtaque >= 4) //se cumplió 4s desde el ultimo ataque?
    {
        ptr_est aux = enemigos;

        while (aux != nullptr) {
            if (aux->ataque == 0 && puedeAtacar(aux)) 
            {
                aux->ataque = 1;
                aux->estado = 100; // estado de ataque

                break; // solo un atacante a la vez
            }
            aux = aux->Siguiente;
        }
    
    tiempoUltimoAtaque = al_get_time();
    }
    if(enemigos==nullptr)
    {
        nivel++;
        crearnivel();
        tiempoUltimoAtaque = al_get_time();
    }
    actualizarenemigos(sys);
    colisiones(sys);
    actualizarNave(sys);
    actualizarbala(sys);
}

void Game::draw() const {
    al_clear_to_color(al_map_rgb(5, 2, 10)); // Fondo oscuro del espacio
    
    // Puntaje en la parte superior
    std::string scoreText = "SCORE: " + std::to_string(currentScore);
    std::string highScoreText = "HIGH SCORE: " + std::to_string(highScore);
    
    // Puntaje actual (izquierda, fuente normal, blanco)
    al_draw_text(font, al_map_rgb(255, 255, 255), 20, 10, 
                 ALLEGRO_ALIGN_LEFT, scoreText.c_str());
    
    // High score (derecha, fuente normal, ROJO)
    al_draw_text(font, al_map_rgb(255, 0, 0), width - 20, 10, 
                 ALLEGRO_ALIGN_RIGHT, highScoreText.c_str());
    
    // Vidas como naves 
    int bannerHeight = 40;                    // Altura del banner
    int bannerY = height - bannerHeight;      // Posición Y del banner
    
    // Fondo del banner (negro semi-transparente)
    al_draw_filled_rectangle(0, bannerY, width, height, 
                           al_map_rgba(0, 0, 0, 200)); // Negro con transparencia
    
    
    // Vidas como naves dentro del banner
    int livesX = 30;                          // Margen izquierdo
    int livesY = bannerY + 8;                 // Centrado en el banner
    int spacing = 35;                         // Espacio entre naves
    
    // Dibujar las naves de vida
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
    
    // ===== DIBUJAR ELEMENTOS DEL JUEGO =====
    dibujarenemigos();
    dibujarnave();
    dibujarbala();
    
    // ===== DIBUJAR ESTRELLAS =====
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
            limpiarenemigos();
        } 
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                running = false;
                limpiarenemigos();
            }
        } 
        
        if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) 
        {
            if (!(nave->bitmap == nave->disparobitmap)) 
            {
                nave->bitmap = nave->disparobitmap;
                nave->tiempo = al_get_time(); // guardar tiempo del cambio
                crearbala(3, nave->x+15, nave->y-30, sys);
            }
        }
        if(nave->vida<=0)
        {
            running=false;
            limpiarenemigos();

        }
        else if (event.type == ALLEGRO_EVENT_TIMER) {
            update(sys);
            draw();
        }
        

    }

    return 0;
}

int Game::getCurrentScore() const {
    return currentScore;
}
