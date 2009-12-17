/*
 * game.cpp
 * @description: Implementaci�n de la graficaci�n y l�gica del juego
 */

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "main.h"
#include "imageloader.h"
#include "texture.h"
#include "utils.h"
#include "sfx.h"
#include "motionblur.h"
#include "game_entities.h"
#include "colors.h"

#define ROAD_MIN 0
#define ROAD_MAX 145
#define ROAD_STEPS 35
#define ROAD_WIDTH 5

#define LOOK_AT_X 0
#define LOOK_AT_Y 10
#define LOOK_AT_Z -ROAD_MAX

#define CAR_WIDTH 0.7
#define CAR_HEIGHT 0.7
#define CAR_LENGTH 1.0

#define FOG_FAR -(ROAD_MAX-ROAD_MIN)-10
#define FOG_NEAR -(ROAD_MAX-ROAD_MIN)/10

#define BASE_SPEED 0.01
#define BASE_SPAWN_INTERVAL 450

#define PI 3.14159265358979323846

// BLUR STUFF
GLuint _green_texture; // textura verde
int _tex_0_size = 512; // tama�o de la textura
//float blur_zoom = 0; // factor de "zoom" de la textura

//  GENERAL
float _anglez = 90.0; // angulo x
float _angley = 75.0; // angulo y
float _distance = 30; // distancia al origen

// CARRETERA
float curvatura_h = 100000.0;
float curvatura_v = 100000.0;
int sentido_h = 1;
int sentido_v = 1;
float delta = 0; // usado para "transladar" a  la carretera

float _speed = 1.1;

// CAMARA
int current_cam = 1;
int _last_impact = 0;
struct Punto3D _look_from, _look_from_shake = {0, 0, 0};

// ENTIDADES
struct game_entity entity_header;

// AUTO
struct Punto3D car_pos[3]; // 0 center, 1: bounding box min, 2: bounding box max
float _car_roll; // para cuando gira
int left_key = 0;
int right_key = 0;

// JUGADOR y JUEGO misc
int _base_time = 0;
int _lives = 3;
int _level = 1;
unsigned int _score = 0;

/*
 * Inicializaci�n
 */
void game_init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.9);
    glPointSize(3.0);

    glEnable (GL_DEPTH_TEST); //enable the depth testing
    
    CreateMotionBlurTexture();

    music_stop(0);
	music_play("g-storm.mp3");
	
	entity_header.next = create_entity();
	
	_speed = BASE_SPEED;
	srand(time(NULL)); // seed the random!
	
	_last_impact = -1500; // el ultimo impacto decimos que fue hace tiempo para que no suceda... todav�a
	
    Image * image = loadBMP("img\\verde.bmp");
	_green_texture = texture_load_texture(image);
	
	// posici�n inicial del auto
	car_pos[0].x = 0;
	car_pos[0].y = 0.4;
	car_pos[0].z = -7;
    // bounds del bouding box, minimos
    car_pos[1].x = car_pos[0].x - CAR_WIDTH/2;
    car_pos[1].y = car_pos[0].y - CAR_HEIGHT/2;
    car_pos[1].z = car_pos[0].z - CAR_LENGTH;
    // bounds del bouding box, maximos
    car_pos[2].x = car_pos[0].x + CAR_WIDTH/2;
    car_pos[2].y = car_pos[0].y + CAR_HEIGHT/2;
    car_pos[2].z = car_pos[0].z;
    
    _base_time = glutGet(GLUT_ELAPSED_TIME);
    
}

/**
 * Manejo de teclas convencionales
 */
void game_handle_keypress(unsigned char key, int x, int y) {
    static float xx = 0;
    static float yy = 0;
    switch (key) {
/*        case 87: // W
        case 119: // w
            toggle_polygon_mode();
            break;*/
        case '+':
        	_distance = constraint_f(_distance, -1.5, 5, 200);
        	break;
        case '-':
        	_distance = constraint_f(_distance, 1.5, 5, 200);
			break;
        case 'f':
            _anglez = wrap_f(_anglez, 3.0, 0.0, 360.0);
            break;
        case 'h':
            _anglez = wrap_f(_anglez, -3.0, 0.0, 360.0);
            break;
        case 't':
            _angley = wrap_f(_angley, -3.0, 0.0, 360);
            break;
        case 'g':
            _angley = wrap_f(_angley, 3.0, 0.0, 360);
            break;
 		case 'a':
        case 'A':
            if (xx > -15)
                xx = xx - 0.04;
            break;
		case 'd':
        case 'D':
            if (xx < 15)
                xx = xx + 0.04;
            break;
 		case 'w':
        case 'W':
            if (yy > -15)
                yy = yy + 0.04;
            break;
		case 's':
        case 'S':
            if (yy < 15)
                yy = yy - 0.04;
            break;
        case ',':
            _speed -= 1.1*BASE_SPEED;
            break;
        case '.':
            _speed += 1.1*BASE_SPEED;
            break;
        case 'i':
            _last_impact = glutGet(GLUT_ELAPSED_TIME);
    }
    
    sentido_h = (xx > 0 ? 1 : -1);
    curvatura_h = (xx == 0 ? 100 : 1 * sqrt(1/xx*1/xx)+5);
    sentido_v = (yy > 0 ? 1 : -1);
    curvatura_v = (yy == 0 ? 100 : 1 * sqrt(1/yy*1/yy)+5);
}

