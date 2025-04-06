int validate_txt_file(FILE *txt_file);
int validate_img_file(FILE *img_file, FILE *txt_file);
bool is_img_enc(FILE *img_file);
int encode_msg(FILE *img_file, FILE *txt_file, char *pass);
int decode_msg(FILE *img_file, FILE *txt_file, char *pass);
int validate_decode_pass(FILE *img_file, char *pass);