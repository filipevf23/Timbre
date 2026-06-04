#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <SDL3/SDL.h>

#define FAIL exit(EXIT_FAILURE);
#define READ 0
#define WRITE 1
#define RATE 48000
#define NOTE_A 440.0f
#define CHUNK_SIZE 1024

double note = NOTE_A;

int MyAudioThread(void *data) {
    SDL_AudioStream *stream = (SDL_AudioStream *)data;
    double phase = 0.0;
    float audio_buf[CHUNK_SIZE];

    for (;;) {
        if (SDL_GetAudioStreamAvailable(stream) < (CHUNK_SIZE * sizeof(float) * 2)) {
            for (int i = 0; i < CHUNK_SIZE; i++) {
                audio_buf[i] = (float)sin(phase);
                
                // Avança a onda no tempo usando a 'note' (frequência) atual
                phase += (2.0 * SDL_PI_F * note) / RATE;
                
                // Impede que a variável phase cresça ao infinito
                if (phase > 2.0 * SDL_PI_F) {
                    phase -= 2.0 * SDL_PI_F;
                }
            }
            SDL_PutAudioStreamData(stream, audio_buf, sizeof(audio_buf));
        } else {
            SDL_Delay(1);
        }
    }

    return 0;
}

int main() {

    // PIPE BETWEEN LC AND THIS --------
    const char *myfifo = "/tmp/timbrefifo";
    if (access(myfifo, F_OK) != 0) {
        if ((mkfifo(myfifo, 0666)) == -1) {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
    }
    int fd = 0;
    if ((fd = open(myfifo, O_RDWR)) == -1) {  // O_RDWR is necessary to not send EOF to getline() returning -1.
        perror("open myfifo");
        exit(EXIT_FAILURE);
    }
    FILE *file = fdopen(fd, "r");
    if (file == NULL) {
        perror("fdopen myfifo");
        exit(EXIT_FAILURE);
    }
    // ---------------------------------



    // SDL THINGS ----------------------
    if (!SDL_Init(SDL_INIT_AUDIO)) {
        fprintf(stderr, "Erro SDL Init: %s\n", SDL_GetError());
        FAIL
    }

    const SDL_AudioSpec src_spec = {.format = SDL_AUDIO_F32, .channels = 1, .freq = RATE};
    SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &src_spec, NULL, NULL);
    if (!stream) {
        fprintf(stderr, "Error opening AudioDevice: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_ResumeAudioStreamDevice(stream);
    SDL_CreateThread(MyAudioThread, "My Audio Thread", stream);
    // ---------------------------------

    
    char *buf = NULL;
    size_t len = 0;

    // Waits forever for new notes
    for (;;) {
        if ((getline(&buf, &len, file)) != -1) {
            int line = atoi(buf);
            note = NOTE_A + (double)line;  
        }
    }

    SDL_DestroyAudioStream(stream);
    SDL_Quit();
    free(buf);
    fclose(file);
    return 0;
}
