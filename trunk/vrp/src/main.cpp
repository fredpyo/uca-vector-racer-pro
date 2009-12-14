/**
 * Info 3 - OpenGl
 * Trabajo Práctico Final : Vector Racer Pro
 * @author: Federico Cáceres <fede.caceres@gmail.com> 051461
 * @author: Sergio Stanichevsky <sergio.sta@gmail.com> 051461
 * @description: Logica general para la aplicación
 * Este módulo maneja la lógica general de la aplicación, el resto es delegado
 * a las funciones propias de cada módulo (intro, title, game). Esto se realiza
 * mediante la mágia de los punteros a funciones.
 */

#include <stdio.h>
#include <stdlib.h> // exit
#include <math.h>

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "scene_switcher.h"
#include "intro.h"
#include "title.h"
#include "ranking.h"
#include "game.h"
#include "utils.h"
#include "sfx.h"

#define INITIAL_SCENE SCENE_INTRO

float fps = 0; // almacena el fps
char _debug_string[512] = {0}; // buffer para mensajes de debug
char _message_string[512] = {0}; // buffer para mensajes
int _show_debug = 1; // mostar mensajes de fps, debug etc

using namespace std;

int _height; // altura de la ventana
int _width; // ancho de la ventana

float _angle = 30.0; // para camara en modo perspectiva

// generic pointers
void (*generic_handle_keypress)(unsigned char key, int x, int y) = NULL;
void (*generic_handle_keypress_special)(int key, int x, int y) = NULL;
void (*generic_handle_mouse_button)(int button, int state, int x, int y) = NULL;
void (*generic_handle_mouse_motion)(int x, int y) = NULL;
void (*generic_handle_mouse_passive_motion)(int x, int y) = NULL;
void (*generic_handle_draw_scene)(void) = NULL;
void (*generic_handle_idle)(void) = NULL;


// Funciones genéricas, usando punteros a funciones
void handle_keypress(unsigned char key, int x, int y) {
    switch (key) {
        case 27: //Esq
            exit(0);
        default:
            // genericness galore!
            generic_handle_keypress(key, x, y);
            break;
    }    
}

void handle_keypress_special(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_F1:
            _show_debug = !_show_debug;
            break;
        default:
            generic_handle_keypress_special(key, x, y);
            break;
    }
}

void handle_mouse_button (int button, int state, int x, int y) {

}

void handle_mouse_motion (int x, int y) {
    
}

void handle_mouse_passive_motion (int x, int y) {

}

/**
 * En caso de que la ventana cambie de tamaño, ajustar el viewport y la perspectiva
 */
