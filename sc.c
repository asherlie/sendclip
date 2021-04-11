#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "pooler/pool.h"

#define PORT 12

void p_long_str(char* str){
      int len = strlen(str);
      if(len > 500){
            char tmp = str[199];
            str[199] = 0;

            printf("\"%s\"", str);
            printf("\n\n<OMITTING %i CHARACTERS>\n\n", len-400);

            /*printf("succesfully sent \"%s...%s\" to %s\n", );*/
            str[199] = tmp;
            printf("\"%s\"", str+len-200);
      }
      else printf("\"%s\"", str);
}

#ifdef MAC_OS
#include <signal.h>
#include <libclipboard.h>


void set_block(int sock){
      int fl = fcntl(sock, F_GETFL);
      fl |= O_NONBLOCK;
}

/*  */
int create_sock(_Bool list, _Bool alt_port){
      int sock = socket(AF_INET, SOCK_STREAM, 0);
      struct sockaddr_in addr;
      memset(&addr, 0, sizeof(struct sockaddr_in));
      addr.sin_port = htons(PORT+alt_port);
      addr.sin_port = (PORT+alt_port);
      addr.sin_port = PORT+alt_port;
      addr.sin_family = AF_INET;
      addr.sin_addr.s_addr = INADDR_ANY;

      /*addr.sin_addr.s_addr = inet_addr();*/
      if(bind(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1)perror("bind");
      if(list && listen(sock, 0) == -1)perror("listen()");
      /*
       *int enable = 1;
       *setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
       *setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));
       */


      return sock;
}

void update_cb(int sock, clipboard_c* c){
      socklen_t len = sizeof(struct sockaddr_in);
      struct sockaddr_in addr;
      int peer;
      int n_bytes = 0;
      while(1){
            if((peer = accept(sock, (struct sockaddr*)&addr, &len)) < 0)continue;

            int sz = (read(peer, &n_bytes, sizeof(int)));
            (void)sz;
            /*if(sz != n_bytes)printf("failed to read %i bytes\n", n_bytes);*/
            /*printf("reading %i, %i bytes\n", sz, n_bytes);*/
            char* buf = malloc(n_bytes+1);

            int cc = 0;

            for(int i = 0; i < 500; ++i){
                  cc += read(peer, buf+cc, n_bytes);
                  if(cc == n_bytes)break;
            }
            if(cc != n_bytes){
            /*if((cc = read(peer, buf, n_bytes)) != n_bytes){*/
                  printf("attempted to read %i bytes but only received %i\n", n_bytes, cc);
                  /*printf("%i != %i\n", cc, n_bytes);*/
                  /*puts("read bad bytes");*/
                  free(buf);
                  /*close(sock);*/
                  close(peer);
                  /*exit(0);*/
                  continue;
            }

            buf[n_bytes] = 0;

            /*printf("set clipboard contents to \"%s\"\n", buf);*/
            printf("set clipboard contents to ");
            p_long_str(buf);
            puts("");
            clipboard_set_text(c, buf);

            free(buf);
            close(peer);
      }
}

clipboard_c* clip = NULL;
int lsock = -1;

void exit_safely(int sig){
      (void)sig;
      close(lsock);
      clipboard_free(clip);
      puts("\nclosed local socket and freed memory, exiting safely.");
      exit(0);
}

#endif
_Bool send_clip(char* ip, char* str){
      /*int sock = create_sock(0, 1);*/
      int sock = socket(AF_INET, SOCK_STREAM, 0);

      struct sockaddr_in addr;
      memset(&addr, 0, sizeof(struct sockaddr_in));

      /*(void)ip;*/
      addr.sin_addr.s_addr = inet_addr(ip);
      addr.sin_family = AF_INET;
      addr.sin_port = PORT;

      if(connect(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1){
            return 0;
      }


      /*char ash[6] = {0};*/
      /*
       *printf("was able to read %zi bytes\n", read(sock, ash, 5));
       *printf("read %s\n", ash);
       */
      
      int len = strlen(str);
      int written = write(sock, &len, sizeof(int));
      if(written < 0)perror("write()");
      written += write(sock, str, len);
      printf("wrote %i/%i\n", written, 4+len);
      usleep(1000000);
      return written == (int)sizeof(int)+len;
}

struct send_clip_arg{
    char* ip, * str;
};

volatile void* send_clip_vv(void* v_sca){
    struct send_clip_arg* sca = v_sca;
    if(send_clip(sca->ip, sca->str)){
        printf("succesfully sent ");
        p_long_str(sca->str);
        printf(" to %s\n", sca->ip);
    }
    else printf("failed to send to %s\n", sca->ip);
    free(sca->ip);
    free(sca);
    return NULL;
}

#define CONFIG_FILE ".sendclip"

char** parse_cfg_file(char* fn, int* sz){
      FILE* fp = fopen(fn, "r");
      int cap = 1;
      char** ret = malloc(sizeof(char*)*(cap+1));
      *sz = 0;
      if(!fp)return ret;
      char* ln = NULL;
      size_t lncap = 0;
      ssize_t llen;
      while((llen = getline(&ln, &lncap, fp)) != -1){
            /* leaving one extra space for possible stdin arg */
            if(*sz+1 > cap){
                  cap *= 2;
                  char** tmp = malloc(sizeof(char*)*(cap));
                  memcpy(tmp, ret, sizeof(char*)*(*sz));
                  free(ret);
                  ret = tmp;
            }
            /* overwriting \n */
            ln[llen-1] = 0;
            puts(ln);
            ret[((*sz)++)] = ln;
            ln = NULL;
      }
      free(ln);
      fclose(fp);
      return ret;
}

void p_usage(char** b){
      printf("usage:\n  %s <text> {ip} - send <text> to {ip}'s clipboard as well as all ip addresses set in config file"
                   "\n                   | <text> must be provided, {ip} is optional"
                   "\n  %s             - await connections (mac only)\n"
                   "\n  %s -help       - print this usage information\n", *b, *b, *b);
}

/*writing code to use a config file that specifies a list of ip addresses*/
int main(int a, char** b){
      #ifdef MAC_OS
      if(a == 1){
            clipboard_c* c = (clip = clipboard_new(NULL));
            signal(SIGINT, exit_safely);
            update_cb((lsock = create_sock(1, 0)), c);
      }
      #else
      if(0){
      }
      #endif
      else if(a > 1){
            if(b[1][0] == '-' && b[1][1] == 'h'){
                  p_usage(b);
                  return 0;
            }
            int n_targets = 0;
            char* homedir = getenv("HOME"), ** targets = NULL;
            if(!homedir){
                  puts("no HOME directory is set");
                  if(a <= 2){
                        puts("please provide a recipient");
                        return 1;
                  }
                  /* if no home directory is set, just use stdin recipient */
                  targets = malloc(sizeof(char*));
                  n_targets = 1;
                  *targets = b[2];
            }
            else{
                  char cfgpath[50] = {0};
                  sprintf(cfgpath, "%s/%s", homedir, CONFIG_FILE);
                  targets = parse_cfg_file(cfgpath, &n_targets);

                  /* append stdin recipient to targets, there's a guaranteed space for it */
                  /* done just to simplify logic of freeing targets */
                  if(a > 2)targets[n_targets++] = strdup(b[2]);
            }
            /* TODO: if less than n targets, send individually without the overhead of spawning threads */
            _Bool TP = 1;
            struct thread_pool tp;
            if(TP){
                /*struct thread_pool tp;*/
                init_pool(&tp, (n_targets > 50) ? 50 : n_targets);

                set_await_target(&tp, n_targets);
            }

            for(int i = 0; i < n_targets; ++i){
                  if(TP){
                      struct send_clip_arg* sca = malloc(sizeof(struct send_clip_arg));
                      sca->ip = targets[i];
                      sca->str = b[1];
                      /*exec_pool(send_clip_vv, sca);*/
                      exec_routine(&tp, send_clip_vv, sca);
                  }
                  else{
                      if(send_clip(targets[i], b[1])){
                            printf("succesfully sent ");
                            p_long_str(b[1]);
                            printf(" to %s\n", targets[i]);
                      }
                      else printf("failed to send to %s\n", targets[i]);
                      /*printf("freeing targets[%i]\n", i);*/
                      free(targets[i]);
                  }
            }

            if(TP)await(&tp);

            if(!n_targets)puts("at least one recipient must be specified, either in a config file, or as a stdin argument");
            free(targets);

            if(TP)destroy_pool(&tp);
      }
      else p_usage(b);

      return 0;
}
