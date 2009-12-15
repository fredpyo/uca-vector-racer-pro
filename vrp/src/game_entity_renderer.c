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

    entity->pos.z += _speed * elapsed_time;
    calcular_coordenadas(entity->pos, &a);

    glPushMatrix();
    glTranslatef(a.x, a.y, a.z); 

    switch (entity->instance) {
        case GAME_ENTITY_INSTANCE_OBSTACLE_PYR:
            glRotatef(-90, 1,0,0);
            glRotatef(45, 0,0,1);
            glColor4f(1.0, 0.0, 0.0, calcular_alpha(a.z));
            glutWireCone(0.65, 1.0, 4, 1);
            break;
        case GAME_ENTITY_INSTANCE_OBSTACLE_CUBE:
            glTranslatef(0.0, 0.5, 0.0);
            glColor4f(0.0, 1.0, 0.0, calcular_alpha(a.z));
            glutSolidCube(1.4);
            break;
        case GAME_ENTITY_INSTANCE_OBSTACLE_CIL:
            glRotatef(-90, 1,0,0);
            glTranslatef(0.0, 0.5, 0.0);
            glColor4f(1.0, 1.0, 0.0, calcular_alpha(a.z));
            glutSolidCylinder(0.3, 3.5, 8, 2);
            break;
    }
    glPopMatrix();

}
