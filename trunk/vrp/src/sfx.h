/*
** SFX.c PUBLIC
**
*/
#define SFX_BOOM1 0
#define SFX_BOOM2 1
#define SFX_POWERUP 3
#define SFX_POWERUP_GOOD 4
#define SFX_POWERUP_BAD 5

#define SFX_SONG_INTRO 0
#define SFX_SONG_TITLE 1
#define SFX_SONG_RANKING 2
#define SFX_SONG_RANKING_WIN 3
#define SFX_SONG_GAME_OVER 4
#define SFX_SONG_GAME1 5
#define SFX_SONG_GAME2 6
#define SFX_SONG_GAME3 7
#define SFX_SONG_GAME4 8
#define SFX_SONG_GAME5 9


void sfx_init();

void music_play(char * filename);
void music_stop(int fade);
void sfx_play_sample(int sample);
void music_invicible_play();
