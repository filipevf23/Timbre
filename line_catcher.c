#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


int main(int argc, char **argv) {

    const char *myfifo = "/tmp/timbrefifo";
    
    if (access(myfifo, F_OK) != 0) {
        if ((mkfifo(myfifo, 0666)) == -1) {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
    }
    int fd = 1;
    if ((fd = open(myfifo, O_WRONLY)) == -1) {
            perror("open myfifo");
            exit(EXIT_FAILURE);
    }

    int port = 9000;
    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in addr = {.sin_family=AF_INET, .sin_port=htons(port), .sin_addr={.s_addr=htonl(INADDR_ANY)}};
    socklen_t sizeaddr = sizeof(addr);
    if ((bind(tcp_socket, (struct sockaddr *)&addr, sizeaddr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(tcp_socket, 3) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    char *buff = malloc(1024*sizeof(char));
    size_t size = 1024;
    int count;
    for (;;) {
        int accepted_socket = accept(tcp_socket, (struct sockaddr*)&addr, &sizeaddr);
        if ((count = recv(accepted_socket, buff, size, 0)) == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        }
        buff = strstr(buff, "linha=") + 6;
        *strstr(buff, " HTTP") = '\0';
        strcat(buff, "\n");
        write(fd ,buff, strlen(buff)*sizeof(char));
        close(accepted_socket);
    }
    close(tcp_socket);
    return 0;
}