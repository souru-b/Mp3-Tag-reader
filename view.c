#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "main.h"

// ANSI color codes
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define WHITE   "\033[97m"
#define YELLOW  "\033[93m"
#define LIGHT_BLUE "\033[94m"

status view_tag(mp3tagreader *mp)
{
    status result = e_failure;
    
    if (open_files(mp) == e_success)
    {
        printf("INFO: File opened successfully\n");
        if (check_header(mp) == e_success)
        {
            printf("INFO: ID3 TAG Header viewed successfully\n");
            if (skip_header(mp) == e_success)
            {
                printf("INFO: Skipped header successfully\n");
                result = main_view_function(mp);
                if (result == e_success)
                {
                    printf("INFO: MP3 tag viewed successfully\n");
                }
                else
                {
                    printf("ERROR: Failed to view MP3 tag\n");
                }
            }
            else
            {
                printf("ERROR: Failed to skip header.\n");
            }
        }
        else
        {
            printf("ERROR: Invalid or missing header.\n");
        }
        fclose(mp->fptrfilename);
    }
    else
    {
        printf("ERROR: Failed to open file.\n");
    }

    return result;
}


status open_files(mp3tagreader *mp)
{
    mp->fptrfilename = fopen(mp->filename,"rb");
    if(mp->fptrfilename == NULL)
    {
        perror("Failed to open file\n");
        fprintf(stderr,"ERROR: Unable to open file %s\n",mp->filename);
        return e_failure;
    }
    printf("INFO: %s filename opened successfully\n", mp->filename);
    return e_success;
}

status check_header(mp3tagreader *mp)
{
    char str[4] = {0};
    fread(str,3,1,mp->fptrfilename);
    if(strcmp(str,"ID3") != 0)
    {
        printf("ERROR: The file is not an ID3 tag file\n");
        return e_failure;
    }
    rewind(mp->fptrfilename); // file pointer reset
    return e_success;
}

status skip_header(mp3tagreader *mp)
{
    if(fseek(mp->fptrfilename,10,SEEK_SET) != 0)
    {
        return e_failure;
    }
    long pos = ftell(mp->fptrfilename);
    return e_success;
}

// Function to display tag data in a colorful table
void display_mp3_tag_data(const char *tag, const char *tag_name, const char *data, int row_number)
{
    // Alternate row colors
    const char *row_color = (row_number % 2 == 0) ? WHITE : YELLOW;
    printf("%s│ %-5s │ %-15s │ %-40s │%s\n", row_color, tag, tag_name, data, RESET);
    printf(CYAN "├───────┼─────────────────┼──────────────────────────────────────────┤" RESET "\n");
}


status main_view_function(mp3tagreader *mp)
{
    const char *tags[MAXTAGCOUNT] = {"TIT2", "TPE1", "TALB", "TYER", "TCON", "COMM"};
    const char *tagnames[MAXTAGCOUNT] ={"TITLE","ARTIST","ALBUM","YEAR","CONTENT","COMMENT"};
    int row_number = 0;

    // Print main header
    printf(CYAN "┌──────────────────────────────────────────────────────────┐\n" RESET);
    printf(CYAN "│" LIGHT_BLUE "                  MP3 TAG READER DETAILS                  " CYAN "│\n" RESET);
    printf(CYAN "└──────────────────────────────────────────────────────────┘\n" RESET);

        // Print table header
    printf(CYAN "┌───────┬─────────────────┬──────────────────────────────────────────┐\n" RESET);
    printf(GREEN BOLD "│ TAG   │ TAG NAME        │ DATA                                     │\n" RESET);
    printf(CYAN "├───────┼─────────────────┼──────────────────────────────────────────┤" RESET "\n");

    for(int i=0; i < MAXTAGCOUNT; i++)
    {
        if(extract_tag(mp) != e_success || extract_size(mp) != e_success ||  skip_flag(mp) != e_success || extract_data(mp) != e_success)
        {
          // extract_size inside call the function convert Big endian to little endian
          // extract_data function and run a loop for size times bytes/ use fread
          printf("ERROR: Failed to process tag data\n");
          return e_failure;
        }

       for(int j=0; j < MAXTAGCOUNT; j++) // checking if extracted tag matching of the tag above
       {
            if(strcmp(mp->tag,tags[j]) == 0)
            {

                // Call display function
                display_mp3_tag_data(mp->tag,tagnames[j], mp->data, ++row_number);
                
            }
       } 
        // Stop if EOF or unexpected behavior
        if (feof(mp->fptrfilename))
        {
            break;
        }

    }

        // End table
    printf(CYAN "└───────┴─────────────────┴──────────────────────────────────────────┘\n" RESET);
    return e_success;

}

status extract_tag(mp3tagreader *mp)
{
    char tagger[5] = {0};
    size_t bytes_read = fread(tagger,4,1,mp->fptrfilename);
    if(bytes_read != 1)
    {
        printf("ERROR: Failed to read tag\n");
        return e_failure;
    }

    strncpy(mp->tag,tagger,4);     // content copied to mp->tag
    mp->tag[4] = '\0';
    return e_success;
}

status extract_size(mp3tagreader *mp)
{
  
    size_t bytes_read = fread(&mp->size,4,1,mp->fptrfilename);

    if(bytes_read != 1)
    {
        printf("ERROR: Failed to read size\n");
        return e_failure;
    }

    mp->size = conversion_big_to_little_endian(mp->size)-1;
    return e_success;
}

int conversion_big_to_little_endian(int size)
{
    char *ptr = (char*)&size;
    for(int i=0; i < 2; i++)
    {
        char temp = ptr[i];
        ptr[i] = ptr[4-i-1];
        ptr[4-i-1] = temp;
    }

    return size;
}

status skip_flag(mp3tagreader *mp)
{
    if(fseek(mp->fptrfilename,3,SEEK_CUR) !=0)
    {
        return e_failure;
    }
    return e_success;
}

status extract_data(mp3tagreader *mp)
{
    if (mp->size > TITLESIZE)
    {
        printf("ERROR: Data size (%d) exceeds buffer limit (%d)\n", mp->size, TITLESIZE);
        return e_failure;
    }
    size_t bytes_read = fread(mp->data,1,mp->size,mp->fptrfilename);

    if(bytes_read != (size_t)mp->size)
    {
        printf("ERROR: Failed to read the data from '%s'\n",mp->filename);
        printf("ERROR: Failed to read the data (expected %d bytes, but got %zu bytes)\n", mp->size, bytes_read);
        return e_failure;
    }
    mp->data[mp->size] = '\0';  // null termination of the string
    return e_success;
}

