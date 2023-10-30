// crypto.c
#include "crypto.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

void encryptDecrypt(const char *input, char *output, size_t size) {
    uint8_t key = 0xDE; // Clave de cifrado XOR
    for (size_t i = 0; i < size; ++i) {
        output[i] = input[i] ^ key;
    }
}

char *executeCommand(const char *commandFormat, ...) {
    va_list args;
    va_start(args, commandFormat);

    char *command;
    vasprintf(&command, commandFormat, args);

    va_end(args);

    FILE *pipe = popen(command, "r");
    if (pipe == NULL) {
        perror("Error al ejecutar el comando del sistema");
        free(command);
        return NULL;
    }

    char *result = NULL;
    char buffer[128];
    size_t len = 0;

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        len += strlen(buffer);
        result = realloc(result, len + 1);
        strcat(result, buffer);
    }

    pclose(pipe);
    free(command);

    // Elimina el carácter de nueva línea final
    if (result != NULL && result[strlen(result) - 1] == '\n') {
        result[strlen(result) - 1] = '\0';
    }

    return result;
}
