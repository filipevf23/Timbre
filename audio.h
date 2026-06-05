#pragma once
#include <SDL3/SDL.h>

typedef SDL_AudioStream AudioPlayer;

extern _Atomic double note;

void AddToNote(double add);
AudioPlayer *AudioPlayerInit();
void AudioPlayerQuit(AudioPlayer *);