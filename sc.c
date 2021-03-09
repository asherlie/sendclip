#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <libclipboard.h>

#define PORT 382

/*  */
int create_sock(_Bool list){
      int sock = socket(AF_INET, SOCK_STREAM, 0);
      struct sockaddr_in addr;
      memset(&addr, 0, sizeof(struct sockaddr_in));
      addr.sin_port = PORT;
      addr.sin_family = AF_INET;

      /*addr.sin_addr.s_addr = inet_addr();*/
      bind(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
      if(list)listen(sock, 0);

      return sock;
}

void update_cb(int sock, clipboard_c* c){
      socklen_t len;
      struct sockaddr_in addr;
      int peer;
      int n_bytes;
      while(1){
            if((peer = accept(sock, (struct sockaddr*)&addr, &len) < 0))continue;
            if(read(peer, &n_bytes, sizeof(int)) != sizeof(int))continue;
            char* buf = malloc(n_bytes+1);

            if(read(peer, buf, n_bytes) != n_bytes){
                  free(buf);
                  continue;
            }

            buf[n_bytes] = 0;

            printf("set clipboard contents to \"%s\"\n", buf);
            clipboard_set_text(c, buf);

            free(buf);
            close(peer);
      }
}

_Bool send_clip(char* ip, char* str){
      int sock = create_sock(0);

      struct sockaddr_in addr;
      memset(&addr, 0, sizeof(struct sockaddr_in));

      addr.sin_addr.s_addr = inet_addr(ip);
      addr.sin_family = AF_INET;
      addr.sin_port = PORT;

      connect(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
      
      int len = strlen(str);
      return write(sock, &len, sizeof(int)) == sizeof(int) && write(sock, str, len) == len;
}

/*int main(int a, char** b){*/
int main(){
      clipboard_c* c = clipboard_new(NULL);

      update_cb(create_sock(1), c);

      /*int len;*/
      /*puts(clipboard_text_ex(c, &len, LCB_PRIMARY)); */
      /*clipboard_set_text(c, "hi");*/
      /* clipboard_clear(c, LCB_CLIPBOARD); */
      clipboard_free(c); 
}
