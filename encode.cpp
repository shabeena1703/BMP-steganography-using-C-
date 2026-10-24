
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "encode.h"
//#include "common.h"

using namespace std;


/* Read and validate Encode args from argv */
Status validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    string source_file = argv[2];
    string secret_file = argv[3];

    /* Check source image file */
    if (source_file.find(".bmp") == string::npos)
    {
        cout << RED
             << "ERROR: Source image should be a .bmp file\n"
             << RESET;
        return failure;
    }

    encInfo->src_image_fname = source_file;

    encInfo->src_image_fptr.open(
        encInfo->src_image_fname,
        ios::binary
    );

    if (!encInfo->src_image_fptr)
    {
        cout << RED
             << "ERROR: Unable to open source image file\n"
             << RESET;
        return failure;
    }

    /* Check secret file */
    encInfo->secret_fname = secret_file;

    if (secret_file.find(".txt") == string::npos &&
        secret_file.find(".c") == string::npos &&
        secret_file.find(".cpp") == string::npos)
    {
        cout << RED
             << "ERROR: Secret file should have .txt, .c or .cpp extension\n"
             << RESET;
        return failure;
    }

    encInfo->secret_fptr.open(
        encInfo->secret_fname,
        ios::binary
    );

    if (!encInfo->secret_fptr)
    {
        cout << RED
             << "ERROR: Unable to open secret file\n"
             << RESET;
        return failure;
    }

    /* Get output image name */
    if (argv[4] != nullptr)
    {
        encInfo->output_image_fname = argv[4];

        if (encInfo->output_image_fname.find(".bmp") == string::npos)
        {
            cout << RED
                 << "ERROR: Output image should be a .bmp file\n"
                 << RESET;
            return failure;
        }
    }
    else
    {
        encInfo->output_image_fname = "stego.bmp";
    }

    return success;
}


/* Perform the encoding */
Status do_encoding(char *argv[], EncodeInfo *encInfo)
{
    cout << YELLOW
         << "Opening required files....."
         << RESET << endl;

    if (open_files(encInfo) == failure)
    {
        return failure;
    }

    cout << GREEN << "DONE" << RESET << endl;


    cout << YELLOW
         << "Checking image capacity....."
         << RESET << endl;

    if (check_capacity(encInfo) == failure)
    {
        return failure;
    }

    cout << GREEN << "DONE" << RESET << endl;


    cout << YELLOW
         << "Copying BMP header....."
         << RESET << endl;

    if (copy_bmp_header(encInfo) == failure)
    {
        return failure;
    }

    cout << GREEN << "DONE" << RESET << endl;


    cout << YELLOW
         << "Encoding Magic String....."
         << RESET << endl;

    if (encode_magic_string(MAGIC_STRING, encInfo) == failure)
    {
        return failure;
    }

    cout << GREEN << "DONE" << RESET << endl;


    cout << YELLOW
         << "Encoding Secret File Extension....."
         << RESET << endl;

    if (encode_secret_file_extn(encInfo) == failure)
    {
        return failure;
    }

    cout << GREEN << "DONE" << RESET << endl;


    cout << YELLOW
         << "Encoding Secret File Size....."
         << RESET << endl;

    if (encode_secret_file_size(encInfo) == failure)
    {
        return failure;
    }

    cout << GREEN << "DONE" << RESET << endl;


    cout << YELLOW
         << "Encoding Secret File Data....."
         << RESET << endl;

    if (encode_secret_file_data(encInfo) == failure)
    {
        return failure;
    }

    cout << GREEN << "DONE" << RESET << endl;


    cout << YELLOW
         << "Copying remaining image data....."
         << RESET << endl;

    if (copy_remaining_img_data(encInfo) == failure)
    {
        return failure;
    }

    cout << GREEN << "DONE" << RESET << endl;


    cout << GREEN
         << "ENCODING COMPLETED SUCCESSFULLY"
         << RESET << endl;

    return success;
}


/* Get file pointers for input and output files */
Status open_files(EncodeInfo *encInfo)
{
    /* Source image */
    if (!encInfo->src_image_fptr.is_open())
    {
        encInfo->src_image_fptr.open(
            encInfo->src_image_fname,
            ios::binary
        );
    }

    if (!encInfo->src_image_fptr)
    {
        cout << RED
             << "ERROR: Unable to open source image\n"
             << RESET;
        return failure;
    }

    /* Secret file */
    if (!encInfo->secret_fptr.is_open())
    {
        encInfo->secret_fptr.open(
            encInfo->secret_fname,
            ios::binary
        );
    }

    if (!encInfo->secret_fptr)
    {
        cout << RED
             << "ERROR: Unable to open secret file\n"
             << RESET;
        return failure;
    }

    /* Output image */
    encInfo->output_image_fptr.open(
        encInfo->output_image_fname,
        ios::binary
    );

    if (!encInfo->output_image_fptr)
    {
        cout << RED
             << "ERROR: Unable to create output image\n"
             << RESET;
        return failure;
    }

    return success;
}


/* Check capacity */
Status check_capacity(EncodeInfo *encInfo)
{
    unsigned int image_capacity;

    image_capacity =
        get_image_size_for_bmp(encInfo->src_image_fptr);

    unsigned int secret_file_size =
        get_file_size(encInfo->secret_fptr);

    encInfo->secret_file_size = secret_file_size;

    string extension;

    size_t dot_position =
        encInfo->secret_fname.find_last_of('.');

    if (dot_position != string::npos)
    {
        extension =
            encInfo->secret_fname.substr(dot_position);
    }

    encInfo->secret_extn = extension;

    encInfo->secret_extn_size =
        extension.length();

    unsigned int required_size;

    required_size =
        (strlen(MAGIC_STRING)
         + 4
         + encInfo->secret_extn_size
         + 4
         + encInfo->secret_file_size) * 8;

    if (required_size > image_capacity)
    {
        cout << RED
             << "ERROR: Insufficient image capacity\n"
             << RESET;
        return failure;
    }

    return success;
}


