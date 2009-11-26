/***
 * title.c
 */

#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "imageloader.h"
#include "texture.h"
#include "sfx.h"
#include "main.h"

GLuint _texture_id2; // the id of the texture

void title_draw_scene_fadein(int setup)
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

void title_draw_lines(void)
{
    static float last_time = glutGet(GLUT_ELAPSED_TIME);
    float elapsed_time;
    int time = glutGet(GLUT_ELAPSED_TIME);
    float speed1 = 0.1;
    float speed2 = 2;
    int x;
    
    elapsed_time = glutGet(GLUT_ELAPSED_TIME) - last_time;
    
    glColor3f(1,1,1);
    glBegin(GL_LINES);
        for (x = 0; x < 300; x ++) {
            glVertex3f((((time + (int)(sin(x) * 10000))%10000)/100.0) - 50,cos(x) * 2,-x/6.0);
            glVertex3f((((time + (int)(sin(x) * 10000))%10000)/100.0) - 50 + 1,cos(x) * 2,-x/6.0);
        }

    glEnd();
    
    
    last_time = elapsed_time;
    
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

 
    // borrar y preparar colores
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(color,color,color);

    // preparar texturas
	glEnable(GL_TEXTURE_2D);
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
    
    glDisable(GL_TEXTURE_2D);
    perspective_mode();

    title_draw_lines();
    
//    title_draw_scene_fadein(0);


}

void title_handle_keypress(unsigned char key, int x, int y)
{
    switch (key) 
    {
        case ' ' : 
        case 13:
            exit(0);
            break;
    }
}

void title_init() {
	glEnable(GL_COLOR_MATERIAL);
    Image* image = loadBMP("img\\vrp.bmp");
	_texture_id2 = texture_load_texture(image);
	delete image;

    music_stop(0);
	music_play("test.ogg");
	
	title_draw_scene_fadein(1);
}

