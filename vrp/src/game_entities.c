/*
** Game Entities
** Entidades que existen el el juego
*/

#include <stdlib.h>
#include "game_entities.h"
#include "game_entity_renderer.h"

void recorrer_lista(struct game_entity * entity) {
    while (entity) {
        entity->renderer(entity);
        if (entity == entity->next) {
            if (entity->next->pos.z > entity->next->dissapear_at)
                entity->next = borrar_de_lista(entity->next);
        }
        entity = entity->next;
        
    }
}

/**
 * Agregar al comienzo de la lista
 */
void agregar_a_lista(struct game_entity * entity, struct game_entity * list) {
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

/**
 * Crear una nueva entidad
 */
struct game_entity *  create_entity() {
    struct game_entity * nuevo = NULL;
    
    nuevo = (struct game_entity *)malloc(sizeof(struct game_entity));
    
    nuevo->type = GAME_ENTITY_TYPE_OBSTACLE;
    nuevo->instance = GAME_ENTITY_INSTANCE_OBSTACLE1;
    nuevo->pos.x = 0;
    nuevo->pos.y = 1;
    nuevo->pos.z = -10;
    nuevo->dissapear_at = 0;
    nuevo->renderer = game_entity_render_obstacle;
    nuevo->next = NULL;

    return nuevo;
}
