#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void executeCommand(const char *command) {
    // Ejecutar el comando con el sistema
    int status = system(command);

    // Verificar el estado de ejecución del comando
    if (WIFEXITED(status)) {
        printf("Comando terminado con código de salida: %d\n", WEXITSTATUS(status));
    } else {
        printf("El comando terminó de manera anormal.\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <comando>\n", argv[0]);
        return 1;
    }

    // Unir los argumentos de la línea de comandos en un solo string
    char command[1024];
    strcpy(command, argv[1]);
    for (int i = 2; i < argc; i++) {
        strcat(command, " ");
        strcat(command, argv[i]);
    }

    // Si el comando es "cd", cambiar el directorio de trabajo
    if (strncmp(argv[1], "cd", 2) == 0) {
        if (argc > 2) {
            if (chdir(argv[2]) != 0) {
                perror("Error al cambiar el directorio");
            } else {
                printf("Directorio cambiado a %s\n", argv[2]);
            }
        } else {
            printf("Uso: cd <directorio>\n");
        }
    } else {
        // Ejecutar otros comandos
        executeCommand(command);
    }

    return 0;
}
