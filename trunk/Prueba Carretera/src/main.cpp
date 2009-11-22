/**
 * Info 3 - OpenGl - Clase 3
 * @author: Federico Cáceres <fede.caceres@gmail.com> 051461
 * @description: Uso de mouse y teclado para movimiento, luces y materiales
 */

#define AXIS_SIZE 10.0

#include <iostream>
#include <fstream>
//#include <stdlib.h> // exit
#include <math.h>

//#include <GL/glut.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

float fps = 0;

/**
 ** BLUR STUFF
 **/
GLuint g_tex_0_ID;
int g_Viewport = 512;
float blur_zoom = 0;
//uint g_texID;

using namespace std;

int _polygon_mode = GL_FILL; // modo de rellenado
float _anglez = 90.0; // angulo x
float _angley = 75.0; // angulo y
float _distance = 30; // distancia al origen
int _x; // vieja posicion del mouse en x
int _y; // vieja posicion del mouse en y
int _height; // altura de la ventana
int _width; // ancho de la ventana
bool _dragging = false; // arrastrando?

float delta = 0;

float curvatura_h = 100.0;
float curvatura_v = 100.0;
int sentido_h = 1;
int sentido_v = 1;

/* == Utilitarios == */

/**
 * Aproximar value a la siguiente potencia de increment
 */
int nearest_f(float value, int increment) {
	int x = increment;
	while (x * increment < value) x = x * increment;
	return x;
}

/**
 * Restringir a value incrementado en increment entre minimum y maximum
 */
float constraint_f(float value, float increment, float minimum, float maximum) {
    value += increment;
    if (value < minimum) value = minimum;
    if (value > maximum) value = maximum;
    return value;
}

/**
 * Incrementar value en increment y si supera maximun, hacer que desbore a minimum o vice versa
 */
float wrap_f(float value, float increment, float minimum, float maximum) {
    value += increment;
    if (value < minimum) value += maximum;
    if (value > maximum) value -= maximum;
    return value;
}

/**
 * Cambia la variable global de tipo de dibujo rellenado o wireframe
 */
void toggle_polygon_mode() {
    if (_polygon_mode == GL_FILL)
        _polygon_mode = GL_LINE;
    else
        _polygon_mode = GL_FILL;
}

/**
 * Manejo de teclas convencionales
 */
