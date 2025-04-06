#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "stegno_interface.h"
#include "limits.h"
#include "error.h"

int main(int argc, char *argv[]) {

    FILE *img_file;
    FILE *txt_file;
    int pass_len;

    if (argc != 5) {
        return 1;
    }

    if (
        strcmp(argv[1], "E") != 0 &&
        strcmp(argv[1], "e") != 0 &&
        strcmp(argv[1], "D") != 0 &&
        strcmp(argv[1], "d") != 0
    ) {
        printf("select enc or dec\n");
        return INVALID_NUM_ARGS;
    }

    if (strcmp(argv[1], "E") == 0 || strcmp(argv[1], "e") == 0) {
        txt_file = fopen(argv[3], "r");
        if (txt_file == NULL) {
            printf("failed to open/create .txt file: %s\n", argv[3]);
            return TXT_OPEN_FAIL;
        }

        img_file = fopen(argv[2], "r+b");
        if (img_file == NULL) {
            printf("failed to open image file: %s\n", argv[2]);
            fclose(txt_file);  // cleanup
            return IMAGE_OPEN_FAIL;
        }
    } else {
        txt_file = fopen(argv[3], "w");
        if (txt_file == NULL) {
            printf("failed to open .txt file: %s\n", argv[3]);
            return TXT_OPEN_FAIL;
        }

        img_file = fopen(argv[2], "r+b");
        if (img_file == NULL) {
            printf("failed to open image file: %s\n", argv[2]);
            fclose(txt_file);  // cleanup
            return IMAGE_OPEN_FAIL;
        }
    }

    if (strcmp(argv[1], "E") == 0 || strcmp(argv[1], "e") == 0) {
        if (validate_txt_file(txt_file) != 0) {
            printf("txt validation failed\n");
            fclose(img_file);
            fclose(txt_file);
            return TXT_VALIDATION_FAIL;
        }

        if (validate_img_file(img_file, txt_file) != 0) {
            printf("image validation failed\n");
            fclose(img_file);
            fclose(txt_file);
            return IMAGE_VALIDATION_FAIL;
        }
    } else {
        if (is_img_enc(img_file) != true) {
            printf("image provided is not encoded by this programme\n");
            return NON_ENC_IMG;
        }
    }

    pass_len = strlen(argv[4]);
    if (pass_len < MIN_PASS_LEN || pass_len > MAX_PASS_LEN) {
        printf("invalid password lenght\n");
        return INVALID_PASS_LEN;
    }
    if (strcmp(argv[1], "E") == 0 || strcmp(argv[1], "e") == 0) {
        int encode_status = encode_msg(img_file, txt_file, argv[4]);
        if (encode_status != 0) {
            printf("encode failed\n");
            return EXIT_FAILURE;
        } else {
            printf("encoded successfully\n");
            return EXIT_SUCCESS;
        }
    } else {
        int decode_status = decode_msg(img_file, txt_file, argv[4]);
        if (decode_status != 0) {
            printf("decode failed\n");
            return EXIT_FAILURE;
        } else {
            printf("decoded successfully\n");
            return EXIT_SUCCESS;
        }
    }
    

    fclose(img_file);
    fclose(txt_file);
    return 0;
}
