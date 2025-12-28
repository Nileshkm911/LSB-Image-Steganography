/*
Project Title:
LSB Image Steganography using BMP Image

Project Overview:
This project implements image steganography using the Least Significant Bit (LSB)
technique. It hides a secret file inside a BMP image in such a way that the image
appears unchanged to the human eye. The hidden data can later be extracted using
a decoding process.

Basic Theory:
Steganography is the art of hiding information inside another file.
In LSB image steganography, the least significant bit of each image byte is replaced
with bits of secret data. Since the LSB has very little effect on color, the change
is not noticeable.

Why BMP Image is Used:
BMP images store pixel data without compression.
This makes it easy to modify individual bytes safely without data loss.

Overall Working of the Project:
1. The program takes a BMP image and a secret file as input.
2. It checks whether the image has enough capacity to store the secret data.
3. A magic string is first stored to identify that data is hidden.
4. The secret file extension is stored.
5. The secret file size is stored.
6. The actual secret file data is stored byte by byte.
7. The remaining image data is copied as it is.
8. A new stego image is generated containing hidden data.

Encoding Process (What the encoder does):
- Reads BMP image header and pixel data
- Uses bitwise operations to modify LSBs
- Hides metadata (magic string, extension, size)
- Hides secret file content
- Produces a stego image

Decoding Process (What the decoder does):
- Reads stego image
- Checks magic string to confirm hidden data
- Extracts file extension and file size
- Extracts secret file data
- Reconstructs the original secret file

Important Concepts Used in This Project:
- File handling (fopen, fread, fwrite, fseek, ftell)
- Structures to store file information
- Bitwise operations (AND, OR, bit shifting)
- BMP file format understanding
- Least Significant Bit (LSB) manipulation

Conclusion:
This project demonstrates a practical implementation of LSB image steganography.
It provides hands-on understanding of file formats, bit manipulation, and secure
data hiding techniques using the C programming language.
*/


#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

Status open_files(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }
    printf("INFO: Opened beautiful.bmp\n");

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }
    printf("INFO: Openedsecret.txt\n");

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    printf("INFO: Opened steged_img.bmp\n");

    // No failure return e_success
    return e_success;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (argv[2][0] != '.')
    {
        char* str=strstr(argv[2],".bmp");
        if (str!= NULL && strcmp(str,".bmp") == 0)
        {
            encInfo->src_image_fname=argv[2];
        }
        else
        {
            printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> ‹.txt file> [output file]\n");
            return e_failure;
        }
    }
    else
    {
        printf("INFO: ./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> ‹.txt file> [output file]");
        return e_failure;
    }
    
    if (argv[3][0] != '.')
    {
        char* str1=strchr(argv[3],'.');
        if (strcmp(str1,".txt")==0 || strcmp(str1,".c")==0 || strcmp(str1,".h")==0 || strcmp(str1,".sh")==0)
        {
            encInfo->secret_fname=argv[3];
        }
        else
        {
            printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> ‹.txt file> [output file]\n");
            return e_failure;
        }
    }
    else
        return e_failure;

    if (argv[4] == NULL)
    {
        printf("INFO: Output File not mentioned. Creating steged_img.bmp as default\n");
        encInfo -> stego_image_fname = "steged_img.bmp";
    }
    else
    {
        if (argv[4][0] != '.')
        {
            char* str2=strstr(argv[4],".bmp");
                if (str2 != NULL && strcmp(str2,".bmp") == 0)
                {
                    encInfo->stego_image_fname=argv[4];
                }
                else
                {
                    printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> ‹.txt file> [output file]\n");
                    return e_failure;
                }
        }
        else
        {
            printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> ‹.txt file> [output file]\n");
            return e_failure;
        }
    }
    
    return e_success;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