void handle_keypress(unsigned char key, int x, int y) {
    static float xx = 0;
    static float yy = 0;
    switch (key) {
        case 27: //Esq
            exit(0);
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
/*		case 'a':
        case 'A':
            if (sentido_h == 1) {// -->
                if (curvatura_h > 100) // recto
                    sentido_h = -1;
                else // curvo
                    curvatura_h++;
            } else { // <--
                if (curvatura_h > 8) { // curvo
                    curvatura_h--;
                    if (curvatura_h > 20) curvatura_h --;
                    if (curvatura_h > 30) curvatura_h --;
                    if (curvatura_h > 40) curvatura_h --;
                    if (curvatura_h > 50) curvatura_h --;
                    if (curvatura_h > 60) curvatura_h --;
                    if (curvatura_h > 70) curvatura_h --;
                    if (curvatura_h > 80) curvatura_h --;
                    if (curvatura_h > 90) curvatura_h --;
                }
            }
            break;
		case 'd':
        case 'D':
            if (sentido_h == 1) {// -->
                if (curvatura_h > 8) { // curvo
                    curvatura_h--;
                    if (curvatura_h > 20) curvatura_h --;
                    if (curvatura_h > 30) curvatura_h --;
                    if (curvatura_h > 40) curvatura_h --;
                    if (curvatura_h > 50) curvatura_h --;
                    if (curvatura_h > 60) curvatura_h --;
                    if (curvatura_h > 70) curvatura_h --;
                    if (curvatura_h > 80) curvatura_h --;
                    if (curvatura_h > 90) curvatura_h --;
                }
            } else { // <--
                if (curvatura_h > 100) // recto
                    sentido_h = +1;
                else // curvo
                    curvatura_h++;
            }
            break;
            */
 		case 'a':
        case 'A':
            if (xx > -15)
                xx = xx - 0.08;
            break;
		case 'd':
        case 'D':
            if (xx < 15)
                xx = xx + 0.08;
            break;
 		case 'w':
        case 'W':
            if (yy > -15)
                yy = yy + 0.08;
            break;
		case 's':
        case 'S':
            if (yy < 15)
                yy = yy - 0.08;
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
void handle_special_keypress(int key, int x, int y) {
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
    }
}

/**
 * Manejo de clics del mouse
 */
void handle_mouse_button (int button, int state, int x, int y) {
	printf("BotÃ³n del mouse: %d\n",button);
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
void handle_mouse_motion (int x, int y) {
	_anglez = wrap_f(_anglez, 180 *((float)x - _x)/_height, 0, 360);
	_angley = wrap_f(_angley, 180 *((float)_y - y)/_width, 0, 360);
	_x = x;
	_y = y;
}

/**
 * Manejo de movimiento del mouse sin boton presionado
 */
void handle_mouse_motion_passive (int x, int y){
	_x = x;
	_y = y;
}

//void CreateBlankRenderTexture( Dword_ textureArray[], int size, int channels, int type, int textureID )										
void CreateBlankRenderTexture( GLuint *textureID, int size, int channels, int type )										
{
	// Create a pointer to store the blank image data
	GLuint *pTexture = NULL;											

	// Allocate and init memory for the image array and point to it from pTexture
	pTexture = new GLuint[size * size * channels];
	memset(pTexture, 0, size * size * channels * sizeof(GLuint));	

	// Register the texture with OpenGL and bind it to the texture ID
	//glGenTextures(1, &textureArray[textureID]);								
	glGenTextures(1, textureID);								
	glBindTexture(GL_TEXTURE_2D, *textureID);					
	
	// Create the texture and store it on the video card
	glTexImage2D(GL_TEXTURE_2D, 0, channels, size, size, 0, type, GL_UNSIGNED_INT, pTexture);						
//	glTexImage2D(GL_TEXTURE_2D, 0, channels, size, size, 0, type, GL_UNSIGNED_BYTE, pTexture);						
	
	// Set the texture quality
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	// Since we stored the texture space with OpenGL, we can delete the image data
	delete [] pTexture;																					
}

/**
 * Inicializar las luces
 */
void init_lights() {
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);

    //Add ambient light
	GLfloat ambientColor[] = {0.5f, 0.5f, 0.5f, 1.0f}; //Color(0.2, 0.2, 0.2)
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    //Add positioned light
    GLfloat diffuse[] = {0.20, 0.20, 0.20, 1.0};
    GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat lightPos[] = {0.0f, -5.5f, 2.5f, 0.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    //Add directed light
    GLfloat lightColor1[] = {0.6f, 0.7f, 0.8f, 1.0f}; //Color (0.5, 0.2, 0.2)
    //Coming from the direction (-1, 0.5, 0.5)
    GLfloat lightPos1[] = {-1.0f, 1.0f, 0.0f, 0.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    
    // add specular light
    GLfloat lightColor2[] = {0.3f, 0.3f, 0.3f, 1.0f}; //Color (0.5, 0.2, 0.2)
    //Coming from the direction (-1, 0.5, 0.5)
    GLfloat lightPos2[] = {3.0f, 3.0f, 3.0f, 0.0f};
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lightColor2);
    glLightfv(GL_LIGHT2, GL_POSITION, lightPos2);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);

}

/**
 * Inicializar rendereo
 */
void init_rendering() {
    glEnable(GL_DEPTH_TEST);

    //init_lights();

//    glClearColor(0.3, 0.35, 0.39, 0.9);
    glClearColor(0.0, 0.0, 0.0, 0.9);
    glPointSize(3.0);
    
    CreateBlankRenderTexture( &g_tex_0_ID, 512, 3, GL_RGB);
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
                    1.0, // near clipping
                    200.0 // far clipping
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

float _angle = 30.0; // 30
float _camera_angle = 0.0f; // 00

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
** f(x)  = x^3
** f'(x) = 3x^2
*/
static void dibujar_carretera()  {
    float z = 0, x, y = 0;
    float i, j, k;
    float b, c;
    int min = 0;
    int map_max = 20;
    int max = 35;
    int steps = 50;
    float ancho = 5;
    
    glDisable(GL_DEPTH_TEST);
    
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    for (i = 0 - delta; i < max; i = i + (max-min)/(float)steps ) {
        k = i / curvatura_h; // x
        j = (k*k)*sentido_h; // y
        
        b = i / curvatura_v; // x
        c = (b*b)*sentido_v/3; // z

        // hallar el x y z real
        if (k == 0)
            x = -ancho;
        else
            x = sin(atan(-1/(2*(k))*curvatura_h*sentido_h)) * ancho;

        if (k == 0)
            y = 0;
        else
            y = cos(atan(-1/(2*(k))*curvatura_h*sentido_h)) * ancho;

        if (sentido_h < 0) {
            glVertex3f(j-x,c,-i+y);        
            glVertex3f(j+x,c,-i-y);        
        } else {
            glVertex3f(j+x,c,-i-y);        
            glVertex3f(j-x,c,-i+y);        
        }
    }
    glEnd();

    // medio ROJO
    glBegin(GL_LINES);
    glColor3f(1.0,0,0);
    for (i = 0 - delta; i < max; i = i + (max-min)/(float)steps ) {
        k = i / curvatura_h; // x
        j = (k*k)*sentido_h; // y

        b = i / curvatura_v; // x
        c = (b*b)*sentido_v/3; // z

        glVertex3f(j,c,-i);        
    }
    glEnd();
    
    glEnable(GL_DEPTH_TEST);
}

void OrthoMode(int left, int top, int right, int bottom)
{
	// Switch to our projection matrix so that we can change it to ortho mode, not perspective.
	glMatrixMode(GL_PROJECTION);						

	// Push on a new matrix so that we can just pop it off to go back to perspective mode
	glPushMatrix();									
	
	// Reset the current matrix to our identify matrix
	glLoadIdentity();								

	//Pass in our 2D ortho screen coordinates like so (left, right, bottom, top).  The last
	// 2 parameters are the near and far planes.
	glOrtho( left, right, bottom, top, 0, 1 );	
	
	// Switch to model view so that we can render the scope image
	glMatrixMode(GL_MODELVIEW);								

	// Initialize the current model view matrix with the identity matrix
	glLoadIdentity();										
}

void PerspectiveMode()										// Set Up A Perspective View
{
	// Enter into our projection matrix mode
	glMatrixMode( GL_PROJECTION );							

	// Pop off the last matrix pushed on when in projection mode (Get rid of ortho mode)
	glPopMatrix();											

	// Go back to our model view matrix like normal
	glMatrixMode( GL_MODELVIEW );							

	// We should be in the normal 3D perspective mode now
}

void draw_text() {
    char buffer[1000];

	OrthoMode( 0, 0, _width, _height );
		
        glRasterPos2i(100, 200);
        glColor3f(1.0f, 1.0f, 0.0f);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *) "W A S D");
        // TIMING
        glColor3f(1.0f, 1.0f, 0.0f);
        glRasterPos2i(3, 14);
        sprintf(buffer, "[TIMING] FPS: %f Transcurrido: %dms", fps, glutGet(GLUT_ELAPSED_TIME));
        glutBitmapString(GLUT_BITMAP_9_BY_15, (unsigned char *) buffer);
        // DEBUG
        glColor3f(0.0f, 1.0f, 0.0f);
        glRasterPos2i(3, _height-4);
        sprintf(buffer, "[DEBUG] sentido_h: %d curvatura_h %f", sentido_h, curvatura_h);
        glutBitmapString(GLUT_BITMAP_9_BY_15, (unsigned char *) buffer);
        glRasterPos2i(0, 0);

    PerspectiveMode();
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

void blur_tex_zoom (GLuint tex, int passes)
{
    int i;
    int WIDTH = _width, HEIGHT = _height;
 
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture (GL_TEXTURE_2D, tex);
    while (passes > 0) {
        for (i = 0; i < 2; i++)
        {
            glPushMatrix ();
            glLoadIdentity ();
                if (i == 1)
                {
                    glTranslatef (WIDTH/2, HEIGHT/2,0);
                    glRotatef (1, 0, 0, 1);
                    glTranslatef (-WIDTH/2, -HEIGHT/2,0);
 
                }
                glColor4f (1.0f,1.0f,1.0f,1.0 / (i+1));
                glBegin (GL_TRIANGLE_STRIP);
                    glTexCoord2f (0.0f, 1.0f); glVertex2f (0, 0);
                    glTexCoord2f (0.0f, 0.0f); glVertex2f (0, HEIGHT);
                    glTexCoord2f (1.0f, 1.0f); glVertex2f (WIDTH, 0);
                    glTexCoord2f (1.0f, 0.0f); glVertex2f (WIDTH, HEIGHT);
                glEnd ();
            glPopMatrix ();
        }
        glCopyTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, WIDTH, HEIGHT, 0);
        passes--;
    }
    glDisable (GL_BLEND);
}