/*
 * direction -1 izq
 *           +1 der
 */
void car_move(float how_much) {
/*    if (direction == -1) {
        if (car_pos[0].x - CAR_WIDTH > -ROAD_WIDTH)
            car_pos[0].x -= 0.1;
    } else {
        if (car_pos[0].x + CAR_WIDTH < ROAD_WIDTH)
            car_pos[0].x += 0.1;
    }*/
    car_pos[0].x += how_much;
    
    if (car_pos[0].x < -ROAD_WIDTH + CAR_WIDTH/2)
        car_pos[0].x = -ROAD_WIDTH + CAR_WIDTH/2;
    else if (car_pos[0].x > ROAD_WIDTH - CAR_WIDTH/2)
        car_pos[0].x = ROAD_WIDTH - CAR_WIDTH/2;


    car_pos[1].x = car_pos[0].x - CAR_WIDTH/2;
/*    car_pos[1].y = car_pos[0].y - CAR_HEIGHT/2;
    car_pos[1].z = car_pos[0].z - CAR_LENGTH/2;*/

    car_pos[2].x = car_pos[0].x + CAR_WIDTH/2;
/*    car_pos[2].y = car_pos[0].y + CAR_HEIGHT/2;
    car_pos[2].z = car_pos[0].z + CAR_LENGTH/2;*/
}
    

/**
 * Manejo de teclas especiales (flechas, funci�n, etc)
 */
void game_handle_keypress_special(int key, int x, int y, int state) {
    switch (key) {
        case GLUT_KEY_LEFT:
            left_key = state;
  
            break;    
        case GLUT_KEY_RIGHT:
            right_key = state;
            break;        
        case GLUT_KEY_F2:
            if (state)
                current_cam = !current_cam;
            break;
    }
    sprintf(_debug_string, "KEYS: left=%d right=%d roll=%f", left_key, right_key, _car_roll);
    
}

void game_handle_keypress_special_up(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            left_key = 0;
            break;    
        case GLUT_KEY_RIGHT:
            right_key = 0;
            break;        
        }
}

float calcular_alpha(float z) {
    float x;
    if (z >= FOG_NEAR) {
        return 1;
    } else if (z > FOG_FAR) {
        x = 1 - ((-z+FOG_NEAR) / (+FOG_NEAR-(float)FOG_FAR));
        return x;
    } else {
        return 0;
    }
}

