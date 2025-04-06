#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "stegno_interface.h"
#include "limits.h"
#include "error.h"

int encode_msg(FILE *img_file, FILE *txt_file, char *pass) {
    fseek(img_file, 0, SEEK_SET);  // Start at beginning of image

    size_t pass_len = strlen(pass);
    if (pass_len > 16) {
        printf("Password too long.\n");
        return INVALID_PASS_LEN;
    }

    unsigned char img_byte;

    // --- Step 1: Encode password length using LSBs of 8 bytes ---
    unsigned char pass_len_byte = (unsigned char)pass_len;
    for (int bit = 7; bit >= 0; --bit) {
        if (fread(&img_byte, 1, 1, img_file) != 1) return 1;
        img_byte = (img_byte & 0xFE) | ((pass_len_byte >> bit) & 1);
        fseek(img_file, -1, SEEK_CUR);
        fwrite(&img_byte, 1, 1, img_file);
    }

    // --- Step 2: Encode password string using LSBs ---
    for (size_t i = 0; i < pass_len; ++i) {
        unsigned char ch = pass[i];
        for (int bit = 7; bit >= 0; --bit) {
            if (fread(&img_byte, 1, 1, img_file) != 1) return 1;
            img_byte = (img_byte & 0xFE) | ((ch >> bit) & 1);
            fseek(img_file, -1, SEEK_CUR);
            fwrite(&img_byte, 1, 1, img_file);
        }
    }

    // --- Step 3: Get message length in bytes ---
    fseek(txt_file, 0, SEEK_END);
    long msg_len = ftell(txt_file);
    rewind(txt_file);

    if (msg_len <= 0 || msg_len > 0x7FFFFFFF) {
        printf("Invalid or empty message.\n");
        return 1;
    }

    // --- Step 4: Encode message length (4 bytes = 32 bits) ---
    for (int bit = 31; bit >= 0; --bit) {
        if (fread(&img_byte, 1, 1, img_file) != 1) return 1;
        img_byte = (img_byte & 0xFE) | ((msg_len >> bit) & 1);
        fseek(img_file, -1, SEEK_CUR);
        fwrite(&img_byte, 1, 1, img_file);
    }

    // --- Step 5: Encode message itself ---
    int msg_ch;
    while ((msg_ch = fgetc(txt_file)) != EOF) {
        for (int bit = 7; bit >= 0; --bit) {
            if (fread(&img_byte, 1, 1, img_file) != 1) {
                printf("Image too small to encode message.\n");
                return 1;
            }
            img_byte = (img_byte & 0xFE) | ((msg_ch >> bit) & 1);
            fseek(img_file, -1, SEEK_CUR);
            fwrite(&img_byte, 1, 1, img_file);
        }
    }

    // --- Step 6: Encode "Mikhail" signature at the END of the image ---
    const char *signature = "Mikhail";
    const size_t sig_len = strlen(signature); // 7 chars
    const size_t sig_bits = sig_len * 8;      // 56 bits

    if (fseek(img_file, -((long)sig_bits), SEEK_END) != 0) {
        printf("Failed to seek to end of image for signature.\n");
        return 1;
    }

    for (int i = 0; i < sig_len; ++i) {
        unsigned char ch = signature[i];
        for (int bit = 7; bit >= 0; --bit) {
            if (fread(&img_byte, 1, 1, img_file) != 1) return 1;
            img_byte = (img_byte & 0xFE) | ((ch >> bit) & 1);
            fseek(img_file, -1, SEEK_CUR);
            fwrite(&img_byte, 1, 1, img_file);
        }
    }

    return 0;
}
