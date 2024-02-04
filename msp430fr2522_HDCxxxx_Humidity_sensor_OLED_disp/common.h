/*
 * common.h
 *
 *  Created on: 2022. 6. 30.
 *      Author: yoch
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>

#define TRUE    1
#define FALSE   0


///////////////////////////////////////////////////////////////////////
// check 0x00 and write the number of 0x00 after the 0x00, other number is written directly.
// ex :
//   0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//-> 0x00, 0x04, 0x1e, 0x00, 0x01, 0x1e, 0x00, 0x03, 0x1e, 0x00, 0x01, 0x1e, 0x00, 0x07
//   0x80, 0x00, 0x90, 0x0e, 0xf0, 0x01, 0x9e, 0x00, 0x90, 0x0e, 0xf0, 0x01, 0x9e, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
//-> 0x80, 0x00, 0x01, 0x90, 0x0e, 0xf0, 0x01, 0x9e, 0x00, 0x01, 0x90, 0x0e, 0xf0, 0x01, 0x9e, 0x00, 0x01, 0x10, 0x00, 0x05
//   0x38, 0x06, 0x7c, 0x0e, 0x4c, 0x0c, 0xfe, 0x1f, 0xcc, 0x0c, 0x9c, 0x0f, 0x98, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//-> 0x38, 0x06, 0x7c, 0x0e, 0x4c, 0x0c, 0xfe, 0x1f, 0xcc, 0x0c, 0x9c, 0x0f, 0x98, 0x07, 0x00, 0x06
//   0x7e, 0x00, 0xfe, 0x0f, 0x00, 0x0f, 0xe0, 0x03, 0xfc, 0x00, 0xfc, 0x00, 0xe0, 0x03, 0x00, 0x0f, 0xfe, 0x0f, 0x7e, 0x00,
//-> 0x7e, 0x00, 0x01, 0xfe, 0x0f, 0x00, 0x01, 0x0f, 0xe0, 0x03, 0xfc, 0x00, 0x01, 0xfc, 0x00, 0x01, 0xe0, 0x03, 0x00, 0x01, 0x0f, 0xfe, 0x0f, 0x7e, 0x00, 0x01,
#define COMPRESS_METHOD_ZERO_ENC_DEC        1       // zero compress method version 1

//
// considered compress method v2, but it is not efficient to make code again, I think it is even with ver1, in my case
//
// assume the pattern is start from 0x00 and patterns are series of {0x00} and {num}
// write number of 0x00 and number of data then data itselves and repeat number of 0x00, ...
// if the patterns are started from number then write 0x00 at first.
//   0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//-> 0x04, 0x01, 0x1e, 0x01, 0x01, 0x1e, 0x03, 0x01, 0x1e, 0x01, 0x01, 0x1e, 0x07
//   0x80, 0x00, 0x90, 0x0e, 0xf0, 0x01, 0x9e, 0x00, 0x90, 0x0e, 0xf0, 0x01, 0x9e, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
//-> 0x00, 0x01, 0x80, 0x01, 0x05, 0x90, 0x0e, 0xf0, 0x01, 0x9e, 0x01, 0x05, 0x90, 0x0e, 0xf0, 0x01, 0x9e, 0x01, 0x01, 0x10, 0x05
//   0x38, 0x06, 0x7c, 0x0e, 0x4c, 0x0c, 0xfe, 0x1f, 0xcc, 0x0c, 0x9c, 0x0f, 0x98, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//-> 0x00, 0x0e, 0x38, 0x06, 0x7c, 0x0e, 0x4c, 0x0c, 0xfe, 0x1f, 0xcc, 0x0c, 0x9c, 0x0f, 0x98, 0x07, 0x06
//   0x7e, 0x00, 0xfe, 0x0f, 0x00, 0x0f, 0xe0, 0x03, 0xfc, 0x00, 0xfc, 0x00, 0xe0, 0x03, 0x00, 0x0f, 0xfe, 0x0f, 0x7e, 0x00,
//-> 0x00, 0x01, 0x7e, 0x01, 0x02, 0xfe, 0x0f, 0x01, 0x04, 0x0f, 0xe0, 0x03, 0xfc, 0x01, 0x01, 0xfc, 0x01, 0x02, 0xe0, 0x03, 0x01, 0x04, 0x0f, 0xfe, 0x0f, 0x7e, 0x01
#define COMPRESS_METHOD_ZERO_ENC_DEC_v2     3       // zero compress method version 2


// from the ver2 theory, make 1st encode data as the number of data, with divided to the upper and lower nibble(4bit).
// upper nibble means num of zero, lower nibble means non-zero data.
// do not write 0x00 in the encode data. just write non-zero data.
// If data array exceed 16(limit of nibble) then just specify 16 then specify again. (ie, 0xf0 means 16x 0x00, and 0xf0, 0x20 means 18x 0x00)
//   0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//-> 0x41, 0x1e, 0x11, 0x1e, 0x31, 0x1e, 0x11, 0x1e, 0x70
//   0x80, 0x00, 0x90, 0x0e, 0xf0, 0x01, 0x9e, 0x00, 0x90, 0x0e, 0xf0, 0x01, 0x9e, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
//-> 0x01, 0x80, 0x15, 0x90, 0x0e, 0xf0, 0x01, 0x9e, 0x15, 0x90, 0x0e, 0xf0, 0x01, 0x9e, 0x11, 0x10, 0x40
//   0x38, 0x06, 0x7c, 0x0e, 0x4c, 0x0c, 0xfe, 0x1f, 0xcc, 0x0c, 0x9c, 0x0f, 0x98, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//-> 0x0e, 0x38, 0x06, 0x7c, 0x0e, 0x4c, 0x0c, 0xfe, 0x1f, 0xcc, 0x0c, 0x9c, 0x0f, 0x98, 0x07, 0x60
//   0x7e, 0x00, 0xfe, 0x0f, 0x00, 0x0f, 0xe0, 0x03, 0xfc, 0x00, 0xfc, 0x00, 0xe0, 0x03, 0x00, 0x0f, 0xfe, 0x0f, 0x7e, 0x00,
//-> 0x01, 0x7e, 0x12, 0xfe, 0x0f, 0x14, 0x0f, 0xe0, 0x03, 0xfc, 0x11, 0xfc, 0x12, 0x03, 0x03, 0x13, 0x0f, 0xf3, 0x0f, 0x7e, 0x10
#define COMPRESS_METHOD_ZERO_ENC_DEC_v3 5       // zero compress method version 2


#define INDEX   1
#define DATA    2




//unsigned long multiply16(unsigned short param1, unsigned short param2);
uint8_t decompress_font_one_char_array( uint8_t compress_method, uint16_t enc_offset, uint8_t *p_enc_data, uint8_t *p_single_dec_data, uint8_t enc_length);

#endif /* COMMON_H_ */
