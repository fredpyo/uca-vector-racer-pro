/*
** sfx.c
**
*/ 

#include "fmod.h"
#include "sfx.h"

#define SFX_FADEIN 0;
#define SFX_FADEOUT 1;

FSOUND_STREAM * stream_handle;
FSOUND_STREAM * stream2_handle;
FSOUND_SAMPLE * sample_boom1;
FSOUND_SAMPLE * sample_boom2;
FSOUND_SAMPLE * sample_power_up;
FSOUND_SAMPLE * sample_power_up_good;
FSOUND_SAMPLE * sample_power_up_bad;

char * songs[] = {
    "music\\test.aac",
};

int current = 2;

void sfx_init() {
    FSOUND_Init (44100, 32, 0);
    
    sample_boom1 = FSOUND_Sample_Load(FSOUND_FREE, "sound\\sound_explosion.wav", 0, 0, 0);
    sample_boom2 = FSOUND_Sample_Load(FSOUND_FREE, "sound\\sound_huge_explosion.wav", 0, 0, 0);
    sample_power_up = FSOUND_Sample_Load(FSOUND_FREE, "sound\\sound_mouseover.wav", 0, 0, 0);
    sample_power_up_good = FSOUND_Sample_Load(FSOUND_FREE, "sound\\sound_powerup.wav", 0, 0, 0);
    sample_power_up_bad = FSOUND_Sample_Load(FSOUND_FREE, "sound\\sound_mouseclick.wav", 0, 0, 0);
}

void sfx_play_sample(int sample) {
    switch (sample) {
        case SFX_BOOM1:
            FSOUND_PlaySound(FSOUND_FREE, sample_boom1);
            break;
        case SFX_BOOM2:
            FSOUND_PlaySound(FSOUND_FREE, sample_boom1);
            break;
        case SFX_POWERUP:
            FSOUND_PlaySound(FSOUND_FREE, sample_power_up);
            break;
        case SFX_POWERUP_GOOD:
            FSOUND_PlaySound(FSOUND_FREE, sample_power_up_good);
            break;
        case SFX_POWERUP_BAD:
            FSOUND_PlaySound(FSOUND_FREE, sample_power_up_bad);
            break;

    }

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
    path[i] = 0;
    return path;
}

void music_invicible_play() {
    
}

/**
 * Abre filename para reproducir
 */
void music_play(char * filename) {
    stream_handle = FSOUND_Stream_Open(get_music_file_path(filename), 0, 0, 0);
    FSOUND_Stream_Play(FSOUND_FREE, stream_handle);
}

/**
 * Detiene la musica
 */
void music_stop(int fade) {
    FSOUND_Stream_Stop(stream_handle);
    FSOUND_Stream_Close(stream_handle);
}

void music_crossfade(char * filename, int delay) {
    // hahaha, yeah right!
}