/*
** f(x)  = x^2
** f'(x) = 2x
*/
static void dibujar_carretera()  {
    float x = 0, y = 0, z = 0;
    float a, b;
    float i;
    int j = 0, k;
    float aux_x;
    static struct Punto3D vrts[ROAD_STEPS][2];
    float alpha;
    
    i = 0 - delta;
    for (j = 0; j < ROAD_STEPS; j++ ) {
        a = i / curvatura_h; // auxiliar para calcular 'x' y 'z' (es el parametro de la funci�n f(a) = a^2, se divide por curvatura para obtener una porci�n mayor o menor de la formula
        aux_x = (a*a)*sentido_h; // coordenada de x en el centro de la carretera
        
        b = i / curvatura_v; // auxiliar para calcular 'y' (es el parametro de la funci�n f(b) = b^2, se divide por curvatura para obtener una porci�n mayor o menor de la formula
        y = (b*b)*sentido_v/3; // coordenada y de la carretera

        // ahora unas cosas que usan trigonometr�a
        // hallar el incremento de x con respecto al centro de la carretera (esto es para que la curvatura se vea real)
        if (a == 0)
            x = ROAD_WIDTH;
        else
            x = sin(atan(-1/(2*(a))*curvatura_h*sentido_h)) * ROAD_WIDTH * sentido_h;
        // hallar el incremento de z (esto es para que la curvatura se vea real)
        if (a == 0)
            z = 0;
        else
            z = cos(atan(-1/(2*(a))*curvatura_h*sentido_h)) * ROAD_WIDTH * sentido_h;

        // almacenar variables
        vrts[j][0].x = aux_x;
        if (aux_x + x > aux_x - x) {
            vrts[j][1].x = -x;
        } else {
            vrts[j][1].x = x;
        }
        vrts[j][0].y = y;
        vrts[j][1].y = y;
        vrts[j][0].z = -i;
        vrts[j][1].z = z;
        
        // incrementar i, nuestro parametro global (y tambi�n z)
        i += (ROAD_MAX-ROAD_MIN)/(float)ROAD_STEPS;
    }
    
    // el suelo es transparente, habilitar transparencia
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_TEXTURE_2D);
    
    // dibujar la carretera    
    for (j = ROAD_STEPS-1; j >= 0; j--) {
        alpha = calcular_alpha(vrts[j][0].z);

        // el suelo es transparente, dibujar un quad por cada par de puntos3d (menos para el �ltimo, ya que no existe vrts[0-1]
        if (j > 0) {
            glBegin(GL_QUADS);
                i = (j%2 ? 0.10 : 0.05);
                glColor4f(i, i, i, 0.75* alpha); 
                glVertex3f(vrts[j][0].x+vrts[j][1].x,vrts[j][0].y-0.07,vrts[j][0].z-vrts[j][1].z);
                glVertex3f(vrts[j][0].x-vrts[j][1].x,vrts[j][0].y-0.07,vrts[j][0].z+vrts[j][1].z);
                glVertex3f(vrts[j-1][0].x-vrts[j-1][1].x,vrts[j-1][0].y-0.07,vrts[j-1][0].z+vrts[j-1][1].z);
                glVertex3f(vrts[j-1][0].x+vrts[j-1][1].x,vrts[j-1][0].y-0.07,vrts[j-1][0].z-vrts[j-1][1].z);
            glEnd();
        }

        //alpha = 1;
        // dibujar las lineas
        glBegin(GL_LINES);
        glColor4f(1.0, 1.0, 1.0, alpha);
            // sobre el suelo
            glColor4f(0.5,0.5,0.5, alpha);
            glVertex3f(vrts[j][0].x+vrts[j][1].x,vrts[j][0].y,vrts[j][0].z-vrts[j][1].z);
            glVertex3f(vrts[j][0].x-vrts[j][1].x,vrts[j][1].y,vrts[j][0].z+vrts[j][1].z);
            // paredes
            glColor4f(1,1,1, alpha);
            glVertex3f(vrts[j][0].x+vrts[j][1].x,vrts[j][0].y,vrts[j][0].z-vrts[j][1].z);
            glVertex3f(vrts[j][0].x+vrts[j][1].x,vrts[j][0].y+0.5,vrts[j][0].z-vrts[j][1].z);
            glVertex3f(vrts[j][0].x-vrts[j][1].x,vrts[j][1].y,vrts[j][0].z+vrts[j][1].z);
            glVertex3f(vrts[j][0].x-vrts[j][1].x,vrts[j][1].y+0.5,vrts[j][0].z+vrts[j][1].z);

            // laterales
            glColor4f(1,1,1,alpha);
            glVertex3f(vrts[j][0].x+vrts[j][1].x,vrts[j][0].y+0.5,vrts[j][0].z-vrts[j][1].z);
            glColor4f(1,1,1,0.0);
            glVertex3f(vrts[j][0].x+vrts[j][1].x*12,vrts[j][0].y+0.5,vrts[j][0].z-vrts[j][1].z*12);
            
            glColor4f(1,1,1,alpha);
            glVertex3f(vrts[j][0].x-vrts[j][1].x,vrts[j][1].y+0.5,vrts[j][0].z+vrts[j][1].z);
            glColor4f(1,1,1,0.0);
            glVertex3f(vrts[j][0].x-vrts[j][1].x*12,vrts[j][1].y+0.5,vrts[j][0].z+vrts[j][1].z*12);
        glEnd();
        
        // otras l�neas m�s
        if (j%2) {
            glColor4f(1.0, 1.0, 1.0, 0.7*alpha);
            glLineWidth(3);
            glBegin(GL_LINES);
                    glVertex3f(vrts[j][0].x+vrts[j][1].x/3,vrts[j][0].y-0.05,vrts[j][0].z-vrts[j][1].z/3);
                    glVertex3f(vrts[j-1][0].x+vrts[j-1][1].x/3,vrts[j-1][0].y-0.05,vrts[j-1][0].z-vrts[j-1][1].z/3);
                    glVertex3f(vrts[j][0].x-vrts[j][1].x/3,vrts[j][0].y-0.05,vrts[j][0].z+vrts[j][1].z/3);
                    glVertex3f(vrts[j-1][0].x-vrts[j-1][1].x/3,vrts[j-1][0].y-0.05,vrts[j-1][0].z+vrts[j-1][1].z/3);
    
            glEnd();
            glLineWidth(1);
        }
    }

    glDisable(GL_BLEND);
}

