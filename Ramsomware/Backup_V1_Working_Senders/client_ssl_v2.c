#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "crypto.h"

#define PORT 4433

int main() {
    SSL *ssl;
    SSL_CTX *ctx;
    int server_fd;
    char buf[1024];

    // Inicializa OpenSSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    // Crea un contexto SSL/TLS
    ctx = SSL_CTX_new(SSLv23_client_method());
    if (ctx == NULL) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // Crea un socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    // Configura la dirección del servidor
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
    };

    // Conecta al servidor
    if (connect(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Unable to connect");
        close(server_fd);
        SSL_CTX_free(ctx);
        ERR_free_strings();
        EVP_cleanup();
        exit(EXIT_FAILURE);
    }

    // Crea una estructura SSL y la asocia con el socket
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, server_fd);

    // Establece la conexión SSL/TLS
    if (SSL_connect(ssl) != 1) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    printf("SSL/TLS handshake successful. Connected to server.\n");

    // Bucle para leer y enviar mensajes continuamente
    while (1) {
        // Solicita al usuario que ingrese un comando para enviar al servidor
        printf("Enter command to send (or type 'exit' to quit): ");
        memset(buf, 0, sizeof(buf));
        fgets(buf, sizeof(buf) - 1, stdin);
        buf[strcspn(buf, "\n")] = '\0';  // Elimina el carácter de nueva línea

        // Asegúrate de que el mensaje termina con un carácter nulo
        buf[strlen(buf)] = '\0';

        // Cifra y obfusca el mensaje antes de enviarlo
        encryptDecrypt(buf, buf, strlen(buf) + 1);
        SSL_write(ssl, buf, strlen(buf) + 1);

        // Sal del bucle si el usuario escribe 'exit'
        if (strncmp(buf, "exit", 4) == 0) {
            printf("Exiting.\n");
            break;
        }

        // Recibe y muestra la respuesta del servidor
        memset(buf, 0, sizeof(buf));
        int bytes_received = SSL_read(ssl, buf, sizeof(buf));
        if (bytes_received <= 0) {
            // Error o conexión cerrada por el servidor
            ERR_print_errors_fp(stderr);
            break;
        }

        // Descifra y desobfusca el mensaje recibido del servidor antes de mostrarlo
        encryptDecrypt(buf, buf, bytes_received);
        printf("Received message from server: %s\n", buf);
    }

    // Cierra la conexión SSL/TLS y el socket
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(server_fd);

    // Limpia y libera recursos de OpenSSL
    SSL_CTX_free(ctx);
    ERR_free_strings();
    EVP_cleanup();

    return 0;
}
