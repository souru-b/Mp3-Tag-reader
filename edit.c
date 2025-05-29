#include "main.h"

status edit_mp3file(mp3tagreader *mp, char** argv) 
{
    if (edit_open_files(mp) == e_success) 
    {
        printf("INFO: File opened successfully\n");
    } else 
    {
        printf("ERROR: File not found\n");
        return e_failure;
    }

    if (copy_mp3_header(mp) == e_success) 
    {
        printf("INFO: Header copied successfully\n");
    } else 
    {
        printf("ERROR: Header copy failed\n");
        fclose(mp->fptrfilename);
        fclose(mp->fptrnewfilename);
        return e_failure;
    }

    if (main_edit_function(mp, argv) == e_success) 
    {
        printf("MP3 file edited successfully\n");
        
    } else
    {
        printf("ERROR: MP3 edit failed\n");
        fclose(mp->fptrfilename);
        fclose(mp->fptrnewfilename);
        return e_failure;
    }

    if (copy_remaining_data(mp) == e_success)
    {
        printf("Remaining data copied from '%s' successfully\n", mp->filename);
    }

    // Close files after all operations
    fclose(mp->fptrfilename);
    fclose(mp->fptrnewfilename);
    return e_success;
}

status edit_open_files(mp3tagreader *mp)
{
    mp->fptrfilename = fopen(mp->filename,"rb");
    mp->fptrnewfilename= fopen("duplicate.mp3","wb");
    if( mp->fptrfilename  == NULL ||  mp->fptrnewfilename == NULL)
    {
        printf("ERROR: File not found\n");
        return e_failure;
    }

    return e_success;
}

status copy_mp3_header(mp3tagreader *mp)
{
    char buffer[10];
    fread(buffer,10,1,mp->fptrfilename);
    fwrite(buffer,10,1,mp->fptrnewfilename);

    return e_success;

}

status main_edit_function(mp3tagreader *mp, char** argv)
{

    switch (argv[2][1])
    {
    case 't':
                edit_tag(mp,"TIT2");
                 break;
    case 'a':   
                edit_tag(mp,"TPE1");
                break;
    case 'A': 
                edit_tag(mp,"TALB");
                break;
    case 'y' : 
                edit_tag(mp,"TYER");
                break;
    case 'm' :
                edit_tag(mp,"TCON");
                break;
    case 'c' : 
                edit_tag(mp,"COMM");
                break;
    
    default:
                printf("Invalid or missing sub-option for edit operation.\n");
                display_help();
                break;
    }

    return e_success;

}

void edit_tag(mp3tagreader *mp, char* field) {
    for (int i = 0; i < MAXTAGCOUNT; i++) {
        extract_tag(mp);  // Read the tag
        extract_size(mp); // Read the size
        extract_flag(mp); // Read the flag
        extract_data(mp); // Read the data
        
        // Compare extracted tag with the target field
        if (strcmp(mp->tag, field) == 0) {
            fwrite(mp->tag, 4, 1, mp->fptrnewfilename);
            // Update size and write the new content
             mp->size = conversion_big_to_little_endian(strlen(mp->newcontent) + 1);
            
            fwrite(&mp->size, 4, 1, mp->fptrnewfilename);
            fwrite(mp->flag, 3, 1, mp->fptrnewfilename);
            fwrite(mp->newcontent, strlen(mp->newcontent), 1, mp->fptrnewfilename);

            return; // Exit loop after editing
        }

        // Copy original tag data if no match
        fwrite(mp->tag, 4, 1, mp->fptrnewfilename);
        mp->size = conversion_big_to_little_endian(mp->size+1);
        fwrite(&mp->size, 4, 1, mp->fptrnewfilename);
        fwrite(mp->flag, 3, 1, mp->fptrnewfilename);
        fwrite(mp->data, strlen(mp->data), 1, mp->fptrnewfilename);
    }
}


void extract_flag(mp3tagreader *mp)
{
    fread(mp->flag,3,1,mp->fptrfilename);
}

status copy_remaining_data(mp3tagreader *mp) 
{
    unsigned char ch;
    while (1) 
    {
        ch = fgetc(mp->fptrfilename);
        if (feof(mp->fptrfilename)) 
        {
            break;
        }
        fputc(ch, mp->fptrnewfilename);
    }

    remove(mp->filename);
    rename("duplicate.mp3",mp->filename);
    return e_success;
}


 



