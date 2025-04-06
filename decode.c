#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include "stegno_interface.h"
#include "limits.h"
#include "error.h"

int validate_decode_pass(FILE *img_file, char *pass) {
    fseek(img_file, 0, SEEK_SET);  // Go to beginning of the image

    // Step 1: Decode password length from first 8 bytes
    unsigned char img_byte;
    unsigned char pass_len = 0;

    for (int i = 0; i < 8; ++i) {
        if (fread(&img_byte, 1, 1, img_file) != 1) return 0;
        pass_len <<= 1;
        pass_len |= (img_byte & 0x01);
    }

    if (pass_len > 16) {
        printf("Invalid password length in image.\n");
        return INVALID_PASS_LEN;
    }

    // Step 2: Decode password of length 'pass_len'
    char decoded_pass[17] = {0}; // Max 16 chars + null
    for (int i = 0; i < pass_len; ++i) {
        unsigned char ch = 0;
        for (int bit = 0; bit < 8; ++bit) {
            if (fread(&img_byte, 1, 1, img_file) != 1) return 0;
            ch <<= 1;
            ch |= (img_byte & 0x01);
        }
        decoded_pass[i] = ch;
    }

    // Step 3: Compare with provided password
    return (strcmp(decoded_pass, pass) == 0);
}

int decode_msg(FILE *img_file, FILE *txt_file, char *pass) {
    // Step 1: Validate password
    if (!validate_decode_pass(img_file, pass)) {
        printf("Password mismatch or invalid.\n");
        return PASS_MISMATCH;
    }

    // Step 2: Reset pointer and read password length again
    fseek(img_file, 0, SEEK_SET);

    unsigned char img_byte;
    unsigned char pass_len = 0;

    for (int i = 0; i < 8; ++i) {
        fread(&img_byte, 1, 1, img_file);
        pass_len <<= 1;
        pass_len |= (img_byte & 0x01);
    }

    // Step 3: Skip password bytes
    fseek(img_file, pass_len * 8, SEEK_CUR);

    // Step 4: Read the message length (32 bits = 4 bytes)
    uint32_t msg_len = 0;
    for (int i = 0; i < 32; ++i) {
        if (fread(&img_byte, 1, 1, img_file) != 1) {
            printf("Failed to read message length.\n");
            return INVALID_TXT_LEN;
        }
        msg_len <<= 1;
        msg_len |= (img_byte & 0x01);
    }

    printf("Decoded message length: %u bytes\n", msg_len);

    // Step 5: Decode message (msg_len bytes)
    for (uint32_t i = 0; i < msg_len; ++i) {
        unsigned char ch = 0;
        for (int bit = 0; bit < 8; ++bit) {
            if (fread(&img_byte, 1, 1, img_file) != 1) {
                printf("Failed to read message byte.\n");
                return 1;
            }
            ch <<= 1;
            ch |= (img_byte & 0x01);
        }
        fputc(ch, txt_file);
    }

    return 0;
}

