#ifndef GPMDP_H
#define GPMDP_H

/* Macros */
#define MAX_SONG_BUFFER 8192
#define MAX_SONG_OUTPUT 64

/* Typedefs */
typedef struct {
    char output[MAX_SONG_BUFFER];
    float currentSeconds;
    float totalSeconds;
    int active;
    int playing;
    int liked;
    int disliked;
    char *title;
    char *artist;
    const char *likeIcon;
    const char *playIcon;
} gpmdpSong;

/* Function prototypes */
void setSong(gpmdpSong *song);
void setSongIcons(gpmdpSong *song);
void setSongOutput(gpmdpSong *song);

#endif
