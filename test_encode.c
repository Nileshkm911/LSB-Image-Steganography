#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

/* Check operation type */
OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1], "-e") == 0)
        return e_encode;
    if (strcmp(argv[1], "-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}

int main(int argc,char* argv[])
{
    if (argc>=2)
    {
        EncodeInfo encodeInfo;
        DecodeInfo decodeInfo;
        int ret=check_operation_type(argv);
        if (ret == 0)
        {
            if(argc>=4)
            {
                if (read_and_validate_encode_args(argv,&encodeInfo) == 0)
                {
                    if (do_encoding(&encodeInfo) == 0)
                    {
                            return e_success;
                    }
                    else
                        return e_failure;
                }
            }
            else
            {
                printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> ‹.txt file> [output file]\n");
                return e_failure;
            }
        }
        else if (ret == 1)
        {
            if(argc>=3)
            {
                if (read_and_validate_decode_args(argv,&decodeInfo) == 0)
                {
                    
                    if (do_decoding(&decodeInfo) == 0)
                    {

                        return e_success;
                    }
                    else
                        return e_failure;
                }
            }
            else
            {
                printf("./lsb_steg: Decoding: ./lsb_steg -e <.bmp file> [output file]\n");
                return e_failure;
            }
    
        }
        else
        {
            printf("ERROR! Invalid Operation Type \n");
        }
    }
    else
        printf("INFO: ./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> ‹.txt file> [output file]\nINFO: ./lsb_steg: Decoding: ./lsb_steg -d <.bmp file>[output file]\n");
    
}
