#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "gpmdp.h"
#include "io.h"

/* Macros */
#define TRUE 1
#define FALSE 0
#define MAX_SONG_TIME_OUTPUT 6
#define NULL_JSON_FILE_SIZE 380
#define JSON_TOKEN_DELIMS " :,\n{}\""

/* Typedefs */
typedef enum {
    Playing,
    Title,
    Artist,
    Liked,
    Disliked,
    Current,
    Total
} SongQuery;

/* Constants */
static const char *const PauseIcon = "";
static const char *const PlayIcon = "";
/* static const char *const StopIcon = ""; */
static const char *const LikedIcon = "";
static const char *const DislikedIcon = "";
static const char *const GPMDPFileName =
    "/home/zack/.config/Google Play Music Desktop Player/json_store/playback.json";
static const char *const SongQueries[] =
{
    "playing",
    "title",
    "artist",
    "liked",
    "disliked",
    "current",
    "total"
};

/* Functions prototypes */
char *queryValue(char *jsonData, SongQuery query);
void setValue(gpmdpSong *song, SongQuery query, char *value);

void setSong(gpmdpSong *song)
{
    SongQuery query = Playing;
    int fd, bytesRead;
    char *value;

    fd = openFile(GPMDPFileName, 'r');
    bytesRead = read(fd, song->output, MAX_SONG_BUFFER - 1);
    closeFile(fd);

    if (bytesRead <= NULL_JSON_FILE_SIZE) {
        song->active = FALSE;
        return;
    }

    song->active = TRUE;
    song->output[bytesRead] = '\0';

    value = queryValue(song->output, query);
    setValue(song, query++, value);
    while (query <= Total) {
        value = queryValue(value + strlen(value) + 1, query);
        setValue(song, query++, value);
    }
}

char *queryValue(char *jsonData, SongQuery query)
{
    char *result = strstr(jsonData, SongQueries[query]);
    result = strstr(result, ":");

    if (query == Artist || query == Title) {
        result = strtok(result, "\"");
        result = strtok(NULL, "\"");
    } else {
        result = strtok(result, JSON_TOKEN_DELIMS);
    }

    return result;
}

void setValue(gpmdpSong *song, SongQuery query, char *value) {
    switch (query) {
    case Playing:
       song->playing = strcmp(value, "true") == 0;
       break;
    case Liked:
       song->liked = strcmp(value, "true") == 0;
       break;
    case Disliked:
       song->disliked = strcmp(value, "true") == 0;
       break;
    case Current:
       song->currentSeconds = atoi(value) / 1000.0;
       break;
    case Total:
       song->totalSeconds = atoi(value) / 1000.0;
       break;
    case Title:
       song->title = value;
       break;
    case Artist:
       song->artist = value;
       break;
    }
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
