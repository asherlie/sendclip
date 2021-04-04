#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 12

#ifdef MAC_OS
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

int read_int(int sock){
      int ret = 0, br = 0, tmp;
      /* timeout */
      /*char rret[4];*/
      for(int i = 0; i < 1000; ++i){
            /*tmp = read(sock, ((char*)&ret)+br, 4-br);*/
            tmp = read(sock, ((char*)&ret)+(4-br), 4-br);
            /*tmp = read(sock, rret+br, 4-br);*/
            /*if(tmp < 0)continue;*/
            if(tmp < 0){
                  perror("read()");
                  return -1;
            }
            br += tmp;
            printf("read %i bytes, total %i\n", tmp, br);
            if(br == sizeof(int))break;
            usleep(100);
      }
      /*printf("%i, %i, %i, %i\n", rret[0], rret[1], rret[2], rret[3]);*/
      return ret;
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
                  close(sock);
                  close(peer);
                  /*exit(0);*/
                  continue;
            }

            buf[n_bytes] = 0;

            printf("set clipboard contents to \"%s\"\n", buf);
            clipboard_set_text(c, buf);

            free(buf);
            close(peer);
      }
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

      if(connect(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1)perror("connect()");

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

void zp_long_str(char* str){
      int len = strlen(str);
      if(len > 500){
            char tmp = str[199];
            str[199] = 0;


            /*printf("succesfully sent \"%s...%s\" to %s\n", );*/
            printf("\"%s...", str);
            str[199] = tmp;
            printf("%s\"", str+len-200);
      }
      else printf("\"%s\"", str);
}


int main(int a, char** b){
      /*
       *close(STDIN_FILENO);
       *close(STDOUT_FILENO);
       */
/*int main(){*/

      // ./sc <ip> <msg>
      // ./sc

      #ifdef MAC_OS
      if(a == 1){
            clipboard_c* c = clipboard_new(NULL);
            update_cb(create_sock(1, 0), c);
      }
      #else
      if(0){
      }
      #endif
      else if(a >= 3){
            if(send_clip(b[1], b[2])){
                  printf("succesfully sent ");
                  p_long_str(b[2]);
                  printf(" to %s\n", b[1]);
            }
            else fputs("failed to send to clipboard", stderr);
      }
      else{
            printf("usage:\n  %s <ip> <text> - send <text> to <ip>'s clipboard\n  %s             - await connections\n", *b, *b);
      }
      /*
       *switch(b[1]){
       *      case 's':
       *            break;
       *}
       */
      /*update_cb(create_sock(1), c);*/

      /*int len;*/
      /*puts(clipboard_text_ex(c, &len, LCB_PRIMARY)); */
      /*clipboard_set_text(c, "hi");*/
      /* clipboard_clear(c, LCB_CLIPBOARD); */
      /*clipboard_free(c); */
}