void calcular_rotacion(struct Punto3D entrada, float * rot_x, float * rot_y) {
    // curva para el plano xz
    float aux = entrada.z / curvatura_h; // auxiliar para calcular 'x' y 'z' (es el parametro de la funci�n f(a) = a^2, se divide por curvatura para obtener una porci�n mayor o menor de la formula
    float aux_x = (aux*aux)*sentido_h; // coordenada de x en el centro de la carretera
    
    // curva para y
    float aux2 = entrada.z / curvatura_v; // auxiliar para calcular 'y' (es el parametro de la funci�n f(b) = b^2, se divide por curvatura para obtener una porci�n mayor o menor de la formula
    float aux_y = (aux2*aux2)*sentido_v/3.0; // coordenada y de la carretera
    
    float test;
    *rot_x = (-atan(-1/(2*(aux2))*curvatura_v) / PI * 180 + 90) * sentido_v;
    *rot_y = (atan(-1/(2*(aux))*curvatura_h) / PI * 180 - 90) * sentido_h;
}

void calcular_coordenadas(struct Punto3D entrada, struct Punto3D * salida) {
    // curva para el plano xz
    float aux = entrada.z / curvatura_h; // auxiliar para calcular 'x' y 'z' (es el parametro de la funci�n f(a) = a^2, se divide por curvatura para obtener una porci�n mayor o menor de la formula
    float aux_x = (aux*aux)*sentido_h; // coordenada de x en el centro de la carretera
    
    // curva para y
    float aux2 = entrada.z / curvatura_v; // auxiliar para calcular 'y' (es el parametro de la funci�n f(b) = b^2, se divide por curvatura para obtener una porci�n mayor o menor de la formula
    float aux_y = (aux2*aux2)*sentido_v/3.0; // coordenada y de la carretera
    
    // ahora unas cosas que usan trigonometr�a
    // hallar el incrementO de x con respecto al centro de la carretera (esto es para que la curvatura se vea real)
    if (aux == 0)
        salida->x = entrada.x;
    else
        salida->x = aux_x + sin(atan(-1/(2*(aux))*curvatura_h * sentido_h)) * entrada.x * sentido_h;

    // hallar el incremento de z (esto es para que la curvatura se vea real)
    if (aux == 0)
        salida->z = entrada.z;
    else
        salida->z = entrada.z + cos(atan(-1/(2*(aux))*curvatura_h*sentido_h)) * entrada.x * sentido_h;
        
    salida->y = aux_y + entrada.y;
}

/**
 * Dibujar el "bouding box
 */
