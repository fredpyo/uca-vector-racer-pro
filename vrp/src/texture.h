void texture_create_blank_render_texture( GLuint *textureID, int size, int channels, int type );
GLuint texture_load_texture(Image* image);
int GenerateTexture(GLuint &, int, int, int, 
					int, int, int);				// Generates a teture with the givven specifications
int GenerateTexture(GLuint &, int, int, int, 
					int, int, int, unsigned int *);		// Generates a texture with the following image
