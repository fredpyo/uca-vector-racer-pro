/*
** sfx.c
**
*/ 

#include "fmod.h"

#define SFX_FADEIN 0;
#define SFX_FADEOUT 1;

FMUSIC_MODULE * fh;
FSOUND_STREAM * stream_handle;

char * songs[] = {
    "music\\Alliance.xm",
    "music\\beyond_music.mod",
    "music\\celesfnt.s3m",
    "music\\DEADLOCK.XM",
    "music\\jt_mind.xm",
    "music\\Metal.mod",
    "music\\myst2.s3m",
    "music\\myst3.s3m",
    "music\\oriental.s3m",
    "music\\satell.s3m",
    "music\\Shadowrun.mod",
    "music\\traveller.mod",
    "music\\Unreal - DigSh.s3m",
    "music\\Unreal - K_vision.s3m",
    "music\\znm-believe.it"
};

int current = 2;

void sfx_init() {
    FSOUND_Init (44100, 32, 0);
}

void sfx_play(char * filename) {
    stream_handle = FSOUND_Stream_Open(filename, FSOUND_2D, 0, 0);
    FSOUND_Stream_Play(0, stream_handle);
//    FMUSIC_PlaySong(fh);
}

void music_start() {
    fh = FMUSIC_LoadSong(songs[current]);
    FMUSIC_PlaySong(fh);
}

void music_stop() {
    FMUSIC_FreeSong(fh);
}

void music_next() {
    music_stop();
    current = (current + 1)%15;
    music_start();
}

void music_prev() {
    music_stop();
    current = (current + 14)%15;
    music_start();
}

char * music_current_song_name() {
    return songs[current];
}

