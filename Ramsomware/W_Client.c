#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

#define PORT 443

int main() {
    WSADATA wsaData;
    SOCKET server_fd;
    struct sockaddr_in server_addr;
    SSL_CTX *ctx;
    SSL *ssl;
    char buf[1024];

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Failed to initialize Winsock");
        exit(EXIT_FAILURE);
    }

    // Inicializar OpenSSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    // Crear un contexto SSL/TLS
    ctx = SSL_CTX_new(SSLv23_client_method());
    if (ctx == NULL) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // Crear un socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Conectar al servidor
    if (connect(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        perror("Unable to connect");
        closesocket(server_fd);
        SSL_CTX_free(ctx);
        ERR_free_strings();
        EVP_cleanup();
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Crear una estructura SSL y asociarla con el socket
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, server_fd);

    // Establecer la conexión SSL/TLS
    if (SSL_connect(ssl) != 1) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    printf("SSL/TLS handshake successful. Connected to server.\n");

    // Bucle para leer y enviar mensajes continuamente
    while (1) {
        // Enviar mensaje al servidor
        printf("Enter message to send (or type 'exit' to quit): ");
        fgets(buf, sizeof(buf), stdin);
        SSL_write(ssl, buf, strlen(buf));

        // Salir del bucle si el usuario escribe 'exit'
        if (strncmp(buf, "exit", 4) == 0) {
            break;
        }

        // Recibir la respuesta del servidor
        memset(buf, 0, sizeof(buf));
        int bytes_received = SSL_read(ssl, buf, sizeof(buf));
        if (bytes_received <= 0) {
            // Error o conexión cerrada por el servidor
            break;
        }

        // Procesar la respuesta del servidor según sea necesario
        printf("Received message from server: %.*s", bytes_received, buf);
    }

    // Cerrar la conexión SSL/TLS y el socket
    SSL_shutdown(ssl);
    SSL_free(ssl);
    closesocket(server_fd);

    // Limpiar y liberar recursos de OpenSSL y Winsock
    SSL_CTX_free(ctx);
    ERR_free_strings();
    EVP_cleanup();
    WSACleanup();

    return 0;
}
