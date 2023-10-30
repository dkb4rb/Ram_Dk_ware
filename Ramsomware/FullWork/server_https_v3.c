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
    SSL_CTX *ctx;
    int server_fd, client_fd;
    char buf[4096];

    // Inicializa OpenSSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    // Crea un contexto SSL/TLS
    ctx = SSL_CTX_new(SSLv23_server_method());
    if (ctx == NULL) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // Carga el certificado y la clave privada del servidor
    if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
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
        .sin_addr.s_addr = INADDR_ANY,
    };

    // Asocia el socket con la dirección del servidor
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Unable to bind");
        close(server_fd);
        SSL_CTX_free(ctx);
        ERR_free_strings();
        EVP_cleanup();
        exit(EXIT_FAILURE);
    }

    // Escucha conexiones entrantes
    if (listen(server_fd, 5) == -1) {
        perror("Unable to listen");
        close(server_fd);
        SSL_CTX_free(ctx);
        ERR_free_strings();
        EVP_cleanup();
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Acepta conexiones y maneja mensajes de clientes
    while (1) {
        // Acepta la conexión del cliente
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            perror("Unable to accept");
            continue;
        }

        // Crea una estructura SSL y la asocia con el socket del cliente
        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_fd);

        // Establece la conexión SSL/TLS con el cliente
        if (SSL_accept(ssl) != 1) {
            ERR_print_errors_fp(stderr);
            close(client_fd);
            SSL_free(ssl);
            continue;
        }

        printf("SSL/TLS handshake successful. Client connected.\n");

        // Bucle para leer y procesar mensajes del cliente
        while (1) {
            printf("Enter command: ");
            memset(buf, 0, sizeof(buf));
            fgets(buf, sizeof(buf) - 1, stdin);

            buf[strcspn(buf, "\n")] = '\0';  // Elimina el carácter de nueva línea

            // Asegúrate de que el comando termina con un carácter nulo
            buf[strlen(buf)] = '\0';

            // Cifra y obfusca el mensaje antes de enviarlo
            encryptDecrypt(buf, buf, strlen(buf) + 1);
            SSL_write(ssl, buf, strlen(buf) + 1);

            if (strncmp(buf, "exit", 4) == 0) {
                printf("Exiting. \n");
                break;
            }

            // Recibe el mensaje del cliente
            memset(buf, 0, sizeof(buf));
            int bytes_received = SSL_read(ssl, buf, sizeof(buf));
            if (bytes_received <= 0) {
                // Error o conexión cerrada por el cliente
                ERR_print_errors_fp(stderr);
                break;
            }

            // Descifra el mensaje recibido del cliente antes de mostrarlo

            encryptDecrypt(buf, buf, bytes_received);
            printf("%s\n", buf);
        }

        // Cierra la conexión SSL/TLS y el socket del cliente
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client_fd);
        printf("Client disconnected.\n");
    }

    // Cierra el socket del servidor y libera recursos de OpenSSL
    close(server_fd);
    SSL_CTX_free(ctx);
    ERR_free_strings();
    EVP_cleanup();
    free(buf);
    return 0;
}
