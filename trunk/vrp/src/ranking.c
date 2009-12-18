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

struct ranking
{
    char name[64];
    int score;
};

GLuint _texture_id3; // the id of the texture

struct ranking rankings[10];
char _new_name[64] = {0};
int _new_name_cursor = 0;
int _new_record = -1; // posición del nuevo record
int _was_best = 0;

void save_ranking();

/**
 * Setear u obtener el ultimo score obtenido por jugador
 * 0 = get
 * 1 = set
 */
int latest_score(int get_set, unsigned int score) {
    static unsigned int lastest_score = 0;
    if (get_set) {
        lastest_score = score;
    } else {
        return lastest_score;
    }
}

/**
 * Llenar de puntitos
 */
char * llenar_de_puntos(int cantidad)
{
    static char puntos[51];
    int i;
    for (i =0; i < cantidad; i++) puntos[i] = '.';
    puntos[i] = 0;
    return puntos;
}

/**
 * Dibujar puntajes
 */
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

    // new record!
    if (_new_record > -1) {
        sine_value = (sin(glutGet(GLUT_ELAPSED_TIME)/300.0))/3 + 0.6;
        sprintf(buffer, "NEW RECORD! Type your name: %s", _new_name);
        offset = (_width - glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (unsigned char *) buffer))/2;
        glColor4f(1, 1, 1-sine_value, 1);
        glRasterPos2i(offset, 170);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *) buffer);
    }

    // text
    for (i = 0; i < 10; i++) {
        sine_value = (sin(i + glutGet(GLUT_ELAPSED_TIME)/300.0))/3 + 0.6;

        if (_new_record == i) {
            glBegin(GL_QUADS);
                glColor4f(1, 0, 0, 0.7);
                glVertex3f(0, 215+30*i, 0); // abajo izquierda
                glVertex3f(_width, 215+30*i, 0); // abajo derecha
                glVertex3f(_width, 240-30+30*i, 0); // arriba derecha
                glVertex3f(0, 240-30+30*i, 0); // arriba izquierda
            glEnd();
        }
        
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
 
    // borrar y preparar colores
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(color,color,color);

    // preparar texturas
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
}

void ranking_handle_keypress(unsigned char key, int x, int y)
{
    // si hay un nuevo record, simular un INPUT BOX
    if (_new_record > -1) {
        if (key >= ' ' && key <= '~' && key != ',') {
            if (_new_name_cursor < 20) {
                _new_name[_new_name_cursor] = key;
                _new_name_cursor++;
            }
        } else if (key == 8) {
            if (_new_name_cursor > 0) {
                _new_name_cursor--;
            }
            _new_name[_new_name_cursor] = 0;
        } else if (key == 13) {
            save_ranking();
            latest_score(1, 0);
            switch_to(SCENE_RANKING);
        }
    // si no hay nuevo record... todo normal nomás
    } else {
        switch (key) 
        {
            default:
                switch_to(SCENE_TITLE);
                break;
        }
    }
}

void ranking_handle_keypress_special(int key, int x, int y, int state) {
    switch (key)
    {
    }
}

/**
 * Leer el archivo de ranking y comparar con el nuevo record (si es que lo hay)
 */
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
    
    // ver si el nuevo score es mejor que alguno del ranking en el archivo
    for (i = 0; i < 10; i++) {
        if (latest_score(0,0) >= rankings[i].score) {
            _new_record = i;
            sprintf(_message_string, "FOUND NEW RECORD %d", i);
            break;
        }
    }
}

/**
 * Guardar el ranking, incluyendo el nuevo record, desplazando todos los demás hacia abajo
 */
void save_ranking()
{
    FILE * fhandle;
    char c;
    int i=0;
    
    fhandle = fopen("ranking.txt", "w");
    for (i = 0; i < 10; i++) {
        if (_new_record < i)
            fprintf(fhandle, "%s,%d\r\n", rankings[i-1].name, rankings[i-1].score);
        else if (_new_record > i)
            fprintf(fhandle, "%s,%d\r\n", rankings[i].name, rankings[i].score);
        else {
            fprintf(fhandle, "%s,%d\r\n", _new_name, latest_score(0,0));
        }
    }
    fclose(fhandle);
}

/**
 * Init
 */
void ranking_init() {
    int i;
    
	glEnable(GL_COLOR_MATERIAL);
    Image* image = loadBMP("img\\vrp.bmp");
	_texture_id3 = texture_load_texture(image);
	delete image;

    for (i = 0; i < 64; i++) _new_name[i] = 0;
    _new_name_cursor = 0;
    _new_record = -1;
	
	parse_ranking();

    if (!_was_best) {
        music_stop(0);
        if (_new_record > -1) {
        	music_play("69 - Hurrah for the Champion (Winning Run).mp3");
            // esto es para que al cambiar de escena, de escribir a ver, sigua la musica    
            _was_best = 1;
        } else {
        	music_play("60 - Step 70's (Option).mp3");
        }
    } else {
        _was_best = 0;
    }
    	
}

