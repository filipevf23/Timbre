#pragma once

typedef struct CursorObserver CursorObserver; /*  This observes the Text Cursor by connecting to the Port(9000) that has information on it.  */
typedef int FileObserver; /*  This observes File changes in the directory provided by path var.  */
typedef struct Observer Observer;

int InitSocket(void);
char **GetFiles(const char *, int *);

CursorObserver *CreateCursorObserver(const int);
FileObserver *CreateFileObserver(const char *);
Observer *CreateObserver(const char *, const int);
void DestroyObserver(Observer *);
int GetInfo(Observer *, int *);