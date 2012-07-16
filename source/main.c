#include "main.h"

int main(int argc, char *argv[]) {
    unsigned long cuts=0;
    char *file_name;
    clock_t start_time;

    start_time= clock();
    printf("\nRollCut v%s", ROLL_CUT_VERSION);
    file_name= parse_argv(argc, argv);
    check_arguments(file_name);
    init_parameters(file_name);
    cuts = file_cut(file_name);

    printf("\nChunks : %ld",cuts);
    printf("\nDone in %2.2f sec.\n", ((clock()-start_time)/(double)CLOCKS_PER_SEC));
    exit(0);
}

char * parse_argv(int argc, char *argv[]) {
    int i;
    char *file_name=NULL;

    if (argc==1) {
        print_usage();
    }
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 'd':
                g_desired_chunks = (unsigned int)(atoi(argv[++i]));
                break;
            case 'm':
                g_magic = (unsigned long)(atol(argv[++i]));
                break;
            }
        } else {
            file_name= argv[i];
        }
    }
    return file_name;
}

void check_arguments(char *file_name) {
    if ((file_name==NULL) || !(file_exist(file_name))) {
        printf("\nError, no file to rollcut.\n");
        exit(1);
    }
}

void print_usage(void) {
    printf("\n\nRollCut is used to cut a file in several chunks, based on a rolling hash.");
    printf("\n\nIntended to divide 'similar' big files in smaller chunks, ");
    printf("\nwith a high probability of sharing some equal chunks.");
    printf("\nStoring this chunks instead of the big files, can provide");
    printf("\nbetter space utilization (by using hard-links), and speed up");
    printf("\nnetwork backups with tools like rsync.");
    printf("\n\nUsage: rollcut [-d desired_chunks] [-m magic_number] file_name");
    printf("\n  -d desired_chunks    An estimation of desired chunks. Will be");
    printf("\n                       rounded to a power of 2.");
    printf("\n  -m magic_number      The number that the rolling hash will be");
    printf("\n                       compared to define a cut point.");
    printf("\n  file_name            file to cut.\n");
    exit(0);
}

void init_parameters(char * file_name) {
    int file_bits=0;
    unsigned long size=0;

    sprintf(g_folder,"%s.rollcuts",file_name);
    make_folder(g_folder);
    size = file_size(file_name);
    file_bits = (int) log2(size);
    g_desired_chunks = (1<<(int)(log2(g_desired_chunks)));
    g_hash_max = (unsigned long)((1<<file_bits)/g_desired_chunks-1);
    if (g_hash_max<255) g_hash_max=255;
    g_min_cut_size = (unsigned long)((g_hash_max+1)/4);
    if (g_min_cut_size<(unsigned long)(READ_BUFFER_LENGTH))
		g_min_cut_size=(unsigned long)(READ_BUFFER_LENGTH);
    while (g_hash_max<g_magic) {
        g_magic = (unsigned long)(g_magic/2-1);
    }
    printf("\nFile  : '%s'", file_name);
    printf("\nFolder: '%s'", g_folder);
    printf("\nDesired chunks: %d",g_desired_chunks);
    printf("\nMagic: %lu", g_magic);
    printf("\nHash max: %lu", g_hash_max);
    printf("\nMinCutSize: %lu", g_min_cut_size);
}

void make_folder(char *folder_name){
    #ifdef _WIN32
        mkdir(folder_name);
    #else
        mkdir(folder_name, (S_IRWXU|S_IRGRP|S_IXGRP));
    #endif
}


unsigned long file_cut(char *file_name) {
    char buffer[READ_BUFFER_LENGTH];
    size_t readed_bytes;
    unsigned short i;
    unsigned long hash=1, cuts=0, old_cuts=0, cut_size=0;
    FILE *fsource, *fdest;

    fsource = file_open(file_name, "rb");
    fdest = file_open(file_next_dest(file_name, cuts), "wb");

    printf("\n");
    while ((readed_bytes = fread(buffer, 1, READ_BUFFER_LENGTH, fsource))) {
        for (i=0; i<readed_bytes; i++) {
            hash = ((hash*2)^buffer[i])&g_hash_max;
            cut_size++;
            if ((hash == g_magic) && (g_min_cut_size < cut_size)) {
                cuts++;
                cut_size=0;
            }
        }

        if (old_cuts == cuts) {
            fwrite(buffer, 1, readed_bytes, fdest);
        } else {
            old_cuts = cuts;
            fwrite(buffer, 1, readed_bytes-cut_size, fdest);
            fclose(fdest);
            printf(".");
            fdest = file_open(file_next_dest(file_name, cuts), "wb");
            if (0 < cut_size) {
                fwrite(&buffer[readed_bytes-cut_size], 1, cut_size, fdest);
            }
        }
    }
    fclose(fdest);
    if ((cut_size==0) && (0<cuts)) {
        remove(file_next_dest(file_name,cuts));
        cuts--;
    }
    fclose(fsource);
    printf("\n");
    return cuts;
}

FILE *file_open(char *file_name, char * file_mode) {
    FILE * fp;

    fp = fopen(file_name, file_mode);
    if (fp == NULL) {
        printf("\nError opening '%s'.", file_name);
        exit(1);
    } else {
        return fp;
    }
}

char *file_next_dest(char *file_name, unsigned long i) {
    static char destname[1024];

    sprintf(destname,"%s/%s.%05lu.rollcut", g_folder,name_and_ext(file_name),i);
    return destname;
}

char * name_and_ext(char * file_name) {
    char * c;

    c=strrchr(file_name,'\\');
    if (c==NULL) {
        c=strrchr(file_name,'/');
        if (c==NULL) return file_name;
    }
    return &c[1];
}

long file_size(char *file_name) {
    struct stat st;

    if (stat(file_name, &st)<0) {
        printf("Problem checking size of '%s'.", file_name);
        exit(1);
    }
    return st.st_size;
}

int file_exist (char *filename) {
    struct stat   buffer;
    return (stat (filename, &buffer) == 0);
}
