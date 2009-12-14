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
    glColor4f(1.0, 0.0, 0.0, calcular_alpha(a.z));
    glTranslatef(a.x, a.y, a.z); 
    glutSolidCone(0.25, 0.5, 16, 2);
    glPopMatrix();

}
