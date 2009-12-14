/*
 * game.cpp
 * @description: Implementación de la graficación y lógica del juego
 */

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "main.h"
#include "texture_processing.h"
#include "imageloader.h"
#include "texture.h"
#include "utils.h"
#include "sfx.h"

#define AXIS_SIZE 10

#define ROAD_MIN 0
#define ROAD_MAX 145
#define ROAD_STEPS 35
#define ROAD_WIDTH 5

#define LOOK_AT_X 0
#define LOOK_AT_Y 10
#define LOOK_AT_Z -ROAD_MAX

#define BASE_SPEED 0.10 / 25

struct Punto3D {
    float x;
    float y;
    float z;
};

// BLUR STUFF
GLuint _tex_0_id4; // textura donde se almacenara el "blur"
int _tex_0_size = 512; // tamaño de la textura
//float blur_zoom = 0; // factor de "zoom" de la textura
//  GENERAL
int _polygon_mode = GL_FILL; // modo de rellenado
float _anglez = 90.0; // angulo x
float _angley = 75.0; // angulo y
float _distance = 30; // distancia al origen
int _x; // vieja posicion del mouse en x
int _y; // vieja posicion del mouse en y
bool _dragging = false; // arrastrando?

float delta = 0;

// CARRETERA
float curvatura_h = 100000.0;
float curvatura_v = 100000.0;
int sentido_h = 1;
int sentido_v = 1;

float _speed = 1.1;

// CAMARA
int current_cam = 0;


/*
 * Inicialización
 */
void game_init() {
    GLfloat density = 0.3; //set the density to 0.3 which is acctually quite thick
    GLfloat fogColor[4] = {0, 0, 0, 1.0}; //set the for color to grey
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.9);
    glPointSize(3.0);
    texture_create_blank_render_texture( &_tex_0_id4, 512, 3, GL_RGB);

    glEnable (GL_DEPTH_TEST); //enable the depth testing

    glFogfv (GL_FOG_COLOR, fogColor); //set the fog color to our color chosen above
    glFogf (GL_FOG_DENSITY, density); //set the density to the value above
    glFogi (GL_FOG_MODE, GL_LINEAR); //set the fog mode to GL_EXP2
    glFogf(GL_FOG_START,20.0);                   /* Where wwe start fogging */
    glFogf(GL_FOG_END,40.0);                       /* end */

    glHint (GL_FOG_HINT, GL_NICEST); // set the fog to look the nicest, may slow down on older cards

    //glEnable (GL_FOG); //enable the fog

    music_stop(0);
	music_play("g-storm.mp3");
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
            _speed -= 0.1;
            break;
        case '.':
            _speed += 0.1;
            break;
    }
    
    sentido_h = (xx > 0 ? 1 : -1);
    curvatura_h = (xx == 0 ? 100 : 1 * sqrt(1/xx*1/xx)+5);
    sentido_v = (yy > 0 ? 1 : -1);
    curvatura_v = (yy == 0 ? 100 : 1 * sqrt(1/yy*1/yy)+5);
}

/**
 * Manejo de teclas especiales (flechas, función, etc)
 */
void game_handle_keypress_special(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            _anglez = wrap_f(_anglez, 3.0, 0.0, 360.0);
            break;
        case GLUT_KEY_RIGHT:
            _anglez = wrap_f(_anglez, -3.0, 0.0, 360.0);
            break;
        case GLUT_KEY_UP:
            _angley = wrap_f(_angley, -3.0, 0.0, 360);
            break;
        case GLUT_KEY_DOWN:
            _angley = wrap_f(_angley, 3.0, 0.0, 360);
            break;
        case GLUT_KEY_F2:
            current_cam = !current_cam;
            break;
    }
}


/**
 * Manejo de clics del mouse
 */
void game_handle_mouse_button (int button, int state, int x, int y) {
//	printf("BotÃ³n del mouse: %d\n",button);
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN)
			_dragging = true;
		else
			_dragging = false;
	} else if ((button == 3 || button == 4) && state == GLUT_UP) {
		_distance = constraint_f(_distance, -(3.5 - button) * 4, 5, 200);
			;
	}

}

/**
 * Manejo de movimiento del mouse con boton presionado
 */
void game_handle_mouse_motion (int x, int y) {
	_anglez = wrap_f(_anglez, 180 *((float)x - _x)/_height, 0, 360);
	_angley = wrap_f(_angley, 180 *((float)_y - y)/_width, 0, 360);
	_x = x;
	_y = y;
}

/**
 * Manejo de movimiento del mouse sin boton presionado
 */
void game_handle_mouse_motion_passive (int x, int y){
	_x = x;
	_y = y;
}

