/***
 * title.c
 */

#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "imageloader.h"
#include "texture.h"
#include "sfx.h"
#include "main.h"

GLuint _texture_id2; // the id of the texture

unsigned int menu_selected = 0;
char menu_option_strings[4][20] = {"> Play >", "* Ranking *", "$ Options $", "- Exit -"};
int first_time = 1;

int title_draw_scene_fadein(int setup)
{
    static float base_time;
    float elapsed_time;
    float alpha = 0;
    float color = 0;
    
    if (!first_time)
        return 0;
    
    if (setup == 2) {
        elapsed_time = glutGet(GLUT_ELAPSED_TIME) - base_time;
        return (elapsed_time < 4000 ? 1 : 0);
    }
    if (setup == 1)
        base_time = glutGet(GLUT_ELAPSED_TIME);
    else {
        elapsed_time = glutGet(GLUT_ELAPSED_TIME) - base_time;
        
        if (elapsed_time < 3000) { // fade from black
            alpha = (3000 - elapsed_time)/3000;
        } else if (elapsed_time < 4000) { // fade to white
            alpha = (elapsed_time - 3000)/1000;
            color = 1;
        } else if (elapsed_time < 6000) { // fade from white
            alpha = (2000 - (elapsed_time-4000)) / 2000;
            color = 1;
        } else {
            first_time = 0;
            return 0;
        }

        ortho_mode(0, 0, _width, _height);
        
        // debug :D
        sprintf(_message_string, "alpha: %f, color: %f", alpha, color);
        
        // render fader
        glDisable(GL_DEPTH_TEST);
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(color, color, color, alpha);
        glBegin(GL_QUADS);
            glVertex3f(0, _height, 0); // abajo izquierda
            glVertex3f(_width, _height, 0); // abajo derecha
            glVertex3f(_width, 0, 0); // arriba derecha
            glVertex3f(0, 0, 0); // arriba izquierda
        glEnd();
        
        glDisable (GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        perspective_mode();
    }
    return 0;
}

void title_draw_lines(void)
{
    static float last_time = glutGet(GLUT_ELAPSED_TIME);
    float elapsed_time;
    int time = glutGet(GLUT_ELAPSED_TIME);
    int speed = 2;
    float speed1 = 0.1;
    float speed2 = 4;
    int z = 300;
    float x, y;

    glDisable(GL_DEPTH_TEST);
    
    elapsed_time = glutGet(GLUT_ELAPSED_TIME) - last_time;

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glBlendFunc (GL_ONE, GL_ONE);
    
    srand(213); // el mismo seed siempre para que renderee bien

    glBegin(GL_LINES);            
        for (z = 0; z < 600; z++) {
            x = (((time*-speed + (int)(sin(z*1) * 10000))%10000)/100.0) + 50;
//            y = cos(z*10) * 2 * z/50.0;
            y = (((rand()%200)/100.0)-1) * 2 * z/50.0 - 1;
            glColor3f(1, (rand()%100)/100.0, 0);    
            glVertex3f(x, y,-z/6.0);
            glColor3f(0, 0, 0);
            glVertex3f(x + 5, y,-z/6.0);
        }

    glEnd();
    glDisable(GL_BLEND);

    glEnable(GL_DEPTH_TEST);    
    last_time = elapsed_time;
    
}

void title_draw_menu(void)
{
    int i;
    ortho_mode(0, 0, _width, _height);
    int offset;
    float sine_value;

    // blend my friend
    glDisable(GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // background
    glColor4f(0, 0, 0, 0.5);
    glBegin(GL_QUADS);
        glVertex3f(0, _height-150, 0); // abajo izquierda
        glVertex3f(_width, _height-150, 0); // abajo derecha
        glVertex3f(_width, 250, 0); // arriba derecha
        glVertex3f(0, 250, 0); // arriba izquierda
    glEnd();
    
    // text
    for (i = 0; i < 4; i++) {
        sine_value = (sin(i + glutGet(GLUT_ELAPSED_TIME)/300.0))/3 + 0.6;
        
        if (menu_selected == i) {
            glBegin(GL_QUADS);
                glColor4f(1, 0, 0, 0.7);
                glVertex3f(0, _height-290+30*i, 0); // abajo izquierda
                glVertex3f(_width, _height-290+30*i, 0); // abajo derecha
                glVertex3f(_width, _height-290-30+30*i, 0); // arriba derecha
                glVertex3f(0, _height-290-30+30*i, 0); // arriba izquierda
            glEnd();
        } 
        
        sprintf(_debug_string, "sine: %f", sine_value);
        offset = (_width - glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (unsigned char *) menu_option_strings[i]))/2;
        glColor4f(1, 1, 1, sine_value);
        glRasterPos2i(offset, 300 + i*30);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *) menu_option_strings[i]);
    }
        

    offset = (_width - glutBitmapLength(GLUT_BITMAP_HELVETICA_10, (unsigned char *) "Federico Caceres, Sergio Stanichevsky : Diciembre 2009 : Ing. Informatica : Informatica 3 : Trabajo Practico Final"))/2;
    glColor4f(1, 1, 1, 1);
    glRasterPos2i(offset, _height - 10);
    glutBitmapString(GLUT_BITMAP_HELVETICA_10, (unsigned char *) "Federico Caceres, Sergio Stanichevsky : Diciembre 2009 : Ing. Informatica : Informatica 3 : Trabajo Practico Final");
        
    
    glDisable (GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    perspective_mode();
}

