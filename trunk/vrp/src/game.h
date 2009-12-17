#define ROAD_MIN 0
#define ROAD_MAX 145
#define ROAD_STEPS 35
#define ROAD_WIDTH 5

extern float _speed;
extern GLuint _green_texture;
extern GLuint _life_texture;
extern GLuint _slow_texture;
extern GLuint _shield_texture;
extern GLuint _random_texture;
extern int _show_bounds;

void game_draw_scene(void);
void game_handle_keypress(unsigned char key, int x, int y);
void game_handle_keypress_special (int key, int x, int y, int state);
void game_init();
void game_handle_idle();

void give_power_up(int instance);
float calcular_alpha(float z);
void calcular_coordenadas(struct Punto3D entrada, struct Punto3D * salida);
void calcular_rotacion(struct Punto3D entrada, float * rot_x, float * rot_y);
void on_collision(void);
void dibujar_bounding_box(Punto3D min, Punto3D max);
