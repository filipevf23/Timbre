#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#define FAIL exit(EXIT_FAILURE)
pid_t pidLC = -1;
pid_t pidPS = -1;
void handler(int);

int main() {

    // SIGNAL HANDLER
    if (signal(SIGINT, handler) == SIG_ERR) {
        perror("signalarg");
        FAIL;
    }
    
    char *pathLC = "./line_catcher";
    char *pathPS = "./play_sound";

    // Start Line Catcher
    if ((pidLC = fork()) == -1) {
        perror("fork 1");
        FAIL;
    } 
    // Line Catcher Child Process
    if (pidLC == 0) {
        execlp(pathLC, pathLC, NULL);
        perror("execlp LC");
        FAIL;
    } 

    // Start Play Sound
    if ((pidPS = fork()) == -1) {
        perror("fork 1");
        FAIL;
    }
    // Play Sound Child Process
    if (pidPS == 0) {
        execlp(pathPS, pathPS, NULL);
        perror("execlp PS");
        FAIL;
    }
    
    size_t sizecmd = 0;
    char *cmd = NULL;
    while ((getline(&cmd, &sizecmd, stdin)) != -1) {
        if (strcmp(cmd, "q\n") == 0) {
            kill(pidPS, SIGTERM);
            kill(pidLC, SIGTERM);
            break;
        }
    }
    if (pidLC > 0) waitpid(pidLC, NULL, 0);
    if (pidPS > 0) waitpid(pidPS, NULL, 0);
    printf("Succesfully ran TIMBRE!\n");
    return 0;
}

void handler(int signum) {
    if (pidLC > 0) kill(pidLC, SIGTERM);
    if (pidPS > 0) kill(pidPS, SIGTERM);
}