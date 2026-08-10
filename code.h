#ifndef CODE_H
#define CODE_H

#include <stdio.h>
#include <string.h>

typedef enum
{
    SUCCESS,
    FAILURE
} Status;

typedef enum
{
    VIEW,
    EDIT,
    UNSUPPORTED
} OperationType;

typedef enum
{
    EDIT_TITLE,
    EDIT_ARTIST,
    EDIT_ALBUM,
    EDIT_YEAR,
    EDIT_GENRE,
    EDIT_COMMENT
} EditOption;

typedef struct
{
    FILE *fptr_mp3;
    char *filename;

    char title[100];
    char artist[100];
    char album[100];
    char year[10];
    char comment[200];
    char genre[50];

    EditOption edit_option;
    char *new_content;

} MP3Info;

Status read_mp3_tags(MP3Info *mp3);
Status edit_mp3_tags(MP3Info *mp3);
Status edit_frame(MP3Info *mp3, const char *frame_id_to_edit);

#endif