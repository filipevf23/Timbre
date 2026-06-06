#pragma once
#include <SDL3/SDL.h>

typedef SDL_AudioStream AudioPlayer;

extern _Atomic int note;
extern _Atomic char notes[];

double getKeyFreq(int key);
void AddToNote(int add);
AudioPlayer *AudioPlayerInit();
void AudioPlayerQuit(AudioPlayer *);