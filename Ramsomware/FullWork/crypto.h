// crypto.h
#include <stddef.h>
#ifndef CRYPTO_H
#define CRYPTO_H

void encryptDecrypt(const char *input, char *output, size_t size);
char* execCommand(const char *command);
char *executeCommand(const char *format, ...);
#endif // CRYPTO_H
