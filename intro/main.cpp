/*
 * FreeGLUT Shapes Demo
 *
 * Written by Nigel Stewart November 2003
 *
 * This program is test harness for the sphere, cone 
 * and torus shapes in FreeGLUT.
 *
 * Spinning wireframe and smooth shaded shapes are
 * displayed until the ESC or q key is pressed.  The
 * number of geometry stacks and slices can be adjusted
 * using the + and - keys.
 */

#include <GL/glut.h>

#include <stdlib.h>
#include <math.h>

#include "imageloader.h"
#include "texture.h"

#include "sfx.h"

int _width;
int _height;

GLuint _texture_id; // the id of the texture

/* GLUT callback Handlers */

static void 
resize(int width, int height)
{
    const float ar = (float) width / (float) height;
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
    
    _width = width;
    _height = height;
}

static void 
display(void)
{
    float color = 0;
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    const double a = t*90.0;
    
    float y1 = sin(t)/10;
    float y2 = sin(t+0.5)/10;
    
    if (glutGet(GLUT_ELAPSED_TIME) < 2000)
        color = glutGet(GLUT_ELAPSED_TIME)/2000.0;
    else if (glutGet(GLUT_ELAPSED_TIME) > 15000)
        exit(1);
    else if (glutGet(GLUT_ELAPSED_TIME) > 10000)
        color = 1 - (glutGet(GLUT_ELAPSED_TIME)-10000)/5000.0;
    else
        color = 1;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(color,color,color);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPushMatrix();
        glTranslatef(0.0, -3.0, 4.0);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(-4, y1, -10);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(4, y2, -10);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(4, y2+6, -10);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(-4, y1+6, -10);            
        glEnd();
    glPopMatrix();

    glutSwapBuffers();
}

static void 
key(unsigned char key, int x, int y)
{
    switch (key) 
    {
        case 27 : 
        case 'q':
            exit(0);
            break;
    }

    glutPostRedisplay();
}

static void 
idle(void)
{
    glutPostRedisplay();
}

void init() {
	glEnable(GL_COLOR_MATERIAL);
    Image* image = loadBMP("img\\penguin_pirate_large.bmp");
	_texture_id = texture_load_texture(image);
	delete image;
}

/* Program entry point */

int 
main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(800,600);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("FreeGLUT Shapes");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    glClearColor(0,0,0,1);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    init();
    sfx_init();

    sfx_play("music\\logo music.mp3");

    glutMainLoop();

    return EXIT_SUCCESS;
}

