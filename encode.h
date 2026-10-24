
#ifndef ENCODE_H
#define ENCODE_H

#include <iostream>
#include <fstream>
#include <string>
#include "common.h"
using namespace std;

struct EncodeInfo
{
    // Source Image info 
    string src_image_fname;
    ifstream src_image_fptr;

    // Secret File Info 
    string secret_fname;
    ifstream secret_fptr;
    string secret_extn;
    int secret_extn_size;
    unsigned long int secret_file_size;

    /* Stego Image Info */
    string output_image_fname;
    ofstream output_image_fptr;
};


// Check operation type 
Opr_type check_operation(char *option);


//Read and validate Encode args from argv 
Status validate_encode_args(char *argv[], EncodeInfo *encInfo);


// Perform the encoding 
Status do_encoding(char *argv[], EncodeInfo *encInfo);


//Get File pointers for i/p and o/p files 
Status open_files(EncodeInfo *encInfo);


//check capacity 
Status check_capacity(EncodeInfo *encInfo);


//Get image size 
unsigned int get_image_size_for_bmp(ifstream &fptr_image);


//Get secret file size 
unsigned int get_file_size(ifstream &fptr);


// Copy bmp image header 
Status copy_bmp_header(EncodeInfo *encInfo);


// Store Magic String 
Status encode_magic_string(const char *magic_string,EncodeInfo *encInfo);


//Encode secret file extension 
Status encode_secret_file_extn(EncodeInfo *encInfo);


//Encode secret file size 
Status encode_secret_file_size(EncodeInfo *encInfo);


// Encode secret file data 
Status encode_secret_file_data(EncodeInfo *encInfo);


//Encode 1 byte into LSB of image data 
Status encode_1byte_to_lsb(char data,unsigned char *buffer_8);


//Encode 4 byte into LSB of image data 
Status encode_4byte_to_lsb(int data,unsigned char *buffer_32);


//Copy remaining image bytes 
Status copy_remaining_img_data(EncodeInfo *encInfo);

#endif

