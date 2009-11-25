/*
** sfx.c
**
*/ 

#include "fmod.h"

#define SFX_FADEIN 0;
#define SFX_FADEOUT 1;

FSOUND_STREAM * stream_handle;

char * songs[] = {
    "music\\test.aac",
};

int current = 2;

void sfx_init() {
    FSOUND_Init (44100, 32, 0);
}

/**
 * Obtener el path del archivo filename, básicamente hace un strcat entre
 * "music\\" y filename
 * a mano
 */
char * get_music_file_path (char * filename) {
    static char path[256] = "music\\";
    int i;
    for (i = 6; filename[i-6]; i++)
        path[i] = filename[i-6];
    return path;
}

/**
 * Abre filename para reproducir
 */
void music_play(char * filename) {
    stream_handle = FSOUND_Stream_Open(get_music_file_path(filename), FSOUND_2D, 0, 0);
    FSOUND_Stream_Play(0, stream_handle);
}

/**
 * Detiene la musica
 */
void music_stop(int fade) {
    FSOUND_Stream_Stop(stream_handle);
}

void music_crossfade(char * filename, int delay) {
    // hahaha, yeah right!
}
