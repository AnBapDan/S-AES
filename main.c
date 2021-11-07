#include <stdio.h>
#include<string.h>
#include <stdint.h>

typedef uint8_t initial[4][4];

static initial keySchedule[10];


//TODO remove and accept input

static initial matrix = {
        {0x32, 0x88, 0x31, 0xe0},
        {0x43, 0x5a, 0x31, 0x37},
        {0xf6, 0x30, 0x98, 0x07},
        {0xa8, 0x8d, 0xa2, 0x34}
};

static initial test = {
        {0x19, 0xa0, 0x9a, 0xe9},
        {0x3d, 0xf4, 0xc6, 0xf8},
        {0xe3, 0xe2, 0x8d, 0x48},
        {0xbe, 0x2b, 0x2a, 0x08}
};

//TODO remove after Mixcolumns
static initial columns = {
        {0x04, 0xe0, 0x48, 0x28},
        {0x66, 0xcb, 0xf8, 0x06},
        {0x81, 0x19, 0xd3, 0x26},
        {0xe5, 0x9a, 0x7a, 0x4c}
};

static initial key = {
        {0x2b, 0x28, 0xab, 0x09},
        {0x7e, 0xae, 0xf7, 0xcf},
        {0x15, 0xd2, 0x15, 0x4f},
        {0x16, 0xa6, 0x88, 0x3c}
};


static uint8_t rcon[10] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};

static uint8_t s_box[16][16] = {
        {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76},
        {0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0},
        {0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15},
        {0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75},
        {0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84},
        {0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf},
        {0xd0, 0xef, 0xaa, 0xfd, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8},
        {0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2},
        {0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73},
        {0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb},
        {0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79},
        {0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08},
        {0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a},
        {0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e},
        {0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf},
        {0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16}
};


//TODO inputs to 16 byte block
int string_matrix(char string[]) {

    for (int i = 0; i < 4; i++) {

        for (int j = 0; j < 4; j++) {
            matrix[j][i] = (uint8_t) string[i + j];

        }

    }
    return 0;

}

int createKeySchedule() {
    //TODO put key automatically instead manual
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            keySchedule[0][j][i] = key[j][i];
        }
    }

    for (int i = 0; i < 10; i++) {
        //last column shift rows up by 1
        uint8_t shifted[4];

        uint8_t temp = keySchedule[i][0][3];
        for (int j = 0; j < 3; j++) {
            shifted[j] = keySchedule[i][j + 1][3];
        }

        shifted[3] = temp;

        //Similar to subbytes
        for (int j = 0; j < 4; j++) {

            uint8_t x = (shifted[j] >> 4);
            uint8_t y = (shifted[j] & 0x0f);
            shifted[j] = s_box[x][y];

        }

        //Print after XOR substitution
        //printf("%X %X %X %X\n", shifted[0], shifted[1], shifted[2], shifted[3]);


        //copy column for XOR
        uint8_t column[4];
        for (int j = 0; j < 4; j++) {
            column[j] = keySchedule[i][j][0];
        }
        //See if correct column copied
        //printf("%X %X %X %X\n", column[0], column[1], column[2], column[3]);

        //final column to insert in next key schedule
        uint8_t entry[4];


        for (int j = 0; j < 4; j++) {
            if (j == 0) {
                entry[j] = shifted[j] ^ column[j] ^ rcon[i];
            } else {
                entry[j] = shifted[j] ^ column[j];
            }
        }

        //printf("%X %X %X %X\n", entry[0], entry[1], entry[2], entry[3]);

        for (int j = 0; j < 4; j++) {
            keySchedule[i + 1][j][0] = entry[j];
        }

        //printf("%X %X %X %X\n", keySchedule[1][0][0], keySchedule[1][1][0], keySchedule[1][2][0], keySchedule[1][3][0]);

        //columns from 2 to 4
        for (int j = 1; j < 4; j++) {

            //copy column for XOR
            for (int k = 0; k < 4; k++) {
                column[k] = keySchedule[i][k][j];
            }

            //xor columns
            for (int k = 0; k < 4; k++) {
                entry[k] = keySchedule[i + 1][k][j - 1] ^ column[k];
            }

            for (int k = 0; k < 4; k++) {
                keySchedule[i + 1][k][j] = entry[k];
            }

        }

        //printf("%X %X %X %X\n", keySchedule[i][0][0], keySchedule[i][1][0], keySchedule[i][2][0], keySchedule[i][3][0]);

    }

    return 0;
}

int AddRoundKey() {
    //TODO alterar test por matrix
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            test[j][i] ^= key[j][i];
        }
    }

    return 0;
}

int SubBytes() {

    //TODO alterar test por matrix
    for (int i = 0; i < 4; i++) {

        for (int j = 0; j < 4; j++) {
            uint8_t x = (test[j][i] >> 4);
            uint8_t y = (test[j][i] & 0x0f);
            test[j][i] = s_box[x][y];

        }

    }
    return 0;
}

int ShiftRows() {
    //TODO alterar test por matrix
    //Row 2
    uint8_t temp = 0;
    temp = test[1][0];
    for (int i = 0; i <= 2; i++) {
        test[1][i] = test[1][i + 1];
    }
    test[1][3] = temp;

    //Row 3
    temp = test[2][0];
    test[2][0] = test[2][2];
    test[2][2] = temp;
    temp = test[2][1];
    test[2][1] = test[2][3];
    test[2][3] = temp;

    //Row 4
    temp = test[3][3];
    for (int i = 3; i > 0; i--) {
        uint8_t tmp = test[3][i - 1];
        test[3][i] = tmp;
    }
    test[3][0] = temp;

    return 0;
}

int MixColumns() {
    //TODO this code is all wrong, need to implement galois field
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            test[j][i] = columns[j][i];
        }
    }
    return 0;
}

int printTestSchedule() {
    printf("%X %X %X %X \n", keySchedule[10][0][0], keySchedule[10][0][1], keySchedule[10][0][2], keySchedule[10][0][3]);
    printf("%X %X %X %X \n", keySchedule[10][1][0], keySchedule[10][1][1], keySchedule[10][1][2], keySchedule[10][1][3]);
    printf("%X %X %X %X \n", keySchedule[10][2][0], keySchedule[10][2][1], keySchedule[10][2][2], keySchedule[10][2][3]);
    printf("%X %X %X %X \n\n", keySchedule[10][3][0], keySchedule[10][3][1], keySchedule[10][3][2], keySchedule[10][3][3]);
}


int printTest() {
    printf("%X %X %X %X \n", test[0][0], test[0][1], test[0][2], test[0][3]);
    printf("%X %X %X %X \n", test[1][0], test[1][1], test[1][2], test[1][3]);
    printf("%X %X %X %X \n", test[2][0], test[2][1], test[2][2], test[2][3]);
    printf("%X %X %X %X \n\n", test[3][0], test[3][1], test[3][2], test[3][3]);
}


int encrypt(){
    createKeySchedule();

    AddRoundKey();

    for(int i = 0; i<9;i++){
        SubBytes();

    }


    return 0;
}

int main() {
    createKeySchedule();
    printTestSchedule();
    /*SubBytes();
    printTest();
    ShiftRows();
    printTest();
    MixColumns();
    AddRoundKey();
    printTest();*/
    return 0;
}