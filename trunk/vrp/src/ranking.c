/***
 * ranking.c
 */

#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <windows.h> // for INI parsing and saving
#include "imageloader.h"
#include "texture.h"
#include "sfx.h"
#include "main.h"

#include "texture_processing.h"

struct ranking
{
    char name[64];
    int score;
};

struct ranking rankings[10];

GLuint _texture_id3; // the id of the texture


int _new_record = 0;

void ranking_draw_scene_fadein(int setup)
{
    static float base_time;
    float elapsed_time;
    float alpha = 0;
    float color = 0;
    
    if (setup == 1)
        base_time = glutGet(GLUT_ELAPSED_TIME);
    else {
        ortho_mode(0, 0, _width, _height);

        elapsed_time = glutGet(GLUT_ELAPSED_TIME) - base_time;
        
        if (elapsed_time < 3000) {
            alpha = (3000 - elapsed_time)/3000;
        } else if (elapsed_time < 4000) {
            alpha = (elapsed_time - 3000)/1000;
            color = 1;
        } else if (elapsed_time < 6000) {
            alpha = (2000 - (elapsed_time-4000)) / 2000;
            color = 1;
        } else {
            return;
        }
        
        // debug :D
        sprintf(_debug_string, "alpha: %f, color: %f", alpha, color);
        
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
}

void ranking_draw_lines(void)
{
    static float last_time = glutGet(GLUT_ELAPSED_TIME);
    float elapsed_time;
    int time = glutGet(GLUT_ELAPSED_TIME);
    int speed = 2;
    float speed1 = 0.1;
    float speed2 = 2;
    int z = 300;
    float x, y;
    
    elapsed_time = glutGet(GLUT_ELAPSED_TIME) - last_time;

/*        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
*/
    
    glBegin(GL_LINES);
    glColor4f(1, 1, 0.5, 0.5);
        for (z = 0; z < 600; z ++) {
            x = (((time*-speed + (int)(sin(z*1) * 10000))%10000)/100.0) + 50;
            y = cos(z*10) * 2;
//            y = 1;
            glVertex3f(x, y,-z/6.0);
            glVertex3f(x + 2, y,-z/6.0);
        }

    glEnd();
    
    
    last_time = elapsed_time;
    
}

char * llenar_de_puntos(int cantidad)
{
    static char puntos[51];
    int i;
    for (i =0; i < cantidad; i++) puntos[i] = '.';
    puntos[i] = 0;
    return puntos;
}

void ranking_draw_scores(void)
{
    int i;
    ortho_mode(0, 0, _width, _height);
    int offset;
    float sine_value;
    
    char buffer[256];


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
    for (i = 0; i < 10; i++) {
        sine_value = (sin(i + glutGet(GLUT_ELAPSED_TIME)/300.0))/3 + 0.6;

        /*
        if (menu_selected == i) {
            glBegin(GL_QUADS);
                glColor4f(1, 0, 0, 0.7);
                glVertex3f(0, _height-290+30*i, 0); // abajo izquierda
                glVertex3f(_width, _height-290+30*i, 0); // abajo derecha
                glVertex3f(_width, _height-290-30+30*i, 0); // arriba derecha
                glVertex3f(0, _height-290-30+30*i, 0); // arriba izquierda
            glEnd();
        }*/ 
        
        sprintf(_debug_string, "sine: %f", sine_value);
        sprintf(buffer, "%d", rankings[i].score);
        sprintf(buffer, "%s%s%d", rankings[i].name,llenar_de_puntos(60 - strlen(rankings[i].name) - strlen(buffer)), rankings[i].score);
        offset = (_width - glutBitmapLength(GLUT_BITMAP_9_BY_15, (unsigned char *) buffer))/2;
        glColor4f(1, 1, 1, sine_value);
        glRasterPos2i(offset, 230 + i*30);
        glutBitmapString(GLUT_BITMAP_9_BY_15, (unsigned char *) buffer);
    }
        

    offset = (_width - glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (unsigned char *) "PRESS ANY KEY TO CONTINUE"))/2;
    glColor4f(1, 1-sine_value, 1-sine_value, 1);
    glRasterPos2i(offset, _height - 70);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char *) "PRESS ANY KEY TO CONTINUE");
        
    
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
void ranking_draw_scene(void)
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

//    blur_tex_zoom(_texture_id3, 100, 512, 256);

 
    // borrar y preparar colores
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(color,color,color);

    // preparar texturas
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _texture_id3);
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
    
    glDisable(GL_TEXTURE_2D);
    perspective_mode();

    ranking_draw_scores();

/*    title_draw_lines();
    title_draw_menu();*/
    
//    title_draw_scene_fadein(0);


}

void ranking_handle_keypress(unsigned char key, int x, int y)
{
    if (_new_record) {
        if (key >= ' ' && key <= '~') {
           // DO SOMETHING!
        }
    } else {
        switch (key) 
        {
            default:
                switch_to(SCENE_TITLE);
                break;
        }
    }
}


void ranking_handle_keypress_special(int key, int x, int y) {
    switch (key)
    {
    }
}

void parse_ranking()
{
    FILE * fhandle;
    char c;
    int i=0,j=0;
    char buffer[512];
    
    fhandle = fopen("ranking.txt", "r");
    while(i < 10 && !feof(fhandle)) {
        c = fgetc(fhandle);
        if (c!=',') {
            if (c != '\n' && c != '\r')
                buffer[j++] = c;
        } else {
            buffer[j] = 0;
            strcpy(rankings[i].name, buffer);
            fscanf(fhandle, "%d", &(rankings[i].score));
            j = 0;
            if (strlen(rankings[i].name))
                sprintf(_message_string, "%d : %s = %d ... %d", i, rankings[i].name, rankings[i].score, strlen(rankings[i].name));
            i++;
        }
    }
    fclose(fhandle);
}

void ranking_init() {
	glEnable(GL_COLOR_MATERIAL);
    Image* image = loadBMP("img\\vrp.bmp");
	_texture_id3 = texture_load_texture(image);
	delete image;

    music_stop(0);
	music_play("test.ogg");
	
	ranking_draw_scene_fadein(1);
	parse_ranking();
}

