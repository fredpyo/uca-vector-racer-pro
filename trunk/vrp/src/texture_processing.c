/*
 * texture_processing.c
 * @description: Operaciones especiales sobre las texturas
 */

#include <gl/glut.h>
#include "main.h"

float blur_zoom = 0;

void render_motion_blur(GLuint textureID)
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
		ortho_mode( 0, 0, _width, _height );

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
		perspective_mode();

		// Turn depth testing back on and texturing off.  If you do NOT do these 2 lines of 
		// code it produces a cool flash effect that might come in handy somewhere down the road.
		glEnable(GL_DEPTH_TEST);						
		glDisable(GL_BLEND);							

	// Go back to our original matrix
	glPopMatrix();
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
