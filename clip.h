#ifndef _CLIP_H
#define _CLIP_H
#include <unistd.h>
#include <stdlib.h>
#ifdef MAC_OS
#include <libclipboard.h>
#endif

typedef struct clip_t{
    #ifdef MAC_OS
    clipboard_c* c;
    #else
    void* c;
    #endif
}clip_t;

clip_t* init_clip();
void set_clip(clip_t* c, char* buf);
void clip_destroy(clip_t* c);
#endif
