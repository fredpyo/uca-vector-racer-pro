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

int check_collisions(Punto3D car_bounds[2], game_entity * entity){
    struct Punto3D e_min, e_max;
    int collision;

    e_min = entity->pos;
    e_min.x += entity->bound_min.x;
    e_min.y += entity->bound_min.y;
    e_min.z += entity->bound_min.z;
    calcular_coordenadas(e_min, &e_min);

    e_max = entity->pos;
    e_max.x += entity->bound_max.x;
    e_max.y += entity->bound_max.y;
    e_max.z += entity->bound_max.z;
    calcular_coordenadas(e_max, &e_max);
    
    if (
        !(car_bounds[0].x > e_max.x) && car_bounds[1].x > e_min.x &&
        !(car_bounds[0].z > e_max.z) && car_bounds[1].z > e_min.z
       ) {
        collision = 1;
    } else {
        collision = 0;
    }
    
//    sprintf(_debug_string, "e_min:%f,%f,%f e_max:%f,%f,%f COL: %d", e_min.x, e_min.y, e_min.z, e_max.x, e_max.x, e_max.x, collision);
    
    return collision;
   
}

void recorrer_lista(struct game_entity * entity, Punto3D car_pos[3]) {
    static int elapsed_time = 0;
    int time;
	time = glutGet(GLUT_ELAPSED_TIME);

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (entity->next) {
        entity->next->renderer(entity->next, time - elapsed_time);
        if (check_collisions(&car_pos[1], entity->next)) {
            if (entity->next->type == GAME_ENTITY_TYPE_POWERUP) {
                give_power_up(entity->next->instance);
            } else {
                on_collision();
            }
            entity->next = borrar_de_lista(entity->next);
        }
        if (entity->next != NULL) {
                if (entity->next->pos.z > entity->next->dissapear_at) {
                    entity->next = borrar_de_lista(entity->next);
                }
        }
        if (entity->next != NULL) {
            entity = entity->next;
        }
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


/**
 * Elige una nueva entidad de manera probabilística
 */
int pick_game_entity_powerup_instance() {
    float probability = rand()/(float)RAND_MAX;
    
    sprintf(_debug_string, "%f", probability);
    
    if (probability > .70)
        return GAME_ENTITY_INSTANCE_POWERUP_COIN;// GAME_ENTITY_INSTANCE_POWERUP_LIFE;
    if (probability > .60)
        return GAME_ENTITY_INSTANCE_POWERUP_LIFE;
    if (probability > .45)
        return GAME_ENTITY_INSTANCE_POWERUP_SLOW;
    if (probability > .05)
        return GAME_ENTITY_INSTANCE_POWERUP_RANDOM;
    return GAME_ENTITY_INSTANCE_POWERUP_INVINCIBLE;
}

/**
 * Elige una nueva entidad de manera probabilística
 */
int pick_game_entity_obstacle_instance() {
    float probability = rand()/(float)RAND_MAX;
    if (probability > .85)
        return GAME_ENTITY_INSTANCE_OBSTACLE_CIL;
    else if (probability > .60)
        return GAME_ENTITY_INSTANCE_OBSTACLE_CUBE;
    else
        return GAME_ENTITY_INSTANCE_OBSTACLE_PYR;
}

/**
 * Setea los bounds de la entidad de acuerdo a su tipo
 */
void set_game_entity_bounds(struct game_entity * game_entity) {
	switch(game_entity->instance) {
		case GAME_ENTITY_INSTANCE_OBSTACLE_PYR:
			game_entity->bound_min.x = - 0.65/2;
			game_entity->bound_min.y =   0;
			game_entity->bound_min.z = - 0.65/2;
			game_entity->bound_max.x = + 0.65/2;
			game_entity->bound_max.y = + 1;
			game_entity->bound_max.z = + 0.65/2;
			break;
		case GAME_ENTITY_INSTANCE_OBSTACLE_CUBE:
			game_entity->bound_min.x = - 1;
			game_entity->bound_min.y =   0;
			game_entity->bound_min.z = - 1;
			game_entity->bound_max.x = + 1;
			game_entity->bound_max.y = + 2;
			game_entity->bound_max.z = + 1;
            game_entity->pos.y = 0;
			break;
		case GAME_ENTITY_INSTANCE_OBSTACLE_CIL:
			game_entity->bound_min.x = - 0.2/2;
			game_entity->bound_min.y =   0;
			game_entity->bound_min.z = - 0.2/2;
			game_entity->bound_max.x = + 0.2/2;
			game_entity->bound_max.y = + 3.2;
			game_entity->bound_max.z = + 0.2/2;
			break;
		case GAME_ENTITY_INSTANCE_POWERUP_LIFE:
        case GAME_ENTITY_INSTANCE_POWERUP_SLOW:
        case GAME_ENTITY_INSTANCE_POWERUP_INVINCIBLE:
        case GAME_ENTITY_INSTANCE_POWERUP_RANDOM:
			game_entity->bound_min.x = - 0.45;
			game_entity->bound_min.y = + 0.00;
			game_entity->bound_min.z = - 0.45;
			game_entity->bound_max.x = + 0.45;
			game_entity->bound_max.y = + 0.90;
			game_entity->bound_max.z = + 0.45;
            break;
        case GAME_ENTITY_INSTANCE_POWERUP_COIN:
			game_entity->bound_min.x = - 0.30;
			game_entity->bound_min.y = + 0.00;
			game_entity->bound_min.z = - 0.30;
			game_entity->bound_max.x = + 0.30;
			game_entity->bound_max.y = + 0.60;
			game_entity->bound_max.z = + 0.30;
            break;
	}
}

/**
 * Crear una nueva entidad
 */
struct game_entity *  create_entity() {
    struct game_entity * nuevo = NULL;
    float probability = rand()/(float)RAND_MAX;

    nuevo = (struct game_entity *)malloc(sizeof(struct game_entity));    
    if (probability > 0.90) {
        nuevo->type = GAME_ENTITY_TYPE_POWERUP;
        nuevo->instance = pick_game_entity_powerup_instance(); //GAME_ENTITY_INSTANCE_OBSTACLE_CIL;
        if (nuevo->instance != GAME_ENTITY_INSTANCE_POWERUP_COIN)
            nuevo->renderer = game_entity_render_powerup;
        else
            nuevo->renderer = game_entity_render_coin;
    } else {
        nuevo->type = GAME_ENTITY_TYPE_OBSTACLE;
        nuevo->instance = pick_game_entity_obstacle_instance(); //GAME_ENTITY_INSTANCE_OBSTACLE_CIL;
        nuevo->renderer = game_entity_render_obstacle;
    }

    nuevo->pos.x = (rand()/(float)RAND_MAX)* (ROAD_WIDTH-0.3)*2 - (ROAD_WIDTH-0.3);
    nuevo->pos.y = 0;
    nuevo->pos.z = -ROAD_MAX;
    nuevo->dissapear_at = 0;
    nuevo->next = NULL;
	// setear los bounds
	set_game_entity_bounds(nuevo);
    return nuevo;
}
