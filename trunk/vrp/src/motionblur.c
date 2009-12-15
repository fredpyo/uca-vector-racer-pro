/*
** Motion Blur
** Sacado de acá:
** http://www.codeproject.com/KB/openGL/MotionBlur.aspx
*/
#include <gl/freeglut.h>
#include "main.h"
#include "imageloader.h"
#include "texture.h"

#define MOTION_BLUR_WIDTH 512
#define MOTION_BLUR_HEIGHT 512

GLuint _motion_blur_texture;

int CreateMotionBlurTexture() {	// Creates the motion blur texture
/*	switch(quality) {
		case Texture_HiQuality:
			motionBlur_Texture_X = 512;
			motionBlur_Texture_Y = 512;
			
			motionBlur_Texture_channels = 3;
			motionBlur_Texture_type = GL_RGB;
			break;
		case Texture_MeQuality:
			motionBlur_Texture_X = 256;
			motionBlur_Texture_Y = 256;
			
			motionBlur_Texture_channels = 3;
			motionBlur_Texture_type = GL_RGB;
			break;
		case Texture_LoQuality:
			motionBlur_Texture_X = 128;
			motionBlur_Texture_Y = 128;
			
			motionBlur_Texture_channels = 3;
			motionBlur_Texture_type = GL_RGB;
			break;
	}

	switch(interpolation) {
		case Texture_Interpolation :
			filter = GL_LINEAR;
			break;
		case Texture_Near:
			filter = GL_NEAREST;
			break;
	}*/

	return GenerateTexture(_motion_blur_texture, 
		MOTION_BLUR_WIDTH, 
		MOTION_BLUR_HEIGHT,			
		3, 
		GL_RGB, 
		GL_LINEAR, GL_LINEAR);
}

void RenderToMotionBlurTexture(int FirstRenderTexture, int SceneFunc()) {

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();

	glViewport(0, 0, MOTION_BLUR_WIDTH, MOTION_BLUR_HEIGHT);

	// do we render first the scene or not?
	if(FirstRenderTexture)
		SceneFunc();

	// render the old texture
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		
		glDisable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, 
			GL_ONE_MINUS_SRC_ALPHA);
			// For experimentation you can use these values too ;)
			//GL_DST_ALPHA);
			//GL_ONE_MINUS_SRC_COLOR );

		// Set the textures customizable color
		glColor4f(1,
				  1,
				  1,
				  0.9);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

			// put the texture on the screen
			glBindTexture(GL_TEXTURE_2D, _motion_blur_texture);

			// go to orthogonal view
			glLoadIdentity();
			glOrtho( 0, MOTION_BLUR_WIDTH, MOTION_BLUR_HEIGHT, 0, 0, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glBegin(GL_QUADS);
				
				glTexCoord2f(0.0, 1.0);  glVertex2f(0, 0);
				glTexCoord2f(0.0, 0.0);  glVertex2f(0, (GLfloat)MOTION_BLUR_HEIGHT);
				glTexCoord2f(1.0, 0.0);  glVertex2f((GLfloat)MOTION_BLUR_WIDTH, (GLfloat)MOTION_BLUR_HEIGHT);
				glTexCoord2f(1.0, 1.0);  glVertex2f((GLfloat)MOTION_BLUR_WIDTH, 0);

			glEnd();

			glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

	glPopMatrix();

	// do we put first the scene or not?
	if(!FirstRenderTexture)
		SceneFunc();

	
	// copy the whole viewport to the teture
	glBindTexture(GL_TEXTURE_2D, _motion_blur_texture);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 
		MOTION_BLUR_WIDTH, MOTION_BLUR_HEIGHT, 0);

	glViewport(0, 0, _width, _height);

	// clear the viewport
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();
}

void ShowMotionBlurTexture() {
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();

	// put the texture on the screen
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
			// put the result onto the screen
			glBindTexture(GL_TEXTURE_2D, _motion_blur_texture);

			// go to orthogonal view
			glLoadIdentity();
			glOrtho(0, _width, _height, 0, 0, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glBegin(GL_QUADS);
				glColor3f(1.0, 1.0, 1.0);

				glTexCoord2f(0.0, 1.0);  glVertex2f(0, 0);
				glTexCoord2f(0.0, 0.0);  glVertex2f(0, (GLfloat)_height);
				glTexCoord2f(1.0, 0.0);  glVertex2f((GLfloat)_width, (GLfloat)_height);
				glTexCoord2f(1.0, 1.0);  glVertex2f((GLfloat)_width, 0);

			glEnd();

			glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