void dibujar_bounding_box(Punto3D min, Punto3D max) {
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
        glPointSize(4);
        glBegin(GL_LINE_STRIP);
            // lejos
            glColor3f(0, 1, 0);
            glVertex3f(min.x, min.y, min.z);
            glColor3f(0.3, 0.6, 0);
            glVertex3f(min.x, max.y, min.z);
            glColor3f(0.6, 0.3, 0);
            glVertex3f(max.x, max.y, min.z);
            glColor3f(0.3, 0.6, 0);
            glVertex3f(max.x, min.y, min.z);
            glColor3f(0, 1, 0);
            glVertex3f(min.x, min.y, min.z);
        glEnd();
        glBegin(GL_LINE_STRIP);
            // cerca
            glColor3f(0.3, 0.6, 0);
            glVertex3f(min.x, min.y, max.z);
            glColor3f(0.6, 0.3, 0);
            glVertex3f(min.x, max.y, max.z);
            glColor3f(1, 0, 0);
            glVertex3f(max.x, max.y, max.z);
            glColor3f(0.6, 0.3, 0);
            glVertex3f(max.x, min.y, max.z);
            glColor3f(0.3, 0.6, 0);
            glVertex3f(min.x, min.y, max.z);
        glEnd();
        glBegin(GL_LINE_STRIP);
            // abajo
            glColor3f(0, 1, 0);
            glVertex3f(min.x, min.y, min.z);
            glColor3f(0.3, 0.6, 0);
            glVertex3f(min.x, min.y, max.z);
            glColor3f(0.6, 0.3, 0);
            glVertex3f(max.x, min.y, max.z);
            glColor3f(0.3, 0.6, 0);
            glVertex3f(max.x, min.y, min.z);
            glColor3f(0, 1, 0);
            glVertex3f(min.x, min.y, min.z);
        glEnd();
        glBegin(GL_LINE_STRIP);
            // arriba
            glColor3f(0.3, 0.6, 0);
            glVertex3f(min.x, max.y, min.z);
            glColor3f(0.3, 0.6, 0);
            glVertex3f(min.x, max.y, max.z);
            glColor3f(1, 0, 0);
            glVertex3f(max.x, max.y, max.z);
            glColor3f(0.6, 0.3, 0);
            glVertex3f(max.x, max.y, min.z);
            glColor3f(0.3, 0.6, 0);
            glVertex3f(min.x, max.y, min.z);
        glEnd();
    
        glColor3f(0,1,0);
        glBegin(GL_POINTS);glVertex3f(min.x,min.y,min.z);glEnd();
        glColor3f(1,0,0);
        glBegin(GL_POINTS);glVertex3f(max.x,max.y,max.z);glEnd();

    glPopMatrix();
    glPointSize(1);

}

/**
 * Dibujar el auto
 */
void dibujar_auto() {
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
        glColor3f(0.2, 0.3, 0.7);
        glTranslatef(car_pos[0].x,car_pos[0].y,car_pos[0].z);
        glRotatef(180, 0, 1, 0);
        glRotatef(_car_roll, 0, 0, 1);
        glutWireCone(CAR_WIDTH/2, 1.0, 8, 1);
        glTranslatef(-0.4,0,0.0);
        glutWireCone(CAR_WIDTH/3, 0.5, 6, 1);        
        glTranslatef(0.8,0,0.0);        
        glutWireCone(CAR_WIDTH/3, 0.5, 6, 1);
        
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glLineWidth(5);
        glBegin(GL_LINES);
            glColor3f(0.2, 0.3, 0.7);
            glVertex3f(0,0,0);
            glColor4f(0.2, 0.3, 0.7, 0.2);
            glVertex3f(0,0,-2);
        glEnd();
        glTranslatef(-0.8,0,0.0);        
        glBegin(GL_LINES);
            glColor3f(0.2, 0.3, 0.7);
            glVertex3f(0,0,0);
            glColor4f(0.2, 0.3, 0.7, 0.2);
            glVertex3f(0,0,-2);
        glEnd();
        glLineWidth(1);
        
        glDisable(GL_BLEND);
    glPopMatrix();
    dibujar_bounding_box(car_pos[1], car_pos[2]);
}

/**
 * Dibujar una mira del final de la carretera
 * La camara mira a una fracci�n de esta direcci�n
 */
void dibujar_mira() {
   struct Punto3D a;
   
   a.x = LOOK_AT_X;
   a.y = LOOK_AT_Y;
   a.z = LOOK_AT_Z;
   
   calcular_coordenadas(a, &a);
   
    glPushMatrix();
    glDisable(GL_DEPTH_TEST);
    glTranslatef(a.x, a.y, a.z);
    
    glColor3f(1.0, 0.8, 0.0);
    glBegin(GL_LINES);
        glVertex3f(a.x, a.y+1, a.z);
        glVertex3f(a.x, a.y+3, a.z);

        glVertex3f(a.x, a.y-1, a.z);
        glVertex3f(a.x, a.y-3, a.z);
  
        glVertex3f(a.x+1, a.y, a.z);
        glVertex3f(a.x+3, a.y, a.z);
  
        glVertex3f(a.x-1, a.y, a.z);
        glVertex3f(a.x-3, a.y, a.z);
    glEnd();
    glEnable(GL_DEPTH_TEST);    
    glPopMatrix();
}