void RenderMotionBlur(GLuint textureID)
{
	// This function was created to blend the rendered texture over top of the screen
	// to create the recursive blur effect.  What happens is we start out by turning
	// off depth testing, setting our blending mode, then binding the texture of our 
	// rendered textured over the QUAD that is about to be created.  Next, we set our 
	// alpha level to %90 of full strength.  This makes it so it will slowly disappear.
	// Before rendering the QUAD, we want to go into ortho mode.  This makes it easier
	// to render a QUAD over the full screen without having to deal with vertices, but
	// 2D coordinates.  Once we render the QUAD, we want to go back to perspective mode.
	// We can then turn depth testing back on and turn off texturing.

	// Push on a new stack so that we do not interfere with the current matrix
	glPushMatrix();

		// Turn off depth testing so that we can blend over the screen
		glDisable(GL_DEPTH_TEST);			

		// Set our blend method and enable blending
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);	
//        glBlendFunc(GL_SRC_ALPHA,	GL_ONE_MINUS_SRC_ALPHA);
//        glBlendFunc(GL_ONE,GL_ONE);	

		//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);	
		glEnable(GL_BLEND);					

		// Bind the rendered texture to our QUAD
		//glBindTexture(GL_TEXTURE_2D, g_Texture[textureID]);			
		glBindTexture( GL_TEXTURE_2D, textureID );
		
		// Decrease the alpha value of the blend by %10 so it will fade nicely
		glColor4f(1, 1, 1, 0.88);
		//glColor4f(0, 0, 0, 0.9f);

		// Switch to 2D mode (Ortho mode)
		//OrthoMode(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		OrthoMode( 0, 0, _width, _height );

		// Display a 2D quad with our blended texture
		glBegin(GL_QUADS);

			// Display the top left point of the 2D image
			glTexCoord2f(0.0f, 1.0f);	glVertex2f(-blur_zoom, -blur_zoom);

			// Display the bottom left point of the 2D image
			//glTexCoord2f(0.0f, 0.0f);	glVertex2f(0, SCREEN_HEIGHT);
			glTexCoord2f(0.0f, 0.0f);	glVertex2f(-blur_zoom, _height+blur_zoom);

			// Display the bottom right point of the 2D image
			//glTexCoord2f(1.0f, 0.0f);	glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT);
			glTexCoord2f(1.0f, 0.0f);	glVertex2f(_width+blur_zoom, _height+blur_zoom);

			// Display the top right point of the 2D image
			//glTexCoord2f(1.0f, 1.0f);	glVertex2f(SCREEN_WIDTH, 0);
			glTexCoord2f(1.0f, 1.0f);	glVertex2f(_width+blur_zoom, -blur_zoom);

		// Stop drawing 
		glEnd();
		
		// Let's set our mode back to perspective 3D mode.
		PerspectiveMode();

		// Turn depth testing back on and texturing off.  If you do NOT do these 2 lines of 
		// code it produces a cool flash effect that might come in handy somewhere down the road.
		glEnable(GL_DEPTH_TEST);						
		glDisable(GL_BLEND);							

	// Go back to our original matrix
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
        dibujar_grid();
        dibujar_eje();
        glTranslatef(0.0, 0.0, 20.0);
        dibujar_carretera();
        //draw_text();
    glPopMatrix();
}