void handle_resize(int w, int h) {
    glViewport(0, 0, w, h);
    _height = h;
    _width = w;
    
    glMatrixMode(GL_PROJECTION); // swtich to camera perspective
    glLoadIdentity(); // reset camera
    gluPerspective(45.0, // cam angle
                    (double)w / (double)h, // w/h ratio
                    0.1, // near clipping
                    400.0 // far clipping
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void ortho_mode(int left, int top, int right, int bottom)
{
	glMatrixMode(GL_PROJECTION); // Switch to our projection matrix so that we can change it to ortho mode, not perspective.
	glPushMatrix(); // Push on a new matrix so that we can just pop it off to go back to perspective mode
	glLoadIdentity(); // Reset the current matrix to our identify matrix
	glOrtho( left, right, bottom, top, 0, 200 ); // Pass in our 2D ortho screen coordinates like so (left, right, bottom, top).  The last 2 parameters are the near and far planes.
	glMatrixMode(GL_MODELVIEW); // Switch to model view so that we can render the scope image
	glLoadIdentity(); // Initialize the current model view matrix with the identity matrix
}

void perspective_mode()										// Set Up A Perspective View
{
	glMatrixMode( GL_PROJECTION ); // Enter into our projection matrix mode
	glPopMatrix(); // Pop off the last matrix pushed on when in projection mode (Get rid of ortho mode)
	glMatrixMode( GL_MODELVIEW ); // Go back to our model view matrix like normal
	// We should be in the normal 3D perspective mode now
}

/**
 * Dibujar textos de debug
 */
void draw_text() {
    char buffer[1000];

    if (_show_debug) {
	ortho_mode( 0, 0, _width, _height );
	glDisable(GL_DEPTH_TEST);
		
        // TIMING
        glColor3f(1.0f, 1.0f, 0.0f);
        glRasterPos2i(3, 14);
        sprintf(buffer, "[TIMING] FPS: %f Transcurrido: %dms", fps, glutGet(GLUT_ELAPSED_TIME));
        glutBitmapString(GLUT_BITMAP_9_BY_15, (unsigned char *) buffer);
        // MESSAGES
        glColor3f(0.0f, 0.3f, 1.0f);
        glRasterPos2i(3, _height-20);
        sprintf(buffer, "[MSG] %s", _message_string);
        glutBitmapString(GLUT_BITMAP_9_BY_15, (unsigned char *) buffer);
        glRasterPos2i(0, 0);
        // DEBUG
        glColor3f(0.0f, 1.0f, 0.0f);
        glRasterPos2i(3, _height-4);
        sprintf(buffer, "[DEBUG] %s", _debug_string);
        glutBitmapString(GLUT_BITMAP_9_BY_15, (unsigned char *) buffer);
        glRasterPos2i(0, 0);

    glEnable(GL_DEPTH_TEST);
    perspective_mode();
    }
}

/**
 * Maneja los eventos a ejecutar cuando se haya terminado de renderizar un frame
 * Calcula el FPS
 * Llama a otros procedimientos debidos
 * Solicita redibujar
 */
void handle_idle(void)
{
    static int timebase = 0;
    static int elapsed_time = 0;
    static int frame;
    int time;
    
    // calcular fps
	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		fps = frame*1000.0/(time-timebase);
	 	timebase = time;		
		frame = 0;
	}

    // procedimientos adicionales    
    if (generic_handle_idle)
        generic_handle_idle();
    
    elapsed_time = time;
    // redibujar
    glutPostRedisplay();
}

void draw_scene() {
    if (generic_handle_draw_scene)
        // si hay una función de dibujado seteado, dibujar
        generic_handle_draw_scene();
    else {
        // sino, no dibujar nada
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear all
    }
    
    draw_text();
	glutSwapBuffers() ;
}

/**
 * Inicializar
 */
void vrp_init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.9);
    glPointSize(3.0);    
    sfx_init();
//    music_play("test.ogg");

    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, // cam angle
                    (double)_width / (double)_height, // w/h ratio
                    1.0, // near clipping
                    200.0 // far clipping
    );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

/**
 * Scene switcher
 */
int switch_to(int scene) {
    int current_scene = -1;
    // kill stuff from previous scene
    if (current_scene != -1) {
        generic_handle_keypress = NULL;
        generic_handle_keypress_special = NULL;
        generic_handle_mouse_button = NULL;
        generic_handle_mouse_motion = NULL;
        generic_handle_mouse_passive_motion = NULL;
        generic_handle_draw_scene = NULL;
        generic_handle_idle = NULL;
    }
    
    // load new
    switch (scene) {
        case SCENE_INTRO:
            generic_handle_draw_scene = intro_draw_scene;
            generic_handle_keypress = intro_handle_keypress;
            intro_init();
            current_scene = scene;
            break;
        case SCENE_TITLE:
            generic_handle_draw_scene = title_draw_scene;
            generic_handle_keypress = title_handle_keypress;
            generic_handle_keypress_special = title_handle_keypress_special;
            title_init();
            current_scene = scene;
            break;
        case SCENE_RANKING:
            generic_handle_draw_scene = ranking_draw_scene;
            generic_handle_keypress = ranking_handle_keypress;
            generic_handle_keypress_special = ranking_handle_keypress_special;
            ranking_init();
            current_scene = scene;
            break;
        case SCENE_GAME:
            generic_handle_draw_scene = game_draw_scene;
            generic_handle_idle = game_handle_idle;
            generic_handle_keypress = game_handle_keypress;
            generic_handle_keypress_special = game_handle_keypress_special;
            game_init();
            current_scene = scene;
            break;
    }
    return 1;
}

/**
 * Main
 */
int main(int argc, char ** argv) {
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH) ;   
    glutInitWindowSize(800,600);

    glutCreateWindow("Vector Racer Pro Extreme Edition");
    
    // handlers por todos lados
    glutDisplayFunc(draw_scene);
    glutKeyboardFunc(handle_keypress);
    glutSpecialFunc(handle_keypress_special);
    glutReshapeFunc(handle_resize);
    glutMouseFunc(handle_mouse_button);
    glutMotionFunc(handle_mouse_motion);
    glutPassiveMotionFunc(handle_mouse_passive_motion);
    glutIdleFunc(handle_idle) ;

    vrp_init();
    
    switch_to(INITIAL_SCENE);
    
    glutMainLoop();
    return 0;
}