void shake() {
    float elapsed;
    if (glutGet(GLUT_ELAPSED_TIME) - _last_impact < 0500)  {
        elapsed = 0500 - (glutGet(GLUT_ELAPSED_TIME) - _last_impact);
        _look_from_shake.x = ((rand()/(float)RAND_MAX)*6 - 3) * (elapsed/0500);
        _look_from_shake.y = ((rand()/(float)RAND_MAX)*6 - 3) * (elapsed/0500);
        _look_from_shake.z = ((rand()/(float)RAND_MAX)*6 - 3) * (elapsed/0500);
    } else {
        _look_from_shake.x = 0;
        _look_from_shake.y = 0;
        _look_from_shake.z = 0;
    }
    
    if (glutGet(GLUT_ELAPSED_TIME) - _last_impact < 50)  {
        ortho_mode(0,0,1,1);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glColor4f(1,1,1, 0.5);
        glBegin(GL_QUADS);
            glVertex3f(0,0,-1);
            glVertex3f(1,0,-1);
            glVertex3f(1,1,-1);
            glVertex3f(0,1,-1);
        glEnd();
        perspective_mode();
    }
}

/**
 * Dibujar el horizonte
 */
void dibujar_horizonte() {
    int i;
    static int start = 0; // offset del tiempo para poder dibujar bien el fondo
    static int elapsed_time = 0;
    static struct hsl color_hsl[3] = {{172, 1, 0.5},{182, 1, 0.2},{192, 1, 0.1}}; // paleta de colores en HSL
    struct rgb color_rgb[3]; // variables auxiliares donde almacenaremos los HSL convertidos
    static float orientation = 0;

    glPushMatrix();
    glDisable(GL_TEXTURE_2D);

    // guardamos el momento de inicio    
    if (start == 0) {
        start = glutGet(GLUT_ELAPSED_TIME);
    }

    // con el paso del tiempo, damos vuelta por la paleta de colores
    // HSL nos permite hacer este ciclo sin mucho problema, ya que solo se tiene que cambiar el HUE
    for (i = 0; i < 3; i++) {
        color_hsl[i].hue = ((glutGet(GLUT_ELAPSED_TIME) - start)/500 + 180 + i*50)%360;
        hsl2rgb(color_hsl[i], &color_rgb[i]);
    }

    orientation += 0.1/curvatura_h * sentido_h * (glutGet(GLUT_ELAPSED_TIME) - elapsed_time);
    glRotatef(orientation, 0.0, 1.0, 0.0);

    // dibujamos el horizonte
    glBegin(GL_QUADS);
        for (i = 1; i >= -1; i-=2) {
            // center
            glColor4f(color_rgb[0].red, color_rgb[0].green, color_rgb[0].blue, 1);
            glVertex3f(0*i, 0, -ROAD_MAX);
            glColor4f(color_rgb[1].red, color_rgb[1].green, color_rgb[1].blue, 1);
            glVertex3f(100*i, 0, -ROAD_MAX+10);
            glColor4f(color_rgb[2].red, color_rgb[2].green, color_rgb[2].blue, 1);
            glVertex3f(100*i, 100, -ROAD_MAX+10);
            glVertex3f(0*i, 100, -ROAD_MAX);

            // center top
            glColor4f(color_rgb[2].red, color_rgb[2].green, color_rgb[2].blue, 1);
            glVertex3f(0*i, 100, -ROAD_MAX);
            glVertex3f(100*i, 100, -ROAD_MAX+10);
            glColor4f(0, 0, 0, 1);
            glVertex3f(100*i, 150, -ROAD_MAX+50);
            glVertex3f(0*i, 170, -ROAD_MAX+100);
  
            // near side
            glColor4f(color_rgb[1].red, color_rgb[1].green, color_rgb[1].blue, 1);
            glVertex3f(100*i, 0, -ROAD_MAX+10);
            glColor4f(color_rgb[2].red, color_rgb[2].green, color_rgb[2].blue, 1);
            glVertex3f(300*i, 0, -ROAD_MAX+30);
            glColor4f(0, 0, 0, 1);
            glVertex3f(300*i, 100, -ROAD_MAX+30);
            glColor4f(color_rgb[2].red, color_rgb[2].green, color_rgb[2].blue, 1);
            glVertex3f(100*i, 100, -ROAD_MAX+10);

            // near side top
            glColor4f(color_rgb[2].red, color_rgb[2].green, color_rgb[2].blue, 1);
            glVertex3f(100*i, 100, -ROAD_MAX+10);
            glColor4f(0, 0, 0, 1);
            glVertex3f(300*i, 100, -ROAD_MAX+30);
            glVertex3f(300*i, 190, -ROAD_MAX+45);
            glVertex3f(100*i, 150, -ROAD_MAX+50);

            // far side
            glColor4f(color_rgb[2].red, color_rgb[2].green, color_rgb[2].blue, 1);
            glVertex3f(300*i, 0, -ROAD_MAX+30);
            glColor4f(0, 0, 0, 1);
            glVertex3f(800*i, 0, -ROAD_MAX+50);
            glVertex3f(800*i, 100, -ROAD_MAX+50);
            glVertex3f(300*i, 100, -ROAD_MAX+30);

            // bottom center
            glColor4f(color_rgb[0].red, color_rgb[0].green, color_rgb[0].blue, 1);
            glVertex3f(0*i, 0, -ROAD_MAX);
            glColor4f(color_rgb[1].red, color_rgb[1].green, color_rgb[1].blue, 1);
            glVertex3f(100*i, 0, -ROAD_MAX+10);
            glColor4f(0.0, 0.0, 0.0, 1);
            glVertex3f(100*i, -40, -ROAD_MAX+10);
            glVertex3f(0*i, -100, -ROAD_MAX);

            // bottom near side
            glColor4f(color_rgb[1].red, color_rgb[1].green, color_rgb[1].blue, 1);
            glVertex3f(100*i, 0, -ROAD_MAX+10);
            glColor4f(color_rgb[2].red, color_rgb[2].green, color_rgb[2].blue, 1);
            glVertex3f(300*i, 0, -ROAD_MAX+30);
            glColor4f(0.0, 0.0, 0.0, 1);
            glVertex3f(300*i, -40, -ROAD_MAX+30);
            glVertex3f(100*i, -40, -ROAD_MAX+10);

            // bottom far side
            glColor4f(color_rgb[2].red, color_rgb[2].green, color_rgb[2].blue, 1);
            glVertex3f(300*i, 0, -ROAD_MAX+30);
            glColor4f(0, 0, 0, 1);
            glVertex3f(800*i, 0, -ROAD_MAX+50);
            glVertex3f(800*i, -10, -ROAD_MAX+50);
            glVertex3f(300*i, -40, -ROAD_MAX+30);

        }
    glEnd();

    // dibujamos una linea fina y brillante en el centro del horizonte
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);
    glBegin(GL_LINES);
        glColor4f(color_rgb[0].red, color_rgb[0].green, color_rgb[0].blue, 1);
        glVertex3f(0, 0, -ROAD_MAX);
        glColor4f(0.13, 0.33, 0.64, 0.0);
        glColor4f(color_rgb[2].red, color_rgb[2].green, color_rgb[2].blue, 1);
        glVertex3f(300, 0, -ROAD_MAX+30);
        glColor4f(color_rgb[0].red, color_rgb[0].green, color_rgb[0].blue, 1);
        glVertex3f(0, 0, -ROAD_MAX);
        glColor4f(color_rgb[2].red, color_rgb[2].green, color_rgb[2].blue, 1);
        glVertex3f(-300, 0, -ROAD_MAX+30);
    glEnd();
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);
    
    glPopMatrix();
    
    elapsed_time = glutGet(GLUT_ELAPSED_TIME);
}

