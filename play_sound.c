#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <alsa/asoundlib.h>
#include <math.h>
#include <pthread.h>

#define M_PI 3.14159265358979323846
#define FAIL exit(EXIT_FAILURE);
#define READ 0
#define WRITE 1
#define RATE 44100
#define FREQ 169.0f
#define CHUNK_SIZE 1024

double note = FREQ;

void *play_note(void *input) {
    snd_pcm_t *pcm = (snd_pcm_t *)input;
    short buffer[CHUNK_SIZE];
    double phase = 0.0;
    for (;;) {
        
        for (int i=0;i<CHUNK_SIZE; i++) {
            buffer[i] = 32760 * sin(phase);
            // Incrementa a fase gradualmente baseada na nota global atual
            phase += 2.0 * M_PI * note / RATE;
            
            // Evita que a variável phase cresça ao infinito e estoure o limite do double
            if (phase >= 2.0 * M_PI) {
                phase -= 2.0 * M_PI;
            }
        }
        snd_pcm_sframes_t frames = snd_pcm_writei(pcm, buffer, CHUNK_SIZE);
        if (frames < 0) {
            snd_pcm_recover(pcm, frames, 0); // ALSA tenta se recuperar sozinho!
        }
    }
    return NULL;
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

    // ALSA AUDIO SIMPLES --------------
    snd_pcm_t *pcm;  

    // 1. O SEGREDO DA SIMPLICIDADE: Abrir e configurar em 2 linhas
    if (snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        fprintf(stderr, "Erro ao abrir o dispositivo de áudio.\n");
        return 1;
    }
    
    // Configura formato 16-bit, interleaved, 1 canal (mono), taxa 44100, e latência de 500000us (0.5s)
    if (snd_pcm_set_params(pcm, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, 1, RATE, 1, 500000) < 0) {
        fprintf(stderr, "Erro ao configurar parâmetros do ALSA.\n");
        return 1;
    }
    
    // ---------------------------------
    

    pthread_t td;
    pthread_create(&td, NULL, play_note, (void *)pcm);
    pthread_detach(td);
    
    char *buf = NULL;
    size_t len = 0;
    int line = 1;

    // Waits forever for new notes
    for (;;) {
        if ((getline(&buf, &len, file)) != -1) {
            line = atoi(buf);
            note = FREQ + (double)line;  
        }
    }

    pthread_cancel(td);
    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    free(buf);
    fclose(file);
    return 0;
}
