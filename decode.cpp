
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

#include "decode.h"
//#include "common.h"

using namespace std;


/* Validate decode arguments */
Status validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    decInfo->stego_image_fname = argv[2];

    /* Check stego image extension */
    if (decInfo->stego_image_fname.find(".bmp") == string::npos)
    {
        cout << RED << "ERROR: Stego image should be a .bmp file"
             << RESET << "\n";
        return failure;
    }

    /* Check whether stego image exists */
    decInfo->stego_image_fptr.open(
        decInfo->stego_image_fname, ios::binary);

    if (!decInfo->stego_image_fptr)
    {
        cout << RED << "ERROR: Unable to open stego image"
             << RESET << "\n";
        return failure;
    }

    return success;
}


/* Perform decoding */
Status do_decoding(char *argv[], DecodeInfo *decInfo)
{
    cout << CYAN << "\nOpening required files....." << RESET << "\n";

    if (open_img_file(decInfo) == failure)
    {
        return failure;
    }

    cout << GREEN << "DONE" << RESET << "\n";

    /* Skip BMP header */
    decInfo->stego_image_fptr.seekg(54, ios::beg);

    cout << CYAN << "\nDecoding Magic String....." << RESET << "\n";

    if (decode_magic_string(decInfo) == failure)
    {
        cout << RED << "ERROR: Magic string not found"
             << RESET << "\n";
        return failure;
    }

    cout << GREEN << "Magic string decoded successfully"
         << RESET << "\n";
    cout << GREEN << "DONE" << RESET << "\n";

    cout << CYAN << "\nDecoding Secret File Extension....."
         << RESET << "\n";

    if (decode_secret_file_extn(argv, decInfo) == failure)
    {
        return failure;
    }

    cout << YELLOW << "Secret file name is : "
         << decInfo->secret_fname << RESET << "\n";
    cout << GREEN << "DONE" << RESET << "\n";

    cout << CYAN << "\nDecoding Secret File Size....."
         << RESET << "\n";

    if (decode_secret_file_size(decInfo) == failure)
    {
        return failure;
    }

    cout << YELLOW << "Secret file size = "
         << decInfo->secret_file_size
         << " bytes" << RESET << "\n";
    cout << GREEN << "DONE" << RESET << "\n";

    cout << CYAN << "\nDecoding Secret File Data....."
         << RESET << "\n";

    if (decode_secret_file_data(decInfo) == failure)
    {
        return failure;
    }

    cout << GREEN << "DONE" << RESET << "\n";

    cout << GREEN << "\nDECODING COMPLETED SUCCESSFULLY"
         << RESET << "\n";

    return success;
}


/* Open stego image file */
Status open_img_file(DecodeInfo *decInfo)
{
    if (!decInfo->stego_image_fptr.is_open())
    {
        decInfo->stego_image_fptr.open(
            decInfo->stego_image_fname, ios::binary);
    }

    if (!decInfo->stego_image_fptr)
    {
        cout << RED << "ERROR: Unable to open stego image"
             << RESET << "\n";
        return failure;
    }

    return success;
}


/* Decode magic string */
Status decode_magic_string(DecodeInfo *decInfo)
{
    char magic_string[3] = {0};

    for (int i = 0; i < 2; i++)
    {
        unsigned char buffer[8];

        decInfo->stego_image_fptr.read(
            reinterpret_cast<char *>(buffer), 8);

        if (!decInfo->stego_image_fptr)
        {
            return failure;
        }

        magic_string[i] = decode_1byte_from_lsb(buffer);
    }

    magic_string[2] = '\0';

    if (strcmp(magic_string, MAGIC_STRING) != 0)
    {
        return failure;
    }

    return success;
}


/* Decode secret file extension */
Status decode_secret_file_extn(char *argv[], DecodeInfo *decInfo)
{
    unsigned char buffer_32[32];

    decInfo->stego_image_fptr.read(
        reinterpret_cast<char *>(buffer_32), 32);

    if (!decInfo->stego_image_fptr)
    {
        return failure;
    }

    decInfo->secret_extn_size =
        decode_4bytes_from_lsb(buffer_32);

    decInfo->secret_extn = "";

    for (int i = 0; i < decInfo->secret_extn_size; i++)
    {
        unsigned char buffer[8];

        decInfo->stego_image_fptr.read(
            reinterpret_cast<char *>(buffer), 8);

        if (!decInfo->stego_image_fptr)
        {
            return failure;
        }

        char ch = decode_1byte_from_lsb(buffer);

        decInfo->secret_extn += ch;
    }

    if (argv[3] != nullptr)
    {
        decInfo->secret_fname = argv[3];
    }
    else
    {
        decInfo->secret_fname =
            "decoded" + decInfo->secret_extn;
    }

    return success;
}


/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    unsigned char buffer_32[32];

    decInfo->stego_image_fptr.read(
        reinterpret_cast<char *>(buffer_32), 32);

    if (!decInfo->stego_image_fptr)
    {
        return failure;
    }

    decInfo->secret_file_size =
        decode_4bytes_from_lsb(buffer_32);

    return success;
}


/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    decInfo->secret_fptr.open(
        decInfo->secret_fname, ios::binary);

    if (!decInfo->secret_fptr)
    {
        cout << RED << "ERROR: Unable to create secret file"
             << RESET << "\n";
        return failure;
    }

    for (int i = 0; i < decInfo->secret_file_size; i++)
    {
        unsigned char buffer[8];

        decInfo->stego_image_fptr.read(
            reinterpret_cast<char *>(buffer), 8);

        if (!decInfo->stego_image_fptr)
        {
            return failure;
        }

        char data = decode_1byte_from_lsb(buffer);

        decInfo->secret_fptr.put(data);
    }

    return success;
}


/* Decode 1 byte from LSB */
char decode_1byte_from_lsb(unsigned char *buffer_8)
{
    char data = 0;

    for (int i = 0; i < 8; i++)
    {
        data = (data << 1) | (buffer_8[i] & 1);
    }

    return data;
}


/* Decode 4 bytes from LSB */
int decode_4bytes_from_lsb(unsigned char *buffer_32)
{
    int data = 0;

    for (int i = 0; i < 32; i++)
    {
        data = (data << 1) | (buffer_32[i] & 1);
    }

    return data;
}