/**
 * Que hacer cuando chocamos
 */
void on_collision() {
    _last_impact = glutGet(GLUT_ELAPSED_TIME); // SHAKE THAT SCREEN!
    _lives--;
}

/**
 * Dibujar el HUD
 */
void draw_hud() {
    char buffer[100];
    int offset;
    
    ortho_mode(0, _height, _width, 0);
	glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // fondo
    glColor4f(0, 0, 0, 0.65);
    glBegin(GL_QUADS);
        glVertex3f(0, 50, 0); // abajo izquierda
        glVertex3f(_width, 50, 0); // abajo derecha
        glVertex3f(_width, 0, 0); // arriba derecha
        glVertex3f(0, 0, 0); // arriba izquierda
    glEnd();
    
    // vidas
    glColor3f(0.3f, 0.6f, 1.0f);
    glRasterPos2i(10, 30);
    sprintf(buffer, "Shields %d", _lives);
    glutBitmapString(GLUT_BITMAP_9_BY_15, (unsigned char *) buffer);

    // velocidad
    glColor3f(0.3f, 1.0f, 0.6f);
    glRasterPos2i(10, 10);
    sprintf(buffer, "Speed ");
    glutBitmapString(GLUT_BITMAP_9_BY_15, (unsigned char *) buffer);
    sprintf(buffer, "%4.2f", _speed*1000 );
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *) buffer);
    sprintf(buffer, "km/h");
    glutBitmapString(GLUT_BITMAP_9_BY_15, (unsigned char *) buffer);
    
    // elapsed time
    glColor3f(1.0f, 0.6f, 0.3f);
    offset = (_width - glutBitmapLength(GLUT_BITMAP_8_BY_13, (unsigned char *) "Elapsed time"))/2;
    glRasterPos2i(offset, 30);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char *) "Elapsed time");
    sprintf(buffer, "%3.2f ", (glutGet(GLUT_ELAPSED_TIME) - _base_time)/1000.0 );
    offset = (_width - glutBitmapLength(GLUT_BITMAP_8_BY_13, (unsigned char *) buffer))/2;
    glRasterPos2i(offset, 10);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *) buffer);
    
    // level
    glColor3f(0.3f, 1.0f, 0.6f);
    glRasterPos2i(_width - 200, 30);
    sprintf(buffer, "Level ");
    glutBitmapString(GLUT_BITMAP_9_BY_15, (unsigned char *) buffer);
    sprintf(buffer, "%d", _level );
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *) buffer);

    // score
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(_width - 200, 10);
    sprintf(buffer, "Score %012u", _score);
    glutBitmapString(GLUT_BITMAP_9_BY_15, (unsigned char *) buffer);


