#ifndef UNTITLED_DECRYPT_H
#define UNTITLED_DECRYPT_H
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "tests.c"
typedef uint8_t initial[4][4];
int decrypt(initial matrix, initial key);
int decrypt_S(initial matrixm, initial keym);
#endif //UNTITLED_DECRYPT_H