/**
 * Dibujar el eje de coordenadas 3d
 */
static void dibujar_eje() {
    // x
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(AXIS_SIZE, 0.0, 0.0);
    glEnd();
    // x cone
    glPushMatrix();
    glTranslatef(AXIS_SIZE, 00.0, 0.0);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glutSolidCone(0.25, 0.5, 16, 2);
    glPopMatrix();

    // y
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, AXIS_SIZE, 0.0);
    glEnd();
    // y cone
    glPushMatrix();
    glTranslatef(0.0, AXIS_SIZE, 0.0);
    glRotatef(90.0, -1.0, 0.0, 0.0);
    glutSolidCone(0.25, 0.5, 16, 2);
    glPopMatrix();

    // z
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, AXIS_SIZE);
    glEnd();
    // z cone
    glPushMatrix();
    glTranslatef(0.0, 0.0, AXIS_SIZE);
    glutSolidCone(0.25, 0.5, 16, 2);
    glPopMatrix();
}

/**
 * Debijar la grilla de "suelo"
 */
static void dibujar_grid() {
    float x;
    float y;
    int a = 0;
    static const float zeta = 0.05; 

    glBegin(GL_LINES);
        for (x = 0.0; x <= 20.0; x += 0.5, a++) {
            if (a%nearest_f(_distance, 2.5))
        	//if (a%10)
                glColor3f(0.06, 0.11, 0.15);
            else
                glColor3f(0.10 - zeta, 0.15 - zeta, 0.19 - zeta);
            // x <--->
            glVertex3f(x, 0.0, -20.0);
            glVertex3f(x, 0.0, 20.0);
            glVertex3f(-x, 0.0, -20.0);
            glVertex3f(-x, 0.0, 20.0);
            // z <--->
            glVertex3f(-20.0, 0.0, x);
            glVertex3f(20.0, 0.0, x);
            glVertex3f(-20.0, 0.0, -x);
            glVertex3f(20.0, 0.0, -x);

        }
    glEnd();
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
    
    i = 0 - delta;
    for (j = 0; j < ROAD_STEPS; j++ ) {
        a = i / curvatura_h; // auxiliar para calcular 'x' y 'z' (es el parametro de la función f(a) = a^2, se divide por curvatura para obtener una porción mayor o menor de la formula
        aux_x = (a*a)*sentido_h; // coordenada de x en el centro de la carretera
        
        b = i / curvatura_v; // // auxiliar para calcular 'y' (es el parametro de la función f(b) = b^2, se divide por curvatura para obtener una porción mayor o menor de la formula
        y = (b*b)*sentido_v/3; // coordenada y de la carretera

        // ahora unas cosas que usan trigonometría
        // hallar el incremente de x con respecto al centro de la carretera (esto es para que la curvatura se vea real)
        if (a == 0)
            x = ROAD_WIDTH;
        else
            x = sin(atan(-1/(2*(a))*curvatura_h*sentido_h)) * ROAD_WIDTH * sentido_h;
        // hallar el incremente de z (esto es para que la curvatura se vea real)
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
        
        // incrementar i, nuestro parametro global (y también z)
        i += (ROAD_MAX-ROAD_MIN)/(float)ROAD_STEPS;
    }
    
    // el suelo es transparente, habilitar transparencia
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // dibujar la carretera    
    for (j = ROAD_STEPS-1; j >= 0; j--) {
        // el suelo es transparente, dibujar un quad por cada par de puntos3d (menos para el último, ya que no existe vrts[0-1]
        if (j > 0) {
            glBegin(GL_QUADS);
                i = (j%2 ? 0.10 : 0.05);
                glColor4f(i, i, i, 0.75); 
                glVertex3f(vrts[j][0].x+vrts[j][1].x,vrts[j][0].y-0.07,vrts[j][0].z-vrts[j][1].z);
                glVertex3f(vrts[j][0].x-vrts[j][1].x,vrts[j][0].y-0.07,vrts[j][0].z+vrts[j][1].z);
                glVertex3f(vrts[j-1][0].x-vrts[j-1][1].x,vrts[j-1][0].y-0.07,vrts[j-1][0].z+vrts[j-1][1].z);
                glVertex3f(vrts[j-1][0].x+vrts[j-1][1].x,vrts[j-1][0].y-0.07,vrts[j-1][0].z-vrts[j-1][1].z);
            glEnd();
        }

        // dibujar las lineas
        glBegin(GL_LINES);
        glColor4f(1.0, 1.0, 1.0, 1.0);
            // sobre el suelo
            glColor3f(0.5,0.5,0.5);
            glVertex3f(vrts[j][0].x+vrts[j][1].x,vrts[j][0].y,vrts[j][0].z-vrts[j][1].z);
            glVertex3f(vrts[j][0].x-vrts[j][1].x,vrts[j][1].y,vrts[j][0].z+vrts[j][1].z);
            // paredes
            glColor3f(1,1,1);            
            glVertex3f(vrts[j][0].x+vrts[j][1].x,vrts[j][0].y,vrts[j][0].z-vrts[j][1].z);
            glVertex3f(vrts[j][0].x+vrts[j][1].x,vrts[j][0].y+0.5,vrts[j][0].z-vrts[j][1].z);
            glVertex3f(vrts[j][0].x-vrts[j][1].x,vrts[j][1].y,vrts[j][0].z+vrts[j][1].z);
            glVertex3f(vrts[j][0].x-vrts[j][1].x,vrts[j][1].y+0.5,vrts[j][0].z+vrts[j][1].z);

            // laterales
/*            glVertex3f(j-x,c+0.5,-i+y);
            glVertex3f(j-x*10,c+0.5,-i+y*10);
            glVertex3f(j+x,c+0.5,-i-y);
            glVertex3f(j+x*10,c+0.5,-i-y*10);*/
        glEnd();

/*
            glEnable (GL_BLEND);
            glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(0.75, 0.75, 0.75, 1.0);
            glVertex3f(j-x,c+0.5,-i+y);
            glColor4f(0.0, 0.0, 0.0, 0.0);
            glVertex3f(j-x*10,c+0.5,-i+y*10);
            glColor4f(0.75, 0.75, 0.75, 1.0);
            glVertex3f(j+x,c+0.5,-i-y);
            glColor4f(0.0, 0.0, 0.0, 0.0);
            glVertex3f(j+x*10,c+0.5,-i-y*10);
            glDisable(GL_BLEND);*/

    }

    glDisable(GL_BLEND);
}