/*    sprintf(_debug_string, "sine: %f", sine_value);

    glColor4f(1, 1, 1, sine_value);
    glRasterPos2i(offset, 300 + i*30);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *) menu_option_strings[i]);*/


    
    glDisable(GL_BLEND);
    perspective_mode();
}

int do_draw() {
   struct Punto3D a;
    
   a.x = LOOK_AT_X;
   a.y = LOOK_AT_Y;
   a.z = LOOK_AT_Z;
   
   calcular_coordenadas(a, &a);
   shake();

    if (current_cam == 0) {
        gluLookAt(sin(_angley*3.14/180) * cos(_anglez*3.14/180) * _distance, cos(_angley*3.14/180) * _distance, sin(_angley*3.14/180) * sin(_anglez*3.14/180) * _distance,  // donde estoy
            0.0, 0.0, 0.0, // a donde miro
            0.0, (_angley >= 180 ? -1.0 : 1.0), 0.0 // mi arriba
        );
    } else {
        gluLookAt(
            -a.x/50 +_look_from_shake.x, 2-a.y/50  +_look_from_shake.y,22  +_look_from_shake.z,
            a.x/2,a.y/2,a.z,
            0, 1, 0
        );
    }
    
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);

//    glPolygonMode(GL_FRONT_AND_BACK, _polygon_mode);

    // esto se pinta sin luz ---
    glDisable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
        dibujar_horizonte();
        glTranslatef(0.0, 0.0, 20.0);

        recorrer_lista(&entity_header, car_pos);
        glEnable(GL_COLOR_MATERIAL);
        dibujar_mira();
        dibujar_carretera();
        dibujar_auto();
        //draw_text();
//        check_collisions();
    glPopMatrix();
//    sprintf(_debug_string, "ENABLED: %d", glIsEnabled(GL_TEXTURE_2D));
}

/**
 * Dibujar la escena
 */
void game_draw_scene() {
    struct Punto3D a;

	RenderToMotionBlurTexture(1, do_draw);
	ShowMotionBlurTexture();
	draw_hud();
}

/**
 * Procesando entre frames
 */
void game_handle_idle() {
    static int elapsed_time = 0;
    static int last_add = 0;
    static int last_add_score = 0;
    int time;
    // tiempo transcurrido
	time = glutGet(GLUT_ELAPSED_TIME);
    delta = wrap_f(delta, (time - elapsed_time) * _speed, ROAD_MIN, (ROAD_MAX - ROAD_MIN)*2/ROAD_STEPS); /* este ultimo es (max - min) / steps */

    if (left_key || right_key)
        _car_roll = (_car_roll + (time - elapsed_time) * _speed*100 * (-left_key + right_key))/2;
    else
        _car_roll = 0 /*_car_roll / ((time - elapsed_time))*/;
    car_move((time - elapsed_time) * _speed/4 * (-left_key + right_key));
    
    // almacenar el nuevo tiempo
    elapsed_time = time;

    // calcular el level
    if (_level < (time - _base_time)/10000) {
        _level = (time - _base_time)/10000;
        _speed = _speed * 1.1;
    }
    
    // agregar score al score :P
    if (time - last_add_score > 1000) {
        last_add_score = time;
        _score += 10;
    }

    // agregar     
    if (time/BASE_SPAWN_INTERVAL > last_add) {
        last_add = time/BASE_SPAWN_INTERVAL;
        agregar_a_lista(&entity_header, create_entity());
    }
}