/**
 * Dibuja un quad grandote en medio de la pantalla con el logo de la empresa
 * Durante los los primeros segundos hace un fadein,
 * Luego se queda ahí hasta que transcurran 10s
 * Luego hace un fade out
 */
void title_draw_scene(void)
{
    float color = 1; // color para aclarar / oscurecer
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0; // tiempo transcurrido
    const double a = t*90.0; // parametro para funcion senoidal
    float amplitude = 10; // amplitud de la función senoidal
    
    // calcular las Ys de nuestro quad
    float y1 = amplitude * sin(t);
    float y2 = amplitude * sin(t+0.5);
    
    gluLookAt(0.0, 0.0, 0.0,  // donde estoy
        0.0, 0.0, -1.0, // a donde miro
        0.0, 1.0, 0.0 // mi arriba
    );

//    blur_tex_zoom(_texture_id2, 100, 512, 256);

 
    // borrar y preparar colores
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(color,color,color);

    // preparar texturas
//	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _texture_id2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    ortho_mode(0, 0, _width, _height);

    // dibujar el quad
    glPushMatrix();
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(0, 184, 0); // abajo izquierda
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(_width, 184, 0); // abajo derecha
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(_width, 0, 0); // arriba derecha
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(0, 0, 0); // arriba izquierda
        glEnd();
    glPopMatrix();
    
//    glDisable(GL_TEXTURE_2D);
    perspective_mode();

    glDisable(GL_TEXTURE_2D);
    if (!title_draw_scene_fadein(2)) {
        title_draw_lines();
        title_draw_menu();
    }    
    title_draw_scene_fadein(0);
}

void title_handle_keypress(unsigned char key, int x, int y)
{
    switch (key) 
    {
        case ' ':
        case 13:
            if (menu_selected == 0)
                switch_to(SCENE_GAME);
            else if (menu_selected == 1)
                switch_to(SCENE_RANKING);
            else if (menu_selected == 2)
                switch_to(SCENE_OPTIONS);
            else
                exit(0);
            break;
    }
}

void title_handle_keypress_special(int key, int x, int y, int state) {
    if (state) {
        switch (key)
        {
            case GLUT_KEY_DOWN:
                menu_selected = (menu_selected+1)%4;
                break;            
            case GLUT_KEY_UP:
                menu_selected = (menu_selected-1)%4;
                break;            
        }
    }
}

void title_init() {
	glEnable(GL_COLOR_MATERIAL);
    Image* image = loadBMP("img\\vrp.bmp");
	_texture_id2 = texture_load_texture(image);
	delete image;

    menu_selected = 0;

    music_stop(0);
	music_play("test.ogg");
	
	title_draw_scene_fadein(1);
}

