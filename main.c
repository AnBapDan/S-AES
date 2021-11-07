#include <stdio.h>
#include <stdint.h>

typedef uint8_t initial[4][4];

static initial keySchedule[11];

//TODO remove and accept input

static initial matrix = {
        {0x32, 0x88, 0x31, 0xe0},
        {0x43, 0x5a, 0x31, 0x37},
        {0xf6, 0x30, 0x98, 0x07},
        {0xa8, 0x8d, 0xa2, 0x34}
};

static initial key = {
        {0x2b, 0x28, 0xab, 0x09},
        {0x7e, 0xae, 0xf7, 0xcf},
        {0x15, 0xd2, 0x15, 0x4f},
        {0x16, 0xa6, 0x88, 0x3c}
};


static const uint8_t rcon[10] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36};

static const uint8_t s_box[16][16] = {
        {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76},
        {0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0},
        {0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15},
        {0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75},
        {0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84},
        {0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf},
        {0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8},
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

//s_box inverted. The index position of the above S_box are now the values and vice versa. Code retrieved from: https://github.com/kokke/tiny-AES-c/blob/master/aes.c

static const uint8_t invs_box[16][16] = {
        {0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb},
        {0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb},
        {0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e},
        {0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25},
        {0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92},
        {0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84},
        {0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06},
        {0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b},
        {0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73},
        {0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e},
        {0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b},
        {0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4},
        {0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f},
        {0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef},
        {0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61},
        {0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d} };


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

int AddRoundKey(int round) {
    //TODO alterar test por matrix
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix[j][i] ^= keySchedule[round][j][i];
        }
    }

    return 0;
}

int SubBytes() {

    for (int i = 0; i < 4; i++) {

        for (int j = 0; j < 4; j++) {
            uint8_t x = (matrix[j][i] >> 4);
            uint8_t y = (matrix[j][i] & 0x0f);
            matrix[j][i] = s_box[x][y];
        }

    }
    return 0;
}

int InvSubBytes() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            uint8_t x = (matrix[j][i] >> 4);
            uint8_t y = (matrix[j][i] & 0x0f);
            matrix[j][i] = invs_box[x][y];
        }
    }
    return 0;
}

int ShiftRows() {
    //Row 2
    uint8_t temp;
    temp = matrix[1][0];
    for (int i = 0; i <= 2; i++) {
        matrix[1][i] = matrix[1][i + 1];
    }
    matrix[1][3] = temp;

    //Row 3
    temp = matrix[2][0];
    matrix[2][0] = matrix[2][2];
    matrix[2][2] = temp;
    temp = matrix[2][1];
    matrix[2][1] = matrix[2][3];
    matrix[2][3] = temp;

    //Row 4
    temp = matrix[3][3];
    for (int i = 3; i > 0; i--) {
        uint8_t tmp = matrix[3][i - 1];
        matrix[3][i] = tmp;
    }
    matrix[3][0] = temp;

    return 0;
}

int InvShiftRows(){
    //Row 4
    uint8_t temp;
    temp = matrix[3][0];
    for (int i = 0; i <= 2; i++) {
        matrix[3][i] = matrix[3][i + 1];
    }
    matrix[3][3] = temp;

    //Row 3
    temp = matrix[2][0];
    matrix[2][0] = matrix[2][2];
    matrix[2][2] = temp;
    temp = matrix[2][1];
    matrix[2][1] = matrix[2][3];
    matrix[2][3] = temp;

    //Row 2
    temp = matrix[1][3];
    for (int i = 3; i > 0; i--) {
        uint8_t tmp = matrix[1][i - 1];
        matrix[1][i] = tmp;
    }
    matrix[1][0] = temp;

    return 0;

}

//mixSingleColumn and MixColumns functions were retrieved from: https://github.com/amanske/aes-128/blob/master/aes.cpp

/* Helper function for the mix columns step
   Implementation taken from https://en.wikipedia.org/wiki/Rijndael_mix_columns#Implementation_example */
