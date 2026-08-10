#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "code.h"

Status read_mp3_tags(MP3Info *mp3)
{
    mp3->fptr_mp3 = fopen(mp3->filename, "rb");

    if (mp3->fptr_mp3 == NULL)
    {
        printf("Unable to open file\n");
        return FAILURE;
    }

    char header[10];

    // Read ID3 
    fread(header, 1, 3, mp3->fptr_mp3);

    if (header[0] == 'I' &&
        header[1] == 'D' &&
        header[2] == '3')
    {
        printf("It is an MP3 file\n");
    }
    else
    {
        printf("It is not an MP3 file\n");
        fclose(mp3->fptr_mp3);
        return FAILURE;
    }

    // Read Version 
    unsigned char three[2];

    fread(three, 1, 2, mp3->fptr_mp3);

    if (three[0] == 3)
        printf("It has ID3v2.3\n");
    else if (three[0] == 4)
        printf("It has ID3v2.4\n");

    // To Skip Flags and Tag Size 
    fseek(mp3->fptr_mp3, 5, SEEK_CUR);

    printf("-------------------------\n");

    for (int i = 0; i < 6; i++)
    {
        char frame_id[5];
        unsigned char size_buffer[4];

        if (fread(frame_id, 1, 4, mp3->fptr_mp3) != 4)
            break;

        frame_id[4] = '\0';

        fread(size_buffer, 1, 4, mp3->fptr_mp3);

        int size = (size_buffer[0] << 24) |
                   (size_buffer[1] << 16) |
                   (size_buffer[2] << 8)  |
                    size_buffer[3];

        //to  Skip  the Frame Flags 
        fseek(mp3->fptr_mp3, 2, SEEK_CUR);

        char data[size + 1];

        fread(data, 1, size, mp3->fptr_mp3);

        data[size] = '\0';

        if (strcmp(frame_id, "TIT2") == 0)
        {
            strcpy(mp3->title, data + 1);
            printf("Title   : %s\n", mp3->title);
        }
        else if (strcmp(frame_id, "TPE1") == 0)
        {
            strcpy(mp3->artist, data + 1);
            printf("Artist  : %s\n", mp3->artist);
        }
        else if (strcmp(frame_id, "TALB") == 0)
        {
            strcpy(mp3->album, data + 1);
            printf("Album   : %s\n", mp3->album);
        }
        else if (strcmp(frame_id, "TYER") == 0)
        {
            strcpy(mp3->year, data + 1);
            printf("Year    : %s\n", mp3->year);
        }
        else if (strcmp(frame_id, "COMM") == 0)
        {
            strcpy(mp3->comment, data + 1);
            printf("Comment : %s\n", mp3->comment);
        }
        else if (strcmp(frame_id, "TCON") == 0)
        {
            strcpy(mp3->genre, data + 1);
            printf("Genre   : %s\n", mp3->genre);
        }
        else
        {
            printf("%s : %s\n", frame_id, data + 1);
        }
    }

    fclose(mp3->fptr_mp3);

    return SUCCESS;
}


Status edit_mp3_tags(MP3Info *mp3)
{
    switch (mp3->edit_option)
    {
        case EDIT_TITLE:
            return edit_frame(mp3, "TIT2");

        case EDIT_ARTIST:
            return edit_frame(mp3, "TPE1");

        case EDIT_ALBUM:
            return edit_frame(mp3, "TALB");

        case EDIT_YEAR:
            return edit_frame(mp3, "TYER");

        case EDIT_GENRE:
            return edit_frame(mp3, "TCON");

        case EDIT_COMMENT:
            return edit_frame(mp3, "COMM");

        default:
            printf("Invalid edit option\n");
            return FAILURE;
    }
}
Status edit_frame(MP3Info *mp3, const char *frame_id_to_edit)
{
    FILE *src = fopen(mp3->filename, "rb");

    if (src == NULL)
    {
        printf("Unable to open source file\n");
        return FAILURE;
    }

    FILE *dest = fopen("temp.mp3", "wb");

    if (dest == NULL)
    {
        printf("Unable to create temp file\n");
        fclose(src);
        return FAILURE;
    }

    char header[10];

    // Copy the 10-byte ID3 header as it is
    fread(header, 1, 10, src);
    fwrite(header, 1, 10, dest);

    int frame_found = 0;

    while (1)
    {
        char frame_id[5];
        unsigned char size_buffer[4];
        unsigned char frame_flags[2];

        if (fread(frame_id, 1, 4, src) != 4)
            break;

        frame_id[4] = '\0';

        // Stop at padding (all-zero frame id)
        if (frame_id[0] == 0 && frame_id[1] == 0 &&
            frame_id[2] == 0 && frame_id[3] == 0)
        {
            fseek(src, -4, SEEK_CUR);
            break;
        }

        fread(size_buffer, 1, 4, src);

        int size = (size_buffer[0] << 24) |
                   (size_buffer[1] << 16) |
                   (size_buffer[2] << 8)  |
                    size_buffer[3];

        fread(frame_flags, 1, 2, src);

        if (strcmp(frame_id, frame_id_to_edit) != 0)
        {
            // Not the target frame, copy it unchanged
            fwrite(frame_id, 1, 4, dest);
            fwrite(size_buffer, 1, 4, dest);
            fwrite(frame_flags, 1, 2, dest);

            char data[size];
            fread(data, 1, size, src);
            fwrite(data, 1, size, dest);
        }
        else
        {
            // This is the target frame, replace it
            frame_found = 1;

            unsigned char encoding;
            fread(&encoding, 1, 1, src);

            // Skip the old string in the source file
            fseek(src, size - 1, SEEK_CUR);

            int new_size = strlen(mp3->new_content) + 1;
            unsigned char new_size_buffer[4];

            new_size_buffer[0] = (new_size >> 24) & 0xFF;
            new_size_buffer[1] = (new_size >> 16) & 0xFF;
            new_size_buffer[2] = (new_size >> 8)  & 0xFF;
            new_size_buffer[3] =  new_size        & 0xFF;

            fwrite(frame_id, 1, 4, dest);
            fwrite(new_size_buffer, 1, 4, dest);
            fwrite(frame_flags, 1, 2, dest);

            fwrite(&encoding, 1, 1, dest);
            fwrite(mp3->new_content, 1, strlen(mp3->new_content), dest);
        }
    }

    // Copy the remaining data (rest of the frames / audio data) as it is
    char buffer[1024];
    int bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0)
    {
        fwrite(buffer, 1, bytes, dest);
    }

    fclose(src);
    fclose(dest);

    if (!frame_found)
    {
        printf("%s frame not found in file\n", frame_id_to_edit);
        remove("temp.mp3");
        return FAILURE;
    }

    remove(mp3->filename);
    rename("temp.mp3", mp3->filename);

    return SUCCESS;
}