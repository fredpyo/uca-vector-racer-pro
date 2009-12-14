#define ROAD_MIN 0
#define ROAD_MAX 145
#define ROAD_STEPS 35
#define ROAD_WIDTH 5

extern float _speed;

void game_draw_scene(void);
void game_handle_keypress(unsigned char key, int x, int y);
void game_handle_keypress_special (int key, int x, int y);
void game_init();
void game_handle_idle();

float calcular_alpha(float z);
void calcular_coordenadas(struct Punto3D entrada, struct Punto3D * salida);

