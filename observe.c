#include "observe.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0600
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
    #define CLOSESOCKET(s) closesocket(s)
    #define GETSOCKETERRNO() (WSAGetLastError())

#elif defined(__linux__) || defined(__APPLE__)
    #include <sys/socket.h>
    #include <netinet/ip.h>
    #include <unistd.h>
    #include <errno.h>
    #include <sys/types.h>
    #include <dirent.h>
    #define ISVALIDSOCKET(s) ((s) >= 0)
    #define CLOSESOCKET(s) close(s)
    #define GETSOCKETERRNO() (errno)
#endif

#define MAXFILENUM 32
#define MAXFILENAMESIZE 256

/* Struct Definitions  ========================== */
struct CursorObserver {
    int socket;
    struct sockaddr_in addr;
    socklen_t sizeaddr;
};

struct FileObserver {
    int filecount;
    char **filenames;
};

struct Observer{
    CursorObserver *co;
    FileObserver *fo;
} ;
/* ============================================= */


int InitSocket(void) {
#ifdef _WIN32
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
    return 0;
#else
    return 0;
#endif
}

char **GetFiles(const char *p, int *filecount) {
    char *path = malloc((strlen(p) + 3)*sizeof(char));
    strcpy(path, p);
    strcat(path, "\\*");
    char **files = malloc(MAXFILENUM*sizeof(char*));
    *filecount = 0;
    for (int i=0; i<MAXFILENUM; i++) files[i] = malloc(MAXFILENAMESIZE*sizeof(char));
#ifdef _WIN32
    WIN32_FIND_DATAA findData;
    HANDLE handle;
    if ((handle = FindFirstFileA(path, &findData)) == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "FileObserver couldn't open directory.\n");
        exit(EXIT_FAILURE);
    }
    do {
        if (*filecount >= MAXFILENUM) break;
        if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
                continue;
            strcpy(files[(*filecount)++], findData.cFileName); 
        }
    } while (FindNextFileA(handle, &findData) != 0);
    FindClose(handle);
#else
    DIR *dir = opendir(path);
    struct dirent* in_file;
    if (dir == NULL) {
        fprintf(stderr, "FileObserver couldn't open directory.\n");
        exit(EXIT_FAILURE);
    }
    while ((in_file = readdir(dir))) {
        if (*filecount >= MAXFILENUM) break;
        if (in_file->d_type == DT_DIR)
                continue;
        if (strcmp(in_file->d_name, ".") != 0 && strcmp(in_file->d_name, "..") != 0) {
            strcpy(files[(*filecount)++], in_file->d_name);
        }
    }
#endif
    return files;
}

CursorObserver *CreateCursorObserver(const int port) {
    /* Create, bind, accept and listen */
    CursorObserver *co = malloc(sizeof(CursorObserver));
    co->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (!ISVALIDSOCKET(co->socket)) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    const char opt = 1;
    if (setsockopt(co->socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    co->addr = (struct sockaddr_in){.sin_family=AF_INET, .sin_port=htons(port), .sin_addr={.s_addr=htonl(INADDR_ANY)}};
    co->sizeaddr = sizeof(co->addr);
    if ((bind(co->socket, (struct sockaddr *)&co->addr, co->sizeaddr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(co->socket, 3) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    return co;
}

FileObserver *CreateFileObserver(const char *dir) {
    FileObserver *fo = malloc(sizeof(FileObserver));

    fo->filenames = GetFiles(dir, &(fo->filecount));
    printf("Observing following files:\n");

    for (int i=0; i<fo->filecount; i++) {
        printf("\t-%s\n", fo->filenames[i]);
    }

    return fo;
}

Observer *CreateObserver(const char *path, const int port) {

    if (InitSocket() != 0) {
        perror("InitSocket");
        exit(EXIT_FAILURE);
    }

    Observer *obs = malloc(sizeof(Observer));

    obs->co = CreateCursorObserver(port);
    obs->fo = CreateFileObserver(path); 

    return obs;
}

void DestroyObserver(Observer *obs) {
    /* Destroying Cursor Observer */
    CLOSESOCKET(obs->co->socket);
    free(obs->co);
    /* Destroying File Observer */
    for (int i=0; i<MAXFILENUM; i++) {
        free(obs->fo->filenames[i]);
    }
    free(obs->fo);
    free(obs);
}

int GetInfo(Observer *obs, int *info) {
    char *buff = malloc(1024*sizeof(char));
    size_t size = 1024;
    int accepted_socket = accept(obs->co->socket, (struct sockaddr*)&obs->co->addr, &obs->co->sizeaddr);
    if (recv(accepted_socket, buff, size, 0) == -1) {
        perror("recv");
        return -1;
    }
    buff = strstr(buff, "linha=") + 6;
    *strstr(buff, " HTTP") = '\0';
    *info = atoi(buff);
    CLOSESOCKET(accepted_socket);
    return 0;
}