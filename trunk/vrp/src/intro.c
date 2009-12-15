/***
 * intro.c
 */

#include <GL/freeglut.h>
#include <stdlib.h>
#include <math.h>
#include "imageloader.h"
#include "texture.h"
#include "sfx.h"
#include "main.h"
#include "scene_switcher.h"

GLuint _texture_id; // the id of the texture

/**
 * Dibuja un quad grandote en medio de la pantalla con el logo de la empresa
 * Durante los los primeros segundos hace un fadein,
 * Luego se queda ahí hasta que transcurran 10s
 * Luego hace un fade out
 */
void intro_draw_scene(void)
{
    float color = 0; // color para aclarar / oscurecer
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0; // tiempo transcurrido
    const double a = t*90.0; // parametro para funcion senoidal
    float amplitude = 10; // amplitud de la función senoidal
    
    // calcular las Ys de nuestro quad
    float y1 = amplitude * sin(t);
    float y2 = amplitude * sin(t+0.5);
    
    // desastres de tiempos transcurridos
    if (glutGet(GLUT_ELAPSED_TIME) < 2000)
        color = glutGet(GLUT_ELAPSED_TIME)/2000.0;
    else if (glutGet(GLUT_ELAPSED_TIME) > 15000) {
        switch_to(SCENE_TITLE);
    } else if (glutGet(GLUT_ELAPSED_TIME) > 10000)
        color = 1 - (glutGet(GLUT_ELAPSED_TIME)-10000)/5000.0;
    else {
        color = 1;
    }

    // borrar y preparar colores
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(color,color,color);

    // preparar texturas
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _texture_id);
/*	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
*/
    ortho_mode(0, 0, _width, _height);

    // dibujar el quad
    glPushMatrix();
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(-amplitude, y1+_height+amplitude, 0); // abajo izquierda
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(_width+amplitude, y2+_height+amplitude, 0); // abajo derecha
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(_width+amplitude, y2-amplitude, 0); // arriba derecha
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(-amplitude, y1-amplitude, 0); // arriba izquierda
        glEnd();
    glPopMatrix();
    
    perspective_mode();
}

void intro_handle_keypress(unsigned char key, int x, int y)
{
    switch (key) 
    {
        case ' ' : 
        case 13:
            switch_to(SCENE_TITLE);
            break;
    }
}

void intro_init() {
	glEnable(GL_COLOR_MATERIAL);
    Image* image = loadBMP("img\\penguin_pirate_large.bmp");
	_texture_id = texture_load_texture(image);
	delete image;

	music_play("logo music.ogg");
}

