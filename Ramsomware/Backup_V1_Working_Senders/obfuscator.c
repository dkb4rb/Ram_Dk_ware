#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

uint8_t key = 0xDE; // Clave de cifrado XOR

void encryptDecrypt(char *input, char *output, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        output[i] = input[i] ^ key;
    }
}



void executeCommand(const char *command) {
    // Ejecuta el comando del sistema
    int status = system(command);

    // Verifica el estado de ejecución del comando
    if (status == -1) {
        perror("Error al ejecutar el comando del sistema");
    } else if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) == 0) {
            printf("Comando ejecutado exitosamente.\n");
        } else {
            printf("El comando del sistema terminó con un código de salida diferente de cero.\n");
        }
    } else {
        printf("El comando del sistema terminó de manera anormal.\n");
    }
}
