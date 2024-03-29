#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <openssl/evp.h>
#include "encrypt.h"
#include "decrypt.h"
#include "wmmintrin.h"

#define KEY_LEN 16 /* bytes */
char salt = 0;
uint8_t firstkey[KEY_LEN];
int sizeoutput;
int flag = 0;
uint8_t seed;
typedef uint8_t initial[4][4];
initial matrix;
initial key;




unsigned long sum;
unsigned long tencrypt;
unsigned long tdecrypt;

void getKey(char *pwd);

void printTest() {
    printf("%02x %02x %02x %02x \n", matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]);
    printf("%02x %02x %02x %02x \n", matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]);
    printf("%02x %02x %02x %02x \n", matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]);
    printf("%02x %02x %02x %02x \n\n", matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
}

uint8_t *ecb_mode_decrypt(uint8_t *string, int size) {

    uint8_t *output = malloc(size * sizeof(uint8_t));
    //insert string into matrix

    uint8_t padding;
    int count = 0;
    int count2 = 0;
    for (int i = 0; i < size / 16; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {

                matrix[k][j] = string[count];
                count++;
            }
        }

        if (flag) {
            decrypt_S(matrix, key, &tdecrypt);
            sum+=tdecrypt;

        } else {
            decrypt(matrix, key);
        }


        for (int j = 0; j < 4; j++) {
            if (count2 == size - padding) {
                break;
            }
            for (int k = 0; k < 4; k++) {
                output[count2] = matrix[k][j];
                count2++;
            }
        }
    }
    padding = output[size - 1];
    sizeoutput = count2 - padding;
    return output;
}

void getKey(char *pwd) {//hashing key

    // The PBKDF2 generator of OpenSSL receives as input the number of byes to generate ,
    // instead of bits
    PKCS5_PBKDF2_HMAC_SHA1(pwd, -1, &salt, 1, 1000, sizeof(firstkey), firstkey);

    for (int i = 0; i < sizeof(firstkey); i++) {
        key[i % 4][i / 4] = firstkey[i];
    }

}

uint8_t getSeed(char *pwd) {//hashing key
    uint8_t firstsk[KEY_LEN];
    uint8_t seed;
    // The PBKDF2 generator of OpenSSL receives as input the number of byes to generate ,
    // instead of bits
    PKCS5_PBKDF2_HMAC_SHA1(pwd, -1, &salt, 1, 1000, sizeof(firstsk), firstsk);
    for (int i = 0; i < KEY_LEN; i++) {
        seed += firstsk[i];
    }
    return seed;
}

uint8_t *ecb_mode_encrypt(char *string, int size) {

    ///add PKCS#7
    uint8_t padding = 16 - (size % 16);

    for (int i = size; i < size + padding; i++) {
        string[i] = padding;
    }

    ///insert string into matrix
    size=strlen(string);
    uint8_t *output = malloc((size - 1) * sizeof(uint8_t));


    int count = 0;
    int count2 = 0;

    for (int i = 0; i < size / 16; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                matrix[k][j] = string[count];
                count++;
            }
        }
        if (flag) {

            encrypt_S(matrix, key, &tencrypt);
            sum += tencrypt;

        } else {
            encrypt(matrix, key);
        }


        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                output[count2++] = matrix[k][j];
            }
        }
    }
    sizeoutput = count2;
    return output;
}

int main(int argc, char *argv[]) {
    // ./a.out pwd(string) mode ( D or E )
    ///Setup
    char string[4096];
    char d[4096 * 2];
    uint8_t dec[4112];
    getKey(argv[1]);
    int size;

    uint8_t *result;

    if (argc == 4) {
        //Change mode from AES to S-AES
        flag = 1;

        //SK to fill the srand();
        seed = getSeed(argv[2]);
        srand(seed);

        if (*argv[3] == 'D') {
            printf("Text: ");
            //input and size
            scanf("%s", d);
            size = 0;
            while (d[size] != '\0') {
                size++;
            }

            size = size / 2;

            char tmp[3];
            tmp[2] = '\0';
            int j = 0;

            for (int i = 0; i < size * 2; i = i + 2) {
                tmp[0] = d[i];
                tmp[1] = d[i + 1];
                dec[j] = strtol(tmp, NULL, 16);
                j++;
            }

            result = ecb_mode_decrypt(dec, size);
            printf("Decrypted: ");
            for (int j = 0; j < sizeoutput; j++) {
                printf("%02x", result[j]);
            }

        }
        else if (*argv[3] == 'E') {
            printf("Text: ");
            //input and size
            scanf("%s", string);
            size = strlen(string);
            result = ecb_mode_encrypt(string, size);
            printf("Encrypted: ");
            for (int j = 0; j < sizeoutput; j++) {
                printf("%02x", result[j]);
            }

        }
        else if (*argv[3] == 'S') {
            unsigned long min = ULLONG_MAX;
            for (int i = 0; i < 1; i++) {
                sum=0;
                FILE * stream;
                stream = fopen("C:\\Users\\DanielAndrade\\CLionProjects\\untitled\\test.txt", "r");
                int count = fread(&string, sizeof(char), 4096, stream);
                fclose(stream);
                result=ecb_mode_encrypt(string, 4096);
                srand(seed);
                ecb_mode_decrypt(result, sizeoutput);

                if (min > sum) {
                    min = sum;
                }
            }
            printf("Elapsed time: %lu", min);
        }
        else {
            perror("Bad command usage. Try: ./a.out Key SKey mode ( D , E or S) ");
        }

        return 0;
    }
    printf("Text: ");
    if (*argv[2] == 'D') {
        //input and size
        scanf("%s", d);
        size = 0;
        while (d[size] != '\0') {
            size++;
        }

        size = size / 2;

        char tmp[3];
        tmp[2] = '\0';
        int j = 0;

        for (int i = 0; i < size * 2; i = i + 2) {
            tmp[0] = d[i];
            tmp[1] = d[i + 1];
            dec[j] = strtol(tmp, NULL, 16);
            j++;
        }
        result = ecb_mode_decrypt(dec, size);
        printf("Decrypted: ");
        for (int j = 0; j < sizeoutput; j++) {
            printf("%02x", result[j]);
        }

    }
    else if (*argv[2] == 'E') {
        //input and size
        scanf("%s", string);
        size = strlen(string);
        result = ecb_mode_encrypt(string, size);
        printf("Encrypted: ");
        for (int j = 0; j < sizeoutput; j++) {
            printf("%02x", result[j]);
        }

    }
    else {
        perror("Bad command usage. Try: ./a.out Key SKey mode(D , E) ");
    }

    printf("\n\n");


    return 0;
}