#include "timbre.h"
#include <stdio.h>
#include "observe.h"
#include "audio.h"

int Timbre(const char *path) {
    printf("Starting Timber @%s\n", path);

    Observer *obs = CreateObserver(path, 9000);
    AudioPlayer *audioplayer = AudioPlayerInit();

    while (1) {
        int info;
        if (GetInfo(obs, &info) == 0) {
            double newnote = (double) info;
            AddToNote(newnote);
        } else break;
    }

    AudioPlayerQuit(audioplayer);
    DestroyObserver(obs);

    return 0;
}