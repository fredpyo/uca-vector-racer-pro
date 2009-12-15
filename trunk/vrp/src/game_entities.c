/*
** Game Entities
** Entidades que existen el el juego
*/
#include <stdio.h>
#include <stdlib.h>
#include <gl/freeglut.h>
#include "main.h"
#include "game.h"
#include "game_entities.h"
#include "game_entity_renderer.h"

void recorrer_lista(struct game_entity * entity) {
    static int elapsed_time = 0;
    int time;
	time = glutGet(GLUT_ELAPSED_TIME);

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (entity) {
        entity->renderer(entity, time - elapsed_time);
        if (entity->next != NULL) {
                sprintf(_message_string, "%f", entity->next->pos.z);
            if (entity->next->pos.z > entity->next->dissapear_at) {

                entity->next = borrar_de_lista(entity->next);
            }
        }
        entity = entity->next;
        
    }
    glDisable(GL_BLEND);
    elapsed_time = time;
}

/**
 * Agregar al comienzo de la lista
 */
void agregar_a_lista(struct game_entity * list, struct game_entity * entity) {
    entity->next = list->next; // la nueva entidad apuntará al primer elemento de la lista
    list->next = entity; // la nueva entidad ahora es el nuevo elemento de la lista
}

/**
 * Borrar y retornar el elemento que está después del eliminado
 */
struct game_entity *  borrar_de_lista(struct game_entity * entity) {
    struct game_entity * next;
    next = entity->next; // almacenar el siguiente en la lista
    free(entity);
    return next; // retornar el siguiente para que la función anterior encadene
}

int game_entity_obstacle_instance() {
    float probability = rand()/(float)RAND_MAX;
    
    if (probability > .85)
        return GAME_ENTITY_INSTANCE_OBSTACLE_CIL;
    else if (probability > .60)
        return GAME_ENTITY_INSTANCE_OBSTACLE_CUBE;
    else
        return GAME_ENTITY_INSTANCE_OBSTACLE_PYR;
}

/**
 * Crear una nueva entidad
 */
struct game_entity *  create_entity() {
    
    struct game_entity * nuevo = NULL;
    
    nuevo = (struct game_entity *)malloc(sizeof(struct game_entity));
    
    nuevo->type = GAME_ENTITY_TYPE_OBSTACLE;
    nuevo->instance = game_entity_obstacle_instance(); //GAME_ENTITY_INSTANCE_OBSTACLE_CIL;
    nuevo->pos.x = (rand()/(float)RAND_MAX)* (ROAD_WIDTH-0.3)*2 - (ROAD_WIDTH-0.3);
    nuevo->pos.y = 1;
    nuevo->pos.z = -ROAD_MAX;
    nuevo->dissapear_at = 0;
    nuevo->renderer = game_entity_render_obstacle;
    nuevo->next = NULL;

    return nuevo;
}
