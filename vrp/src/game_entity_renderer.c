/*
**
**
*/

#include <stdio.h>
#include <gl/freeglut.h>
#include "main.h"
#include "game.h"
#include "game_entities.h"


void game_entity_render_obstacle(struct game_entity * entity, int elapsed_time) {
    static Punto3D a;
    float rot_x, rot_y;
    
    entity->pos.z += _speed * elapsed_time;
    calcular_coordenadas(entity->pos, &a);
    calcular_rotacion(entity->pos, &rot_x, &rot_y);

    glPushMatrix();
    glTranslatef(a.x, a.y, a.z); 

    glRotatef(rot_y, 0, 1, 0);
    glRotatef(rot_x, 1, 0, 0);

    switch (entity->instance) {
        case GAME_ENTITY_INSTANCE_OBSTACLE_PYR:
            glDisable(GL_TEXTURE_2D);
            glRotatef(-90, 1,0,0);
            glRotatef(45, 0,0,1);
            glColor4f(1.0, 0.0, 0.0, calcular_alpha(a.z));
            glutWireCone(0.65, 1.0, 4, 1);
            break;
        case GAME_ENTITY_INSTANCE_OBSTACLE_CUBE:
            glTranslatef(0.0, 0.5, 0.0);

        	glEnable(GL_TEXTURE_2D);
        	glBindTexture(GL_TEXTURE_2D, _green_texture);
/*        	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/
            glColor4f(1.0, 1.0, 1.0, calcular_alpha(a.z));
//            glColor3f(1.0, 1.0, 1.0);            
//            glutSolidCube(1.4);
            glBegin(GL_QUADS);
            // front
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
            // right
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
            // left
			glTexCoord2f(0.0f, 0.0f); glVertex3f( -1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
			glTexCoord2f(1.0f, 0.0f); glVertex3f( -1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(1.0f, 1.0f); glVertex3f( -1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2f(0.0f, 1.0f); glVertex3f( -1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
			// top
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad

			glEnd();

            break;
        case GAME_ENTITY_INSTANCE_OBSTACLE_CIL:
            glDisable(GL_TEXTURE_2D);
            glRotatef(-90, 1,0,0);
            glTranslatef(0.0, 0.5, 0.0);
            glColor4f(1.0, 1.0, 0.0, calcular_alpha(a.z));
            glutSolidCylinder(0.2, 3.2, 8, 1);
            break;
    }
    glPopMatrix();

}
