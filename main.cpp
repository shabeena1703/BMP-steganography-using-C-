
#include <iostream>
#include <string>

#include "encode.h"
#include "decode.h"
#include "common.h"

using namespace std;

#define RED       "\033[1;31m"
#define GREEN     "\033[1;32m"
#define YELLOW    "\033[1;33m"
#define MAGENTA   "\033[1;35m"
#define CYAN      "\033[1;36m"
#define WHITE     "\033[1;37m"
#define RESET     "\033[0m"


Opr_type check_operation(char *option)
{
    string operation = option;

    if (operation == "-e")
        return encode;

    else if (operation == "-d")
        return decode;

    else
        return unsupported;
}


int main(int argc, char *argv[])
{
    EncodeInfo encodeInfo;

    if (argc == 1)
    {
        cout << RED << "ERROR: No arguments provided\n" << RESET;
        cout << YELLOW
             << "Usage: ./a.out -e <src.bmp> <secret>\n"
             << RESET;

        return 0;
    }

    Opr_type opr = check_operation(argv[1]);

    if (opr == encode)
    {
        if (argc < 4 || argc > 5)
        {
            cout << RED << "Invalid arguments\n" << RESET;

            cout << YELLOW
                 << "The usage of arguments order should be : "
                 << "./a.out -e <source_file.bmp> secret_file.ext>\n"
                 << RESET;

            return 0;
        }
        else
        {
            cout << CYAN
                 << "YOU SELECTED ENCODE OPERATION\n"
                 << RESET;

            if (validate_encode_args(argv, &encodeInfo) == failure)
            {
                cout << RED << "Validation failed\n" << RESET;
                return 0;
            }
            else
            {
                cout << GREEN << "Validation success\n" << RESET;

                cout << WHITE
                     << "source image file name is "
                     << encodeInfo.src_image_fname
                     << "\n"
                     << RESET;

                cout << WHITE
                     << "Secret file name is "
                     << encodeInfo.secret_fname
                     << "\n"
                     << RESET;

                cout << WHITE
                     << "output file name is "
                     << encodeInfo.output_image_fname
                     << "\n"
                     << RESET;

                do_encoding(argv, &encodeInfo);
            }
        }
    }
    else if (opr == decode)
    {
        DecodeInfo decInfo;

        if (argc < 3 || argc > 4)
        {
            cout << RED
                 << "Invalid arguements\n"
                 << RESET;

            cout << YELLOW
                 << "The arguments order should be : "
                 << "./a.out -d <stego.bmp> <secret>\n"
                 << RESET;

            return 0;
        }
        else
        {
            cout << CYAN
                 << "YOU SELECTED DECODE OPERATION\n"
                 << RESET;

            if (validate_decode_args(argv, &decInfo) == failure)
            {
                cout << RED
                     << "Validation failed\n"
                     << RESET;

                return 0;
            }
            else
            {
                do_decoding(argv, &decInfo);
            }
        }
    }
    else
    {
        cout << RED<< "Invalid arguments other than encode and decode "
        << ".please give the second argument as -e or -d only"<< RESET;
    }

    return 0;
}