/**
 * Dibujar la escena
 */
void draw_scene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear all
    glLoadIdentity(); // reset drawing perspective
   
//    glMatrixMode(GL_MODELVIEW); // switch to drawing perspective

//    glRotatef(-_camera_angle, 0.1f, -.1f, 1.0f); // rotate camera
//    glTranslatef(0.0f, 0.0f, -5.0f); // moveeee

    gluLookAt(sin(_angley*3.14/180) * cos(_anglez*3.14/180) * _distance, cos(_angley*3.14/180) * _distance, sin(_angley*3.14/180) * sin(_anglez*3.14/180) * _distance,  // donde estoy
        0.0, 0.0, 0.0, // a donde miro
        0.0, (_angley >= 180 ? -1.0 : 1.0), 0.0 // mi arriba
    );
    

    if( AnimateNextFrame(60)) {
        glViewport(0, 0, g_Viewport, g_Viewport);	
        RenderMotionBlur( g_tex_0_ID );
        // draw
        do_draw(0);
   		glBindTexture(GL_TEXTURE_2D,g_tex_0_ID);
		// Render the current screen to our texture
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, g_Viewport, g_Viewport, 0);

		// Here we clear the screen and depth bits of the small viewport
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			

		// Set our viewport back to it's normal size
		glViewport(0, 0, _width, _height);	
	}
	RenderMotionBlur( g_tex_0_ID );
