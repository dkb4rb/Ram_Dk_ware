// crypto.h
#include <stddef.h>

#ifndef CRYPTO_H
#define CRYPTO_H

void encryptDecrypt(const char *input, char *output, size_t size);
char *executeCommand(const char *commandFormat, ...);

#endif // CRYPTO_H
