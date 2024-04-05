/*
 * common.c
 *
 *  Created on: 2022. 11. 9.
 *      Author: yoch
 */

#include "common.h"

#if 0
unsigned long multiply16(unsigned short param1, unsigned short param2)
{
    unsigned short cnt;
    unsigned long res = 0;
    unsigned long _param1 = (unsigned long) param1;
    unsigned long _param2 = (unsigned long) param2;

    if(_param1 != 0 && _param2 != 0)
    {
        for(cnt=16; cnt>0; cnt--)
        {
            if(_param2 & 0x01)
            {
                res += _param1;
            }
            _param1 <<= 1;
            _param2 >>= 1;
        }

    }
    else
        res = 0;

    return res;
}
#endif


uint8_t decompress_font_one_char_array( uint8_t compress_method, uint16_t enc_offset, uint8_t *p_enc_data, uint8_t *p_single_dec_data, uint8_t enc_length)
{
    uint8_t i, j;
    uint8_t l_dec_length = 0;
    uint8_t l_zero_cnt = 0;
    uint8_t l_non_zero_cnt = 0;
    uint8_t index_or_data = INDEX;

    p_enc_data += enc_offset;                   // go to the encode data address with added the offset

    if(compress_method == COMPRESS_METHOD_ZERO_ENC_DEC_v3)
    {
        for(i=0; i<enc_length; i++)
        {
            if(index_or_data == INDEX)
            {
                l_zero_cnt = *p_enc_data;
                l_non_zero_cnt = *p_enc_data++;
                l_zero_cnt = (l_zero_cnt & 0xF0) >> 4;
                l_non_zero_cnt = (l_non_zero_cnt & 0x0F);

                for(j=0; j<l_zero_cnt; j++)
                {
                    *p_single_dec_data++ = 0x00;
                    l_dec_length++;
                }
                if(l_non_zero_cnt != 0)
                {
                    index_or_data = DATA;

                }
            }
            else  // DATA
            {
                *p_single_dec_data++ = *p_enc_data++;
                l_dec_length++;
                l_non_zero_cnt--;
                if( l_non_zero_cnt == 0 )
                {
                    index_or_data = INDEX;
                }
            }

        }

    }
    else if(compress_method == COMPRESS_METHOD_ZERO_ENC_DEC)
    {
        for(i=0; i<enc_length; i++)
        {
            if(*p_enc_data == 0x00)
            {
                for(j=0; j< *(p_enc_data+1); j++)
                {
                    *p_single_dec_data++ = 0x00;
                    l_dec_length++;
                }
                p_enc_data += 2;
                i++;
            }
            else
            {
                *p_single_dec_data++ = *p_enc_data++;
                l_dec_length++;
            }
        }
    }

    return l_dec_length;
}
