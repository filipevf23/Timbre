#include <stdio.h>
#include <stdlib.h>
#include "timbre.h"



#ifdef _WIN32
    #include <direct.h>
    #define getcwd _getcwd
#elif defined(__linux__) || defined(__APPLE__)
    #include <unistd.h>
#endif


int main(int argc, char **argv) {
    printf("Starting TIMBRE . . .\n");

    char path[4096];
    if (getcwd(path, sizeof(path)) == NULL) {
        perror("getcwd()");
        exit(EXIT_FAILURE);
    }

    Timbre(path);

    printf("Succesfully ran TIMBRE!\n");
    return 0;
}