bool AnimateNextFrame(int desiredFrameRate)
{
	static float lastTime = 0.0f;
	float elapsedTime = 0.0;

	// Get current time in seconds  (milliseconds * .001 = seconds)
    float currentTime = GetTickCount() * 0.001f; 

	// Get the elapsed time by subtracting the current time from the last time
	elapsedTime = currentTime - lastTime;

	// Check if the time since we last checked is over (1 second / framesPerSecond)
    if( elapsedTime > (1.0f / desiredFrameRate) )
    {
		// Reset the last time
        lastTime = currentTime;	

		// Return TRUE, to animate the next frame of animation
        return true;
    }

	// We don't animate right now.
	return false;
}


static void calcular_coordenadas(struct Punto3D entrada, struct Punto3D * salida) {
    // curva para el plano xz
    float aux = entrada.z / curvatura_h; // aux
    float aux_x = (aux*aux)*sentido_h; // x
    
    // curva para y
    float aux2 = entrada.z / curvatura_v; // aux
    float aux_y = (aux2*aux2)*sentido_v/3.0; // y
    
    // hallar el x y z real
    if (aux == 0)
        salida->x = entrada.x;
    else
//        salida->x = aux_x;
        salida->x = aux_x + sin(atan(-1/(2*(aux))*curvatura_h * sentido_h)) * entrada.x * sentido_h;


    if (aux == 0)
        salida->z = entrada.z;
    else
///        salida->z = aux_z;
        salida->z = entrada.z + cos(atan(-1/(2*(aux))*curvatura_h*sentido_h)) * entrada.x * sentido_h;
        
    salida->y = aux_y/* + entrada.y*/;
    
    //salida->z -= 2;
    
    sprintf(_message_string, "Curvatura V: %f %d, aux2>%f,aux_y>%f", curvatura_v, sentido_v, aux2,aux_y);
    sprintf(_debug_string, "entrada: %f,%f,%f, salida: %f,%f,%f", entrada.x,entrada.y,entrada.z, salida->x,salida->y,salida->z);
}

void dibujar_cosa() {
    static float decremento = 0.01;
    struct Punto3D a, b, c, d;
    
    a.x = 3;
    a.y = 0;
    a.z = -35.0 + decremento;
//    decremento += 0.04;
    decremento = wrap_f(decremento, 3 * BASE_SPEED * _speed, -ROAD_MAX, 35);
    
    //delta = wrap_f(delta, (time - elapsed_time) * _speed * BASE_SPEED, ROAD_MIN, (ROAD_MAX - ROAD_MIN)*2/ROAD_STEPS); /* este ultimo es (max - min) / steps */
    
    calcular_coordenadas(a, &b);
    
    c.x = -b.x;
    c.y = b.y;
    c.z = b.z;

    d.x = 0;
    d.y = b.y;
    d.z = b.z;
    
    glPushMatrix();
    glTranslatef(b.x, b.y, b.z);
//    glRotatef(90.0, 0.0, 0.0, 0.0);
    glColor3f(1,.5,0);
    glutSolidCone(0.25, 0.5, 16, 2);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(d.x, d.y, d.z);
    glRotatef(90.0, -1.0, 0, 0.0);
    glColor3f(.5,1,0);
    glutSolidCone(0.25, 0.5, 16, 2);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(c.x, c.y, c.z);
    glRotatef(90.0, -1.0, -1.0, 0.0);
    glColor3f(0,.5,1);
    glutSolidCone(0.25, 0.5, 16, 2);
    glPopMatrix();
    
}

