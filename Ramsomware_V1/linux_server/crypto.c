#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "crypto.h"

uint8_t key = 0xDE; // Clave de cifrado XOR

void encryptDecrypt(const char *input, char *output, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        output[i] = input[i] ^ key;
    }
}

char *executeCommand(const char *format, ...) {
    char *result = NULL;
    char command[4096]; // Tamaño suficiente para almacenar el comando
    va_list args;

    va_start(args, format);
    vsnprintf(command, sizeof(command), format, args);
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

    return result;
}
