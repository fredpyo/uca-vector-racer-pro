/*
 * texture.cpp
 * @description: Cosas varias del manejo de texturas
 */
#include <gl/freeglut.h>
#include "imageloader.h"
#include "texture.h"

/**
 * Makes the image into a texture, and returns the id of the texture
 */
GLuint texture_load_texture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
				 0,                            //0 for now
				 GL_RGB,                       //Format OpenGL uses for image
				 image->width, image->height,  //Width and height
				 0,                            //The border of the image
				 GL_RGB, //GL_RGB, because pixels are stored in RGB format
				 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
				                   //as unsigned numbers
				 image->pixels);               //The actual pixel data

//    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image->width, image->height, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering

	return textureId; //Returns the id of the texture
}

//void CreateBlankRenderTexture( Dword_ textureArray[], int size, int channels, int type, int textureID )										
void texture_create_blank_render_texture( GLuint *textureID, int size, int channels, int type )										
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

/*
 * Generar una textura en blanco
 */
int GenerateTexture(GLuint &Texture, int SizeX, int SizeY, int Channels, int Format, int Min_Filter, int Mag_Filter) {
	int Status = FALSE;
	unsigned int memoryRequiredSize = SizeX * SizeY * Channels;

	unsigned int *memoryBlock = new unsigned int[memoryRequiredSize];
	if(memoryBlock == NULL)
		return Status;

	ZeroMemory(memoryBlock, memoryRequiredSize);

	Status = GenerateTexture(Texture, SizeX, SizeY, Channels, Format, Min_Filter, Mag_Filter, memoryBlock);
	delete [] memoryBlock;

	int ErrorStatus = glGetError();
	if(ErrorStatus == GL_NO_ERROR)
		Status = TRUE;

	return Status;
}

int GenerateTexture(GLuint &Texture, int SizeX, int SizeY, int Channels, int Format, int Min_Filter, int Mag_Filter, unsigned int *memoryArea) {
	int Status = FALSE;

	if(memoryArea == NULL)
		return Status;

	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);

	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

	//gluBuild2DMipmaps(GL_TEXTURE_2D, Channels, SizeX, SizeY, Format, GL_UNSIGNED_INT, memoryBlock);
	glTexImage2D(GL_TEXTURE_2D, 0, Channels, SizeX, SizeY, 0, Format, GL_UNSIGNED_INT, memoryArea );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Min_Filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Mag_Filter); 


	int ErrorStatus = glGetError();
	if(ErrorStatus == GL_NO_ERROR)
		Status = TRUE;

	return Status;
}
