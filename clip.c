#include "clip.h"

#include <unistd.h>
#include <stdlib.h>
#ifdef MAC_OS
#include <signal.h>
#include <libclipboard.h>
#endif

clip_t* init_clip(){
    clip_t* ret = malloc(sizeof(clip_t));
    #ifdef MAC_OS
    ret->c = clipboard_new(NULL);
    #else
    ret->c = NULL;
    #endif
    return ret;
}

void set_clip(clip_t* c, char* buf){
    #ifdef MAC_OS
    clipboard_set_text(c->c, buf);
    #else
    (void)c;
    (void)buf;
    #endif
    return;
}

void clip_destroy(clip_t* c){
    #ifdef MAC_OS
    clipboard_free(c->c);
    #else
    (void)c;
    #endif
}
