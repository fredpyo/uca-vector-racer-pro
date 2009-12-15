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
            glutSolidCylinder(0.2, 3.2, 8, 1);
            break;
    }
    glPopMatrix();

}
