#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>

#define ROLL_CUT_VERSION "00.02.04"
#define READ_BUFFER_LENGTH 4096

// * * * * * GLOBAL VARS * * * * *
unsigned long g_hash_max=0, g_min_cut_size=0, g_magic=104717;
unsigned int g_desired_chunks=256;
char g_folder[1024];

// * * * * * FUNCTIONS * * * * *
char * parse_argv(int argc, char *argv[]);

void init_parameters(char * file_name);

void check_arguments(char *file_name);

void make_folder(char *folder_name);

void print_usage(void);

unsigned long file_cut(char *file_name);

FILE * file_open(char *file_name, char * file_mode);

char *file_next_dest(char *file_name, unsigned long i);

char * name_and_ext(char * file_name);

long file_size(char *file_name);

int file_exist (char *filename);
