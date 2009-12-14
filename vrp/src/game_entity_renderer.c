/*
**
**
*/

#include <stdio.h>
#include <gl/freeglut.h>
#include "main.h"
#include "game_entities.h"

void game_entity_render_obstacle(struct game_entity * entity) {

    sprintf(_message_string, "Z:%f --> 0", entity->pos.z);

    glPushMatrix();
    glTranslatef(entity->pos.x, entity->pos.y, entity->pos.z); 
    glutSolidCone(0.25, 0.5, 16, 2);
    glPopMatrix();

}
