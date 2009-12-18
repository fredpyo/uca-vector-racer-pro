#define GAME_ENTITY_TYPE_OBSTACLE 0
#define GAME_ENTITY_TYPE_POWERUP 1
#define GAME_ENTITY_TYPE_PROP 2

#define GAME_ENTITY_INSTANCE_1UP 0
#define GAME_ENTITY_INSTANCE_SLOWMO 1
#define GAME_ENTITY_INSTANCE_OBSTACLE_PYR 2
#define GAME_ENTITY_INSTANCE_OBSTACLE_CUBE 3
#define GAME_ENTITY_INSTANCE_OBSTACLE_CIL 4

#define GAME_ENTITY_INSTANCE_POWERUP_LIFE 5
#define GAME_ENTITY_INSTANCE_POWERUP_SLOW 6
#define GAME_ENTITY_INSTANCE_POWERUP_INVINCIBLE 7
#define GAME_ENTITY_INSTANCE_POWERUP_LIGHTSOFF 8
#define GAME_ENTITY_INSTANCE_POWERUP_COIN 9
#define GAME_ENTITY_INSTANCE_POWERUP_SPEED 10
#define GAME_ENTITY_INSTANCE_POWERUP_RANDOM 20

struct Punto3D {
    float x;
    float y;
    float z;
};

// representa una entidad del juego
struct game_entity {
    int type; // el tipo, si es obstaculo, powerup, etc --> GAME_ENTITY_TYPE_*
    int instance; // el objeto en si instanciado en esta entidad (cubo, piramide, vida, slow) --> --> GAME_ENTITY_ISNTANCE_*
    struct Punto3D pos; // posición con respecto a una carretera recta
    float dissapear_at; // desaparecer luego de pasar este umbral
    struct Punto3D bound_min; // bounding box minimum
    struct Punto3D bound_max; // bouding box maximum
    void (*renderer)(struct game_entity * game_entity, int elapsed_time); // funcion que se encarga de renderear
    struct game_entity * next; // siguiente elemento de la lista
};


void recorrer_lista(struct game_entity * entity);
void agregar_a_lista(struct game_entity * entity, struct game_entity * list);
struct game_entity * borrar_de_lista(struct game_entity * entity);
struct game_entity * create_entity();

