#ifndef UNTITLED_ENCRYPT_H
#define UNTITLED_ENCRYPT_H
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "tests.c"
typedef uint8_t initial[4][4];
int encrypt(initial matrix,initial key);
int encrypt_S(initial matrix,initial key,unsigned long** tencrypt);
#endif //UNTITLED_ENCRYPT_H
