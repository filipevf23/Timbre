#include "observe.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0600
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
    #define GETSOCKETERRNO() (WSAGetLastError())
    #define CLOSESOCKET(s) closesocket(s)

#elif defined(__linux__) || defined(__APPLE__)
    #include <sys/socket.h>
    #include <netinet/ip.h>
    #include <unistd.h>
    #include <errno.h>
    #define ISVALIDSOCKET(s) ((s) >= 0)
    #define CLOSESOCKET(s) close(s)
    #define GETSOCKETERRNO() (errno)
#endif


/* Struct Definitions  ========================== */
struct CursorObserver {
    int socket;
    struct sockaddr_in addr;
    socklen_t sizeaddr;
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

CursorObserver *CreateCursorObserver(const int port) {
    /* Create, bind, accept and listen */
    if (InitSocket() != 0) {
        perror("InitSocket");
        exit(EXIT_FAILURE);
    }
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

FileObserver *CreateFileObserver() {
    // This is placeholder!
    int *fo = malloc(sizeof(int));
    *fo = 1;
    return fo;
}

Observer *CreateObserver(const char *path, const int port) {

    Observer *obs = malloc(sizeof(Observer));

    obs->co = CreateCursorObserver(port);
    obs->fo = CreateFileObserver(); // This does nothing for now

    return obs;
}

void DestroyObserver(Observer *obs) {
    CLOSESOCKET(obs->co->socket);
    free(obs->co);
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