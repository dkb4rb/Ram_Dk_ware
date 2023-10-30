#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "crypto.h"

uint8_t key = 0xDE; // Clave de cifrado XOR

void encryptDecrypt(const char *input, char *output, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        output[i] = input[i] ^ key;
    }
}

char *executeCommand(const char *format, ...) {
    char *result = NULL;
    char *command = NULL;
    va_list args;

    va_start(args, format);
    vsprintf(&command, format, args);
    va_end(args);

    FILE *pipe = popen(command, "r");
    if (pipe != NULL) {
        char buffer[128];
        size_t len = 0;

        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            size_t buffer_len = strlen(buffer);
            result = realloc(result, len + buffer_len + 1);
            if (result == NULL) {
                perror("Error en realloc");
                exit(EXIT_FAILURE);
            }
            strcpy(result + len, buffer);
            len += buffer_len;
        }

        pclose(pipe);
    } else {
        perror("Error al ejecutar el comando del sistema");
        exit(EXIT_FAILURE);
    }

    free(command);
    return result;
}


// funcione vieja

char* execCommand(const char *command) {
    FILE *fp;
    char *output = NULL;
    size_t len = 0;
    
    // Abre un proceso para ejecutar el comando y capturar su salida
    fp = popen(command, "r");
    if (fp == NULL) {
        perror("Error al ejecutar el comando del sistema");
        return NULL;
    }

    // Lee la salida del comando línea por línea
    while (getline(&output, &len, fp) != -1) {
        // Concatena las líneas de salida en una cadena continua
        output = realloc(output, strlen(output) + len + 1);
        strcat(output, "\n");
    }

    // Cierra el flujo de entrada del proceso
    pclose(fp);
    
    // Elimina el carácter de nueva línea final
    if (output != NULL && output[strlen(output) - 1] == '\n') {
        output[strlen(output) - 1] = '\0';
    }

    return output;
}