/* Get image size */
unsigned int get_image_size_for_bmp(ifstream &fptr_image)
{
    unsigned int width;
    unsigned int height;

    fptr_image.seekg(18, ios::beg);

    fptr_image.read(
        reinterpret_cast<char *>(&width),
        sizeof(width)
    );

    fptr_image.read(
        reinterpret_cast<char *>(&height),
        sizeof(height)
    );

    cout << CYAN
         << "width = "
         << width
         << RESET << endl;

    cout << CYAN
         << "height = "
         << height
         << RESET << endl;

    fptr_image.seekg(0, ios::beg);

    return width * height * 3;
}


/* Get secret file size */
unsigned int get_file_size(ifstream &fptr)
{
    fptr.seekg(0, ios::end);

    unsigned int size =
        static_cast<unsigned int>(fptr.tellg());

    fptr.seekg(0, ios::beg);

    return size;
}


/* Copy BMP image header */
Status copy_bmp_header(EncodeInfo *encInfo)
{
    char buffer[54];

    encInfo->src_image_fptr.read(
        buffer,
        54
    );

    if (!encInfo->src_image_fptr)
    {
        return failure;
    }

    encInfo->output_image_fptr.write(
        buffer,
        54
    );

    return success;
}


/* Store Magic String */
Status encode_magic_string(
    const char *magic_string,
    EncodeInfo *encInfo
)
{
    for (int i = 0;
         magic_string[i] != '\0';
         i++)
    {
        char buffer[8];

        encInfo->src_image_fptr.read(
            buffer,
            8
        );

        if (!encInfo->src_image_fptr)
        {
            return failure;
        }

        if (encode_1byte_to_lsb(
                magic_string[i],
                reinterpret_cast<unsigned char *>(buffer)
            ) == failure)
        {
            return failure;
        }

        encInfo->output_image_fptr.write(
            buffer,
            8
        );
    }

    return success;
}


/* Encode secret file extension */
Status encode_secret_file_extn(
    EncodeInfo *encInfo
)
{
    unsigned char buffer_32[32];

    encInfo->src_image_fptr.read(
        reinterpret_cast<char *>(buffer_32),
        32
    );

    if (!encInfo->src_image_fptr)
    {
        return failure;
    }

    if (encode_4byte_to_lsb(
            encInfo->secret_extn_size,
            buffer_32
        ) == failure)
    {
        return failure;
    }

    encInfo->output_image_fptr.write(
        reinterpret_cast<char *>(buffer_32),
        32
    );

    for (int i = 0;
         i < encInfo->secret_extn_size;
         i++)
    {
        char buffer[8];

        encInfo->src_image_fptr.read(
            buffer,
            8
        );

        if (!encInfo->src_image_fptr)
        {
            return failure;
        }

        if (encode_1byte_to_lsb(
                encInfo->secret_extn[i],
                reinterpret_cast<unsigned char *>(buffer)
            ) == failure)
        {
            return failure;
        }

        encInfo->output_image_fptr.write(
            buffer,
            8
        );
    }

    return success;
}


/* Encode secret file size */
Status encode_secret_file_size(
    EncodeInfo *encInfo
)
{
    unsigned char buffer_32[32];

    encInfo->src_image_fptr.read(
        reinterpret_cast<char *>(buffer_32),
        32
    );

    if (!encInfo->src_image_fptr)
    {
        return failure;
    }

    if (encode_4byte_to_lsb(
            encInfo->secret_file_size,
            buffer_32
        ) == failure)
    {
        return failure;
    }

    encInfo->output_image_fptr.write(
        reinterpret_cast<char *>(buffer_32),
        32
    );

    return success;
}


/* Encode secret file data */
Status encode_secret_file_data(
    EncodeInfo *encInfo
)
{
    char data;

    while (encInfo->secret_fptr.get(data))
    {
        char buffer[8];

        encInfo->src_image_fptr.read(
            buffer,
            8
        );

        if (!encInfo->src_image_fptr)
        {
            return failure;
        }

        if (encode_1byte_to_lsb(
                data,
                reinterpret_cast<unsigned char *>(buffer)
            ) == failure)
        {
            return failure;
        }

        encInfo->output_image_fptr.write(
            buffer,
            8
        );
    }

    return success;
}


/* Encode 1 byte into LSB of image data */
Status encode_1byte_to_lsb(
    char data,
    unsigned char *buffer_8
)
{
    for (int i = 0;
         i < 8;
         i++)
    {
        buffer_8[i] =
            buffer_8[i] & 0xFE;

        buffer_8[i] =
            buffer_8[i] |
            ((data >> (7 - i)) & 1);
    }

    return success;
}


/* Encode 4 bytes into LSB of image data */
Status encode_4byte_to_lsb(
    int data,
    unsigned char *buffer_32
)
{
    for (int i = 0;
         i < 32;
         i++)
    {
        buffer_32[i] =
            buffer_32[i] & 0xFE;

        buffer_32[i] =
            buffer_32[i] |
            ((data >> (31 - i)) & 1);
    }

    return success;
}


/* Copy remaining image data */
Status copy_remaining_img_data(
    EncodeInfo *encInfo
)
{
    char buffer;

    while (encInfo->src_image_fptr.get(buffer))
    {
        encInfo->output_image_fptr.put(buffer);
    }

    return success;
}



