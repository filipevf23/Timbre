#include "audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdatomic.h>

#define RATE 48000
#define NOTE_A 440.0f
#define ALL_NOTES 88
#define STARTING_NOTE 1
#define CHUNK_SIZE 1024

_Atomic int note = STARTING_NOTE;
_Atomic char notes[ALL_NOTES+1] = {0};

void AddToNote(int add) {
    if (add > ALL_NOTES) return;
    notes[add] = 1;
    note = STARTING_NOTE + add;
}

double getKeyFreq(int key) {
    return pow(2, (double)(note - 49)/(double)12)*NOTE_A;
}

int MyAudioThread(void *data) {
    SDL_AudioStream *stream = (SDL_AudioStream *)data;
    double phase = 0.0;
    float audio_buf[CHUNK_SIZE];

    for (;;) {
        if (SDL_GetAudioStreamAvailable(stream) < (CHUNK_SIZE * sizeof(float) * 2)) {
            for (int i = 0; i < CHUNK_SIZE; i++) {
                audio_buf[i] = (float)sin(phase);
                phase += (2.0 * SDL_PI_F * getKeyFreq(note)) / RATE;
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

AudioPlayer *AudioPlayerInit() {
    if (!SDL_Init(SDL_INIT_AUDIO)) {
        fprintf(stderr, "Erro SDL Init: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    const SDL_AudioSpec src_spec = {.format = SDL_AUDIO_F32, .channels = 1, .freq = RATE};
    SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &src_spec, NULL, NULL);
    if (!stream) {
        fprintf(stderr, "Error opening AudioDevice: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_ResumeAudioStreamDevice(stream);
    SDL_CreateThread(MyAudioThread, "My Audio Thread", stream);

    return stream;
}

void AudioPlayerQuit(AudioPlayer *player) {
    SDL_DestroyAudioStream(player);
    SDL_Quit();
}






