void dibujar_mira() {
   struct Punto3D a;
   
   a.x = LOOK_AT_X;
   a.y = LOOK_AT_Y;
   a.z = LOOK_AT_Z;
   
   calcular_coordenadas(a, &a);
   
    glPushMatrix();
    glTranslatef(a.x, a.y, a.z);
//    glRotatef(90.0, 0.0, 0.0, 0.0);
//    glColor3f(1,.5,0);
//    glScalef(5,5,5);
    
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
    
    glPopMatrix();
   
}

void dibujar_horizonte() {
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.5);
    glBegin(GL_LINES);
        glVertex3f(-100000, 0, -ROAD_MAX);
        glVertex3f(100000, 0, -ROAD_MAX);
    glEnd();
    glPopMatrix();
}

void do_draw(GLuint textureID) {
 
    glEnable(GL_TEXTURE_2D);   
    if(textureID >= 0) glBindTexture(GL_TEXTURE_2D, textureID);
    
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);

    glPolygonMode(GL_FRONT_AND_BACK, _polygon_mode);

    // esto se pinta sin luz ---
    glDisable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
//        dibujar_grid();
        //dibujar_eje();
        dibujar_horizonte();
        glTranslatef(0.0, 0.0, 20.0);

        dibujar_cosa();
        dibujar_mira();
        dibujar_carretera();
        //draw_text();
    glPopMatrix();
}



/**
 * Dibujar la escena
 */
void game_draw_scene() {
   struct Punto3D a;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear all
    glLoadIdentity(); // reset drawing perspective
   
//    glMatrixMode(GL_MODELVIEW); // switch to drawing perspective

//    glRotatef(-_camera_angle, 0.1f, -.1f, 1.0f); // rotate camera
//    glTranslatef(0.0f, 0.0f, -5.0f); // moveeee


   
   a.x = LOOK_AT_X;
   a.y = LOOK_AT_Y;
   a.z = LOOK_AT_Z;
   
   calcular_coordenadas(a, &a);


    if (current_cam == 0) {
        gluLookAt(sin(_angley*3.14/180) * cos(_anglez*3.14/180) * _distance, cos(_angley*3.14/180) * _distance, sin(_angley*3.14/180) * sin(_anglez*3.14/180) * _distance,  // donde estoy
            0.0, 0.0, 0.0, // a donde miro
            0.0, (_angley >= 180 ? -1.0 : 1.0), 0.0 // mi arriba
        );
    } else {
        gluLookAt(
            -a.x/50,2-a.y/50,22,
            a.x/2,a.y/2,a.z,
            0, 1, 0
        );
    }

    

    if( AnimateNextFrame(60)) {
        glViewport(0, 0, _tex_0_size, _tex_0_size);	
        render_motion_blur( _tex_0_id4);
        // draw
        do_draw(0);
   		glBindTexture(GL_TEXTURE_2D,_tex_0_id4);
		// Render the current screen to our texture
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, _tex_0_size, _tex_0_size, 0);

		// Here we clear the screen and depth bits of the small viewport
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			

		// Set our viewport back to it's normal size
		glViewport(0, 0, _width, _height);	
	}
	render_motion_blur( _tex_0_id4);
//    blur_tex_zoom(g_tex_0_id4, 1);
    do_draw(0);
    //draw_text();
//	glutSwapBuffers() ;
}

void game_handle_idle() {
    static int elapsed_time = 0;
    int time;
	time = glutGet(GLUT_ELAPSED_TIME);
//    delta = wrap_f(delta, (time - elapsed_time) * _speed * 0.05 / 25, ROAD_MIN, (ROAD_MAX - ROAD_MIN)*2/ROAD_STEPS); /* este ultimo es (max - min) / steps */
    delta = wrap_f(delta, (time - elapsed_time) * _speed * BASE_SPEED, ROAD_MIN, (ROAD_MAX - ROAD_MIN)*2/ROAD_STEPS); /* este ultimo es (max - min) / steps */
    
    elapsed_time = time;
}