//    blur_tex_zoom(g_tex_0_ID, 1);
    do_draw(0);
    draw_text();
	glutSwapBuffers() ;
}

/**
 * Actualizaciones para el siguiente frame
 */
void update(int value) {
    static int _d1 = 1;
    static int _d2 = 1;
    static int _d3 = 1;

    /*_angle = wrap_f(_angle, 2.0f, 0.0, 360.0);*/
/*    _angley = wrap_f(_angley, 2.0f, 0.0, 360.0);
    _anglez = wrap_f(_anglez, 0.5f, 0.0, 360.0);*/
    _camera_angle = wrap_f(_camera_angle, 2.0f, 0.0, 360.0);
    delta = wrap_f(delta, 0.05, 0.0, 1.35);

    glutPostRedisplay(); // tell glut that something's changed

    glutTimerFunc(25, update, 0);
}

void handle_idle(void)
{
    static int timebase = 0;
    static int elapsed_time = 0;
    static int frame;
    int time;
    
	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);
	
	if (time - timebase > 1000) {
		fps = frame*1000.0/(time-timebase);
	 	timebase = time;		
		frame = 0;
	}
    
    delta = wrap_f(delta, (time - elapsed_time) * 0.05 / 25, 0.0, 1.35);
    
    elapsed_time = time;
    
    glutPostRedisplay();
}

/**
 * Main
 */
int main(int argc, char ** argv) {
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH) ;   
    glutInitWindowSize(800,600);

    glutCreateWindow("Test Carretera");
    init_rendering();

    glutDisplayFunc(draw_scene);
    glutKeyboardFunc(handle_keypress);
    glutSpecialFunc(handle_special_keypress);
    glutReshapeFunc(handle_resize);
    glutMouseFunc(handle_mouse_button);
    glutMotionFunc(handle_mouse_motion);
    glutPassiveMotionFunc(handle_mouse_motion_passive);
    glutIdleFunc(handle_idle) ;

    //glutTimerFunc(25, update, 0); // add timer

    glutMainLoop();
    return 0;
}
