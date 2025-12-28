#include <stdio.h>
#include<string.h>
#include "decode.h"
#include "types.h"
#include "common.h"
char out_name[100];
char new_magic[100];

Status open_decode_image_files(char *file_name,DecodeInfo *decInfo)
{
    
        // Src Image file
        decInfo->fptr_output_image = fopen(file_name, "r");
        // Do Error handling
        if (decInfo->fptr_output_image == NULL)
        {
    	    perror("fopen");
    	    fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_image_fname);

    	    return e_failure;
        }
    printf("INFO: Opened steged_beautiful.bmp\n");    

    return e_success;  
}
Status open_decode_output_file(char* file_name,DecodeInfo* decInfo)
{
    decInfo->fptr_output = fopen(file_name, "w");
        // Do Error handling
        if (decInfo->fptr_output == NULL)
        {
    	    perror("fopen");
    	    fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_fname);

    	    return e_failure;
        }
    if (strcmp(decInfo->output_fname,"output.txt") == 0 ||strcmp(decInfo->output_fname,"output.c") == 0 ||strcmp(decInfo->output_fname,"output.sh") == 0 || strcmp(decInfo->output_fname,"output.h") == 0 )
    {
        printf("INFO: Output File not mentioned. Creating decoded.txt as default\n");
    }
    printf("INFO: Opened Decoded.txt\n");
    return e_success;
}

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (argv[2][0] != '.')
    {
        char*str3=strstr(argv[2],".bmp");
        if (str3!= NULL && strcmp(str3,".bmp") == 0)
        {
            decInfo->output_image_fname=argv[2];
        }
        else
        {
            printf("./lsb_steg: Decoding: ./lsb_steg -e <.bmp file> [output file]\n");
            return e_failure;
        }
    }
    else
    {
        printf("./lsb_steg: Decoding: ./lsb_steg -e <.bmp file> [output file]\n");
        return e_failure;
    }

    if (argv[3] == NULL)
        decInfo -> output_fname = "output";
    else
    {
        decInfo->output_fname = argv[3];
    }

    return e_success;

}

Status skip_bmp_header(FILE *fptr_output_image)
{
    fseek(fptr_output_image,54,SEEK_SET);
    return e_success;
}
Status decode_byte_from_lsb(char *byte_data,char *image_buffer)
{
    *byte_data = 0;
    for (int i=7;i>=0;i--)
    {
        image_buffer[7-i]=(image_buffer[7-i] & 1) << i;
        *byte_data= *byte_data | image_buffer[7-i];
    }
    return e_success;
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    printf("INFO: Decoding Magic String Signature\n");
    char magic[8];
    for (int i=0;i<(strlen(magic_string));i++)
    {
        fread(magic,8,1,decInfo->fptr_output_image);
        decode_byte_from_lsb(new_magic+i,magic);
    }
    if (strcasecmp(new_magic,MAGIC_STRING) == 0)
    {
        printf("INFO: Done\n");
        return e_success;
    }
    else
        return e_failure;
}

Status decode_int_from_lsb(long *size,char *image_buffer)
{
    *size = 0;
    for (int i=31;i>=0;i--)
    {
        image_buffer[31-i]=(image_buffer[31-i] & 1) << i;
        *size= *size | image_buffer[31-i];
    }
    return e_success;
}
Status decode_output_file_extn_size( DecodeInfo *decInfo)
{
    char extn[32];
    fread(extn,32,1,decInfo->fptr_output_image);
    decode_int_from_lsb(&decInfo->output_extn_size,extn);
    return e_success;
}

Status decode_secret_file_extn( DecodeInfo *decInfo)
{
    unsigned char extn1[8];
    int i;
    for (i=0;i<decInfo->output_extn_size;i++)
    {
        fread(extn1,8,1,decInfo->fptr_output_image);
        decode_byte_from_lsb(&decInfo->extn_output_file[i],extn1);
    }
    decInfo->extn_output_file[i]='\0';
    i=0;
    while(decInfo->output_fname[i])
    {
        if(decInfo->output_fname[i]!='.')
        {
            out_name[i]=decInfo->output_fname[i];
        }
        else
            break;
        i++;
    }
    out_name[i] = '\0';
    decInfo->output_fname=strcat(out_name,decInfo->extn_output_file);

    return e_success;
}

Status decode_secret_file_size( DecodeInfo *decInfo)
{
    printf("INFO: Decoding decoded.txt file size\n");
    char file[32];
    fread(file,32,1,decInfo->fptr_output_image);
    decode_int_from_lsb(&decInfo->size_secret_file,file);
    printf("INFo: Done\n");
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    printf("INFO: Decoding decoed.txt File data\n");
    char file1[8];
    for (int i=0;i<decInfo -> size_secret_file;i++)
    {
        fread(file1,8,1,decInfo->fptr_output_image);
        decode_byte_from_lsb(&decInfo->output_data[0],file1);
        fwrite(&decInfo -> output_data[0], 1, 1, decInfo -> fptr_output);
    }
    printf("INFO: Done\n");
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO: ## Decoding Procedure Started ##\n");
    printf("INFO: Opening required Files\n");
    if (open_decode_image_files(decInfo->output_image_fname,decInfo) == 0)
    {
       
        if (skip_bmp_header(decInfo->fptr_output_image) == 0)
        {
            
            if (decode_magic_string(MAGIC_STRING,decInfo) == 0)
            {
                
                if (decode_output_file_extn_size(decInfo) == 0)
                {
                    
                    if (decode_secret_file_extn(decInfo) == 0)
                    {
                        if (open_decode_output_file(decInfo->output_fname,decInfo) == 0)
                        {
                            printf("INFO: Done Opened all required Files\n");

                            if (decode_secret_file_size(decInfo) == 0)
                            {
                                if (decode_secret_file_data(decInfo) == 0)
                                {
                                    printf("INFO: ## Decoding Done Successfully ##\n");
                                    return e_success;
                                }
                                else
                                    return e_failure;
                            }
                            else
                                return e_failure;
                        }
                        else
                        {
                            printf("unccessfull\n");
                            return e_failure;
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
