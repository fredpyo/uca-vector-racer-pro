#ifndef MAIN
#include "scene_switcher.h"

extern int _height; // altura de la ventana
extern int _width; // ancho de la ventana

/*extern char debug_string[512] = {0}; // buffer para mensajes de debug
extern char message_string[512] = {0}; // buffer para mensajes
*/
void ortho_mode(int left, int top, int right, int bottom);
void perspective_mode();
int switch_to(int scene);
#define MAIN
#endif
