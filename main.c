#include<stdio.h>
#include<string.h>
#include "main.h"

// Function to display the help menu
void display_help() {
    printf("\n----------------------------------Help Menu--------------------------------------\n\n");
    printf("1. -v -> to view mp3 file contents\n");
    printf("2. -e -> to edit mp3 file contents\n");
    printf("          -t -> to edit song title\n");
    printf("          -a -> to edit artist name\n");
    printf("          -A -> to edit album name\n");
    printf("          -y -> to edit year\n");
    printf("          -m -> to edit content\n");
    printf("          -c -> to edit comment\n");
    printf("---------------------------------------------------------------------------------\n\n");
}

// Function to validate and determine the operation type
operationtype checkoperationtype(mp3tagreader *mp, char** argv) {
    if (strcmp(argv[1], "-v") == 0) {
        if (argv[2] == NULL || strstr(argv[2], ".") == NULL || strcmp(strstr(argv[2], "."), ".mp3") != 0) {
            printf("Error: Invalid or missing MP3 file name.\n");
            printf("Usage: filename.mp3\n");
            return e_unsupported;
        } else {
            mp->filename = argv[2];
            return e_view;
        }
    } else if (strcmp(argv[1], "-e") == 0) {
        printf("DEBUG: argv[2]: %s\n", argv[2]);
        if (argv[2] == NULL) {
            printf("Error: Missing sub-option for edit operation.\n");
            return e_help;
        }
        if (!(strcmp(argv[2], "-t") == 0 || strcmp(argv[2], "-a") == 0 || strcmp(argv[2], "-y") == 0 || strcmp(argv[2], "-A") == 0 || strcmp(argv[2], "-c") == 0 || strcmp(argv[2], "-m") == 0)) {
            printf("Error: Invalid sub-option '%s' for edit operation.\n", argv[2]);
            return e_help;
        }

        printf("DEBUG: argv[3]: %s\n", argv[3]);
        if (argv[3] == NULL || strlen(argv[3]) == 0) {
            printf("Error: Missing or empty new content for editing.\n");
            return e_help;
        }

        printf("DEBUG: argv[4]: %s\n", argv[4]);
        if (argv[4] == NULL || strstr(argv[4], ".") == NULL || strcmp(strstr(argv[4], "."), ".mp3") != 0) {
            printf("Error: Invalid or missing MP3 file name.\n");
            return e_unsupported;
        }

        mp->newcontent = argv[3];
        mp->filename = argv[4];
        return e_edit;
    } else if (strcmp(argv[1], "--help") == 0) {
        return e_help;
    }

    return e_unsupported;
}

int main(int argc, char** argv) {
    mp3tagreader mp;

    if (argc < 2) {
        printf("Error: Insufficient arguments provided\n");
        display_help();
        return e_failure;
    }

    operationtype op = checkoperationtype(&mp, argv);

    switch (op) {
        case e_view:
            if (view_tag(&mp) == e_success) {
                printf("INFO: Tag viewed successfully\n");
            } else {
                printf("ERROR: Error viewing tag\n");
            }
            break;

        case e_edit:
            if (edit_mp3file(&mp, argv) == e_success) {
                printf("INFO: MP3Tag Reader edited successfully\n");
            } else {
                printf("ERROR: Error editing tag\n");
            }
            break;

        case e_help:
            display_help();
            return e_failure; // Terminate the program after showing help

        default:
            printf("ERROR: Invalid or unsupported operation\n");
            display_help();
            return e_unsupported;
    }

    return 0;
}