Status check_capacity(EncodeInfo *encInfo)
{
    printf("INFO: ## Encoding Procedure Started ##\n");
    char* ext= strchr(encInfo -> secret_fname,'.');
    strcpy(encInfo->extn_secret_file, ext);
    encInfo->image_capacity=get_image_size_for_bmp(encInfo -> fptr_src_image);
    printf("INFO: Checking for secret.txt size\n");
    encInfo->size_secret_file=get_file_size(encInfo -> fptr_secret);
    printf("INFO: Done. Not Empty\n");
    printf("INFO: Checking for beautiful.bmp capacity to handle secret.txt\n");
    rewind(encInfo->fptr_secret);
    if (encInfo->image_capacity > (54 + ((2+4+(strlen(encInfo->extn_secret_file))+4+encInfo->size_secret_file)*8)))
    {
        printf("INFO: Done. Found Ok\n");
        return e_success;
    }
    else
        return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("INFO: Copying Image Header\n");
    rewind(fptr_src_image);
    unsigned char copy[54];

    fread(copy, 54, 1, fptr_src_image);
    fwrite(copy, 54, 1, fptr_dest_image);

    if (ftell(fptr_src_image) == ftell(fptr_dest_image))
    {
        printf("INFO: Done\n");
        return e_success;
    }
    else
        return e_failure;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i=7; i>=0; i--)
    {
        image_buffer[7-i] = image_buffer[7-i] & (~1) | ((((unsigned)data) >> i) & 1);
    }
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("INFO: Encoding Magic String Signature\n");
    char copy[8];
    for (int i=0; i<strlen(magic_string); i++)
    {
        fread(copy, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], copy);
        fwrite(copy, 8, 1, encInfo->fptr_stego_image);
    }

    if (ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
    {
        printf("INFO: Done\n");
        return e_success;
    }
    else
        return e_failure;
}

Status encode_int_to_lsb(int size, char *image_buffer)
{
    for (int i=31; i>=0; i--)
    {
        image_buffer[31-i] = image_buffer[31-i] & (~1) | ((((unsigned)size) >> i) & 1);
    }
    return e_success;
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char arr[32];
    fread(arr, 32, 1, encInfo->fptr_src_image);
    encode_int_to_lsb(size, arr);
    fwrite(arr, 32, 1, encInfo->fptr_stego_image);

    if (ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
        return e_success;
    else
        return e_failure;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("INFO: Encoding secret.txt File Extension\n");
    unsigned char arr[8];
    for (int i=0; i<strlen(file_extn); i++)
    {
        fread(arr, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], arr);
        fwrite(arr, 8, 1, encInfo->fptr_stego_image);
    }

    if (ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
    {
        printf("INFO: Done\n");
        return e_success;
    }
    else
        return e_failure;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding secret.txt File Size\n");
    char data[32];
    fread(data, 32, 1, encInfo->fptr_src_image);
    encode_int_to_lsb(file_size, data);
    fwrite(data, 32, 1, encInfo->fptr_stego_image);

    if (ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
    {
        printf("INFO: Done\n");
        return e_success;
    }
    else
        return e_failure;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("INFO: Encoding secret.txt File Data\n");
    rewind(encInfo->fptr_secret);

    char data[8];
    for (int i=0; i<encInfo->size_secret_file; i++)
    {
        fread(encInfo->secret_data, 1, 1, encInfo->fptr_secret);
        fread(data, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(encInfo->secret_data[0], data);
        fwrite(data, 8, 1, encInfo->fptr_stego_image);
    }

    if (ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
    {
        printf("INFO: Done\n");
        return e_success;
    }
    else
        return e_failure;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest, EncodeInfo *encInfo)
{
    printf("INFO: Copying Left Over Data\n");
    char arr[1];
    while (fread(arr,1,1,fptr_src))
    {
        fwrite(arr,1,1,fptr_dest);
    }

    if (ftell(fptr_src) == ftell(fptr_dest))
    {
        printf("INFO: Done\n");
        return e_success;
    }
    else
        return e_failure;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_success)
    {
        printf("Done\n");
        if (check_capacity(encInfo) == e_success)
        {
            if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
            {
                if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_success)
                    {
                        if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
                        {
                            if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                            {
                                if (encode_secret_file_data(encInfo) == e_success)
                                {
                                    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo) == e_success)
                                    {
                                        printf("INFO: ## Encoding Done Successfully ##\n");
                                        return e_success;
                                    }   
                                    else
                                    {
                                        printf("ERROR copy_remaining_img_data failure");
                                        return e_failure;
                                    }
                                }
                            }
                            else
                                return e_failure;
                        }
                        else
                            return e_failure;
                    }
                    else
                        return e_failure;
                    
                }
                else
                    return e_failure;
            }
            else
                return e_failure;
        }
        else
            return e_failure;
    }
    else
        return e_failure;
}