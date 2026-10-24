
#ifndef DECODE_H
#define DECODE_H

#include <iostream>
#include <fstream>
#include <string>

#include "common.h"

using namespace std;

struct DecodeInfo
{
    /* Stego Image info */
    string stego_image_fname;

    /* Secret File info */
    string secret_fname;
    string secret_extn;

    ifstream stego_image_fptr;
    ofstream secret_fptr;

    int secret_extn_size;
    int secret_file_size;
};


/* Validate decode arguments */
Status validate_decode_args(char *argv[],DecodeInfo *decInfo);


/* Perform decoding */
Status do_decoding(char *argv[],DecodeInfo *decInfo);


/* Open stego image file */
Status open_img_file(DecodeInfo *decInfo);


/* Decode magic string */
Status decode_magic_string(DecodeInfo *decInfo);


/* Decode secret file extension */
Status decode_secret_file_extn(char *argv[],DecodeInfo *decInfo);


/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);


/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);


/* Decode 1 byte from LSB */
char decode_1byte_from_lsb(unsigned char *buffer_8);


/* Decode 4 bytes from LSB */
int decode_4bytes_from_lsb(unsigned char *buffer_32);

#endif

