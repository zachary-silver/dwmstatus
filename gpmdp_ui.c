#include <stdio.h>
#include <string.h>

#include "gpmdp_ui.h"

/* Macros */
#define MAX_SONG_TIME_OUTPUT 6

/* Constants */
static const char *const PauseIcon = "";
static const char *const PlayIcon = "";
static const char *const LikedIcon = "";
static const char *const DislikedIcon = "";

void setSongOutput(gpmdpSong *song)
{
    unsigned int secondsLeft, maxLength;

    if (!song->active) {
        (song->output)[0] = '\0';
        return;
    }

    snprintf(song->output, MAX_SONG_OUTPUT, "%s%s%s %s - %s",
             song->playIcon, (song->likeIcon)[0] != '\0' ? " " : "",
             song->likeIcon, song->artist, song->title);

    /* -2 to include room for a space and the nul terminator */
    maxLength = MAX_SONG_OUTPUT - MAX_SONG_TIME_OUTPUT - 2;
    if (strlen(song->output) > maxLength) {
        strcpy(song->output + maxLength - 3, "...");
    }

    secondsLeft = song->totalSeconds - song->currentSeconds;
    snprintf(song->output + strlen(song->output), MAX_SONG_TIME_OUTPUT + 2,
             " %d:%02d", secondsLeft / 60, secondsLeft % 60);
}

void setSongIcons(gpmdpSong *song)
{
    if (song->liked) {
        song->likeIcon = LikedIcon;
    } else if (song->disliked) {
        song->likeIcon = DislikedIcon;
    } else {
        song->likeIcon = "";
    }

    song->playIcon = song->playing ? PlayIcon : PauseIcon;
}
