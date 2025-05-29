#ifndef MAIN_H
#define MAIN_H
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#define TITLESIZE 100
#define MAXTAGCOUNT 6

typedef enum {

    e_success,
    e_failure
}status;

typedef enum {
    e_view,
    e_edit,
    e_help,
    e_unsupported
}operationtype;

typedef struct 
{
    char tag[5];                // to store the tag details
    int size;                  // charcater size of tag data
    char data[TITLESIZE];      // to store the data of the tag
    char* filename;           // to store filename
    FILE* fptrfilename;       // to store file pointer adddress
    FILE* fptrnewfilename;      // to  store newfile pointer 
    char* newcontent;           // to store new content
    char flag[4];
}mp3tagreader;

/*function prototypes*/

operationtype checkoperationtype(mp3tagreader *mp, char** argv);
status view_tag(mp3tagreader *mp);
void display_help();
status open_files(mp3tagreader *mp);
status check_header(mp3tagreader *mp);
status skip_header(mp3tagreader *mp);

/*function prototype for view function*/
status main_view_function(mp3tagreader *mp);
status extract_tag(mp3tagreader *mp); // common function called for edit tag
status extract_size(mp3tagreader *mp); // common function called for edit tag
status skip_flag(mp3tagreader *mp);
status extract_data(mp3tagreader *mp); // common function called for edit tag
int conversion_big_to_little_endian(int size);  // common function called for edit tag
void display_mp3_tag_data(const char *tag, const char *tagname, const char *data, int row_number);
status copy_mp3_header(mp3tagreader *mp);

/* function prototype for view function*/
void extract_flag(mp3tagreader *mp);
status main_edit_function(mp3tagreader *mp, char** argv);
void edit_tag(mp3tagreader *mp, char* field);
status copy_remaining_data(mp3tagreader *mp);
status edit_open_files(mp3tagreader *mp);
status edit_mp3file(mp3tagreader *mp,char** argv);

#endif
