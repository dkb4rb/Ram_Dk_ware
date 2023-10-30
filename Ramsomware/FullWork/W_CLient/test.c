#define _NO_OLDNAMES
#include <openssl/ssl.h>
#include <openssl/comp.h>
#include <openssl/err.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int main() {
    SSL_library_init();
    // Resto de tu código de prueba aquí
    return 0;
}

