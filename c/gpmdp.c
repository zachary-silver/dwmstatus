#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "gpmdp.h"
#include "io.h"

/* Macros */
#define TRUE 1
#define FALSE 0
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
