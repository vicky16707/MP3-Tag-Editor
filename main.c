/****************************************************************************************************
 * Project Title : MP3 Tag Reader and Editor
 
 * Description :
   A C-based application that reads and edits ID3v2.3 metadata tags of MP3 audio files.
   The project extracts information such as Title, Artist, Album, Year, Genre, and Comment,
   and allows users to modify any of these tags using command-line options while preserving
   the original audio content.
 
 * Features :
   - Validates MP3 files by checking the ID3 header.
   - Displays ID3v2.3 metadata tags.
   - Reads Title, Artist, Album, Year, Genre, and Comment.
   - Edits individual metadata fields using command-line arguments.
   - Creates a temporary MP3 file during editing to ensure safe modification.
   - Replaces the original MP3 file after successful editing.
   - Uses file handling, structures, enums, and bit manipulation concepts.
 

 
 * Author : Vignesh N
 * Roll no : 26010_020
 ****************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "code.h"

int main(int argc, char *argv[])
{
    MP3Info mp3;

    if (argc < 2)
    {
        printf("Usage:\n");
        printf("  View : ./a.out -v filename\n");
        printf("  Edit : ./a.out -e -t/-a/-A/-y/-c/-m new_content filename\n");
        return FAILURE;
    }

    OperationType op = UNSUPPORTED;

    if (strcmp(argv[1], "-v") == 0)
        op = VIEW;
    else if (strcmp(argv[1], "-e") == 0)
        op = EDIT;

    if (op == VIEW)
    {
        if (argc != 3)
        {
            printf("Usage: ./a.out -v filename\n");
            return FAILURE;
        }

        mp3.filename = argv[2];

        if (read_mp3_tags(&mp3) == SUCCESS)
            return SUCCESS;
        else
            return FAILURE;
    }
    else if (op == EDIT)
    {
        if (argc != 5)
        {
            printf("Usage: ./a.out -e -t/-a/-A/-y/-c/-m new_content filename\n");
            return FAILURE;
        }

        mp3.new_content = argv[3];
        mp3.filename = argv[4];

        if (strcmp(argv[2], "-t") == 0)
            mp3.edit_option = EDIT_TITLE;
        else if (strcmp(argv[2], "-a") == 0)
            mp3.edit_option = EDIT_ARTIST;
        else if (strcmp(argv[2], "-A") == 0)
            mp3.edit_option = EDIT_ALBUM;
        else if (strcmp(argv[2], "-y") == 0)
            mp3.edit_option = EDIT_YEAR;
        else if (strcmp(argv[2], "-c") == 0)
            mp3.edit_option = EDIT_GENRE;
        else if (strcmp(argv[2], "-m") == 0)
            mp3.edit_option = EDIT_COMMENT;
        else
        {
            printf("Invalid option\n");
            return FAILURE;
        }

        if (edit_mp3_tags(&mp3) == SUCCESS)
            printf("Tag updated successfully\n");
        else
            printf("Tag update failed\n");

        return SUCCESS;
    }
    else
    {
        printf("Unsupported option\n");
        printf("Usage:\n");
        printf("  View : ./a.out -v filename\n");
        printf("  Edit : ./a.out -e -t/-a/-A/-y/-c/-m new_content filename\n");
        return FAILURE;
    }
}