void mixSingleColumn(unsigned char *r) {
    unsigned char a[4];
    unsigned char b[4];
    unsigned char c;
    unsigned char h;
    /* The array 'a' is simply a copy of the input array 'r'
     * The array 'b' is each element of the array 'a' multiplied by 2
     * in Rijndael's Galois field
     * a[n] ^ b[n] is element n multiplied by 3 in Rijndael's Galois field */
    for (c = 0; c < 4; c++) {
        a[c] = r[c];
        /* h is 0xff if the high bit of r[c] is set, 0 otherwise */
        h = (unsigned char) ((signed char) r[c]>> 7); /* arithmetic right shift, thus shifting in either zeros or ones */
        b[c] = r[c]<< 1; /* implicitly removes high bit because b[c] is an 8-bit char, so we xor by 0x1b and not 0x11b in the next line */
        b[c] ^= 0x1B & h; /* Rijndael's Galois field */
    }
    r[0] = b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1]; /* 2 * a0 + a3 + a2 + 3 * a1 */
    r[1] = b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2]; /* 2 * a1 + a0 + a3 + 3 * a2 */
    r[2] = b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3]; /* 2 * a2 + a1 + a0 + 3 * a3 */
    r[3] = b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0]; /* 2 * a3 + a2 + a1 + 3 * a0 */
}

/* Performs the mix columns step. Theory from: https://en.wikipedia.org/wiki/Advanced_Encryption_Standard#The_MixColumns_step */
void MixColumns() {
    unsigned char temp[4];

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            temp[j] = matrix[j][i]; //place the current state column in temp
        }
        mixSingleColumn(temp); //mix it using the wiki implementation
        for (int j = 0; j < 4; ++j) {
            matrix[j][i] = temp[j]; //when the column is mixed, place it back into the state
        }
    }
}
// xtime, mult and InvMixColumns were adapted from this website: https://www.fourmilab.ch/javascrypt/javascrypt.html
uint8_t xtime(int poly) {
    poly <<= 1;
    return ((poly & 0x100) ? (poly ^ 0x11B) : (poly));
}

uint8_t multiply(uint8_t x, uint8_t y) {
    int bit, result = 0;

    for (bit = 1; bit < 256; bit *= 2, y = xtime(y)) {
        if (x & bit)
            result ^= y;
    }
    return result;
}

void InvMixColumns() {
    unsigned char temp[4];
    for (int j = 0; j < 4; j++) {            // Go through each column...
        for (int i = 0; i < 4; i++) {        // and for each row in the column...
            temp[i] = multiply(matrix[i][j], 0xE) ^multiply(matrix[(i+1)%4][j], 0xB) ^multiply(matrix[(i+2)%4][j], 0xD) ^multiply(matrix[(i+3)%4][j], 9);
        }
        for (int i = 0; i < 4; i++)          // Place result back into column
            matrix[i][j] = temp[i];
    }
}

void printTestSchedule() {
    printf("%X %X %X %X \n", keySchedule[10][0][0], keySchedule[10][0][1], keySchedule[10][0][2],
           keySchedule[10][0][3]);
    printf("%X %X %X %X \n", keySchedule[10][1][0], keySchedule[10][1][1], keySchedule[10][1][2],
           keySchedule[10][1][3]);
    printf("%X %X %X %X \n", keySchedule[10][2][0], keySchedule[10][2][1], keySchedule[10][2][2],
           keySchedule[10][2][3]);
    printf("%X %X %X %X \n\n", keySchedule[10][3][0], keySchedule[10][3][1], keySchedule[10][3][2],
           keySchedule[10][3][3]);
}

void printTest() {
    printf("%X %X %X %X \n", matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]);
    printf("%X %X %X %X \n", matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]);
    printf("%X %X %X %X \n", matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]);
    printf("%X %X %X %X \n\n", matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
}

int encrypt() {

    createKeySchedule();
    AddRoundKey(0);

    for (int i = 1; i < 10; i++) {
        SubBytes();
        ShiftRows();
        MixColumns();
        AddRoundKey(i);
    }

    //Last Round
    SubBytes();
    ShiftRows();
    AddRoundKey(10);

    return 0;
}

int decrypt(){
    createKeySchedule();

    AddRoundKey(10);
    InvShiftRows();
    InvSubBytes();

    for (int i = 9; i > 0; i--) {
        AddRoundKey(i);
        InvMixColumns();
        InvShiftRows();
        InvSubBytes();
    }

    AddRoundKey(0);

    return 0;
}
///Another method to decrypt AES
/*
int decrypt2(){
    createKeySchedule();
    AddRoundKey(10);

    for (int i = 9; i > 0; i--) {
        InvShiftRows();
        InvSubBytes();
        AddRoundKey(i);
        InvMixColumns();
    }

    InvShiftRows();
    InvSubBytes();
    AddRoundKey(0);

    return 0;
}
*/

int main() {
    printTest();
    encrypt();
    printTest();
    decrypt();
    printTest();
    
    return 0;
}