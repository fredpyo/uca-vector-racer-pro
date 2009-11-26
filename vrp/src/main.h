#ifndef MAIN_INCLUDED
#define MAIN_INCLUDED
extern int _height; // altura de la ventana
extern int _width; // ancho de la ventana

extern char _debug_string[512]; // buffer para mensajes de debug
extern char _message_string[512]; // buffer para mensajes

void ortho_mode(int left, int top, int right, int bottom);
void perspective_mode();
int switch_to(int scene);
#include "scene_switcher.h"
#endif
