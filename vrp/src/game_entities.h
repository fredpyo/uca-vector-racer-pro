#define GAME_ENTITY_TYPE_OBSTACLE 0
#define GAME_ENTITY_TYPE_POWERUP 1
#define GAME_ENTITY_TYPE_PROP 2

#define GAME_ENTITY_INSTANCE_1UP 0
#define GAME_ENTITY_INSTANCE_SLOWMO 1
#define GAME_ENTITY_INSTANCE_OBSTACLE_PYR 2
#define GAME_ENTITY_INSTANCE_OBSTACLE_CUBE 3
#define GAME_ENTITY_INSTANCE_OBSTACLE_CIL 4

struct Punto3D {
    float x;
    float y;
    float z;
};

struct game_entity {
    int type;
    int instance;
    struct Punto3D pos;
    float dissapear_at;
    struct Punto3D bound_min;
    struct Punto3D bound_max;
    void (*renderer)(struct game_entity * game_entity, int elapsed_time);
    struct game_entity * next;
};


void recorrer_lista(struct game_entity * entity);
void agregar_a_lista(struct game_entity * entity, struct game_entity * list);
struct game_entity * borrar_de_lista(struct game_entity * entity);
struct game_entity * create_entity();

