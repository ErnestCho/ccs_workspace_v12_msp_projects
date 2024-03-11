
//WARNING: This Font Require X-GLCD Lib.
//         You can not use it with MikroE GLCD Lib.

//Font Generated by MikroElektronika GLCD Font Creator 1.2.0.0
//MikroElektronika 2011 
//http://www.mikroe.com 

//GLCD FontName : MS_Gothic8x16
//GLCD FontSize : 8 x 16

#if 1
#define INFOMATION_FRAM_BASE        0x1800

uint16_t *MS_Gothic8x16_offset = (uint16_t*) INFOMATION_FRAM_BASE;

#else
#pragma PERSISTENT (MS_Gothic8x16_offset)
#pragma location = 0x1800     // BSL area
//encode data offset array
const uint16_t MS_Gothic8x16_offset[] =
{
 0x000, 0x002, 0x008, 0x00A, 0x00C, 0x00E, 0x01E, 0x02D, 0x032, 0x034,
 0x036, 0x038, 0x047, 0x049, 0x054, 0x059, 0x05B, 0x069, 0x071, 0x07F,
 0x08D, 0x09A, 0x0A8, 0x0B6, 0x0C3, 0x0D1, 0x0DF, 0x0E5, 0x0E7, 0x0E9,
 0x0EB, 0x0ED, 0x0FA, 0x0FC, 0x10B, 0x11B, 0x12B, 0x13B, 0x14B, 0x15A,
 0x16A, 0x17A, 0x182, 0x18F, 0x19F, 0x1AF, 0x1BF, 0x1CF, 0x1DF, 0x1EE,
 0x1FE, 0x20E, 0x21E, 0x22D, 0x23D, 0x24B, 0x25B, 0x26B, 0x27A, 0x28A,
 0x28C, 0x28E, 0x290, 0x292, 0x294, 0x296, 0x2A6, 0x2B6, 0x2C5, 0x2D4,
 0x2E3, 0x2EE, 0x2FD, 0x30C, 0x310, 0x317, 0x327, 0x32B, 0x33B, 0x34A,
 0x359, 0x369, 0x378, 0x383, 0x393, 0x39F, 0x3AF, 0x3BD, 0x3CD, 0x3DD,
 0x3EC, 0x3FC, 0x3FE, 0x400,
};
#endif


//encode data size array
const uint8_t MS_Gothic8x16_data_size[] =
{
 0x02, 0x06, 0x02, 0x02, 0x02, 0x10, 0x0F, 0x05, 0x02, 0x02,
 0x02, 0x0F, 0x02, 0x0B, 0x05, 0x02, 0x0E, 0x08, 0x0E, 0x0E,
 0x0D, 0x0E, 0x0E, 0x0D, 0x0E, 0x0E, 0x06, 0x02, 0x02, 0x02,
 0x02, 0x0D, 0x02, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x0F, 0x10,
 0x10, 0x08, 0x0D, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0F, 0x10,
 0x10, 0x10, 0x0F, 0x10, 0x0E, 0x10, 0x10, 0x0F, 0x10, 0x02,
 0x02, 0x02, 0x02, 0x02, 0x02, 0x10, 0x10, 0x0F, 0x0F, 0x0F,
 0x0B, 0x0F, 0x0F, 0x04, 0x07, 0x10, 0x04, 0x10, 0x0F, 0x0F,
 0x10, 0x0F, 0x0B, 0x10, 0x0C, 0x10, 0x0E, 0x10, 0x10, 0x0F,
 0x10, 0x02, 0x02, 0x02,
};

 uint8_t MS_Gothic8x16[] =
{
 /*size, offset, code,  */ //data...
 /*0x02, 0x000, (00 =  )*/ 0xF0, 0x10,
 /*0x06, 0x002, (01 = !)*/ 0x62, 0xFE, 0x33, 0x11, 0x30, 0x60,
 /*0x02, 0x008, (02 = ")*/ 0xF0, 0x10,
 /*0x02, 0x00A, (03 = #)*/ 0xF0, 0x10,
 /*0x02, 0x00C, (04 = $)*/ 0xF0, 0x10,
 /*0x10, 0x00E, (05 = %)*/ 0x0E, 0x1C, 0x30, 0x22, 0x0C, 0x22, 0x03, 0x9C, 0x1C, 0x60, 0x22, 0x18, 0x22, 0x06, 0x1C, 0x20,
 /*0x0F, 0x01E, (06 = &)*/ 0x19, 0x1E, 0x3C, 0x21, 0xC2, 0x20, 0x42, 0x13, 0x3C, 0x0C, 0x11, 0x13, 0x11, 0x20, 0x20,
 /*0x05, 0x02D, (07 = ')*/ 0x41, 0x0B, 0x11, 0x07, 0x90,
 /*0x02, 0x032, (08 = ()*/ 0xF0, 0x10,
 /*0x02, 0x034, (09 = ))*/ 0xF0, 0x10,
 /*0x02, 0x036, (0A = *)*/ 0xF0, 0x10,
 /*0x0F, 0x038, (0B = +)*/ 0x01, 0x80, 0x11, 0x80, 0x11, 0x80, 0x13, 0xF0, 0x07, 0x80, 0x11, 0x80, 0x11, 0x80, 0x30,
 /*0x02, 0x047, (0C = ,)*/ 0xF0, 0x10,
 /*0x0B, 0x049, (0D = -)*/ 0x21, 0x80, 0x11, 0x80, 0x11, 0x80, 0x11, 0x80, 0x11, 0x80, 0x50,
 /*0x05, 0x054, (0E = .)*/ 0x51, 0x30, 0x11, 0x30, 0x80,
 /*0x02, 0x059, (0F = /)*/ 0xF0, 0x10,
 /*0x0E, 0x05B, (10 = 0)*/ 0x0C, 0xF8, 0x0F, 0x04, 0x10, 0x02, 0x20, 0x02, 0x20, 0x04, 0x10, 0xF8, 0x0F, 0x40,
 /*0x08, 0x069, (11 = 1)*/ 0x21, 0x04, 0x11, 0x04, 0x12, 0xFE, 0x3F, 0x80,
 /*0x0E, 0x071, (12 = 2)*/ 0x0C, 0x18, 0x30, 0x04, 0x28, 0x02, 0x26, 0x02, 0x21, 0xC4, 0x20, 0x38, 0x20, 0x40,
 /*0x0E, 0x07F, (13 = 3)*/ 0x0C, 0x18, 0x0C, 0x04, 0x10, 0x82, 0x20, 0x82, 0x20, 0x44, 0x11, 0x38, 0x0E, 0x40,
 /*0x0D, 0x08D, (14 = 4)*/ 0x19, 0x06, 0x80, 0x05, 0x60, 0x04, 0x18, 0x04, 0xFE, 0x3F, 0x11, 0x04, 0x40,
 /*0x0E, 0x09A, (15 = 5)*/ 0x0C, 0xFE, 0x08, 0x42, 0x10, 0x22, 0x20, 0x22, 0x20, 0x42, 0x10, 0x82, 0x0F, 0x40,
 /*0x0E, 0x0A8, (16 = 6)*/ 0x0C, 0xF8, 0x0F, 0x84, 0x10, 0x42, 0x20, 0x42, 0x20, 0x84, 0x10, 0x18, 0x0F, 0x40,
 /*0x0D, 0x0B6, (17 = 7)*/ 0x01, 0x02, 0x11, 0x02, 0x15, 0x02, 0x3C, 0x82, 0x03, 0x72, 0x11, 0x0E, 0x50,
 /*0x0E, 0x0C3, (18 = 8)*/ 0x0C, 0x38, 0x0E, 0x44, 0x11, 0x82, 0x20, 0x82, 0x20, 0x44, 0x11, 0x38, 0x0E, 0x40,
 /*0x0E, 0x0D1, (19 = 9)*/ 0x0C, 0x78, 0x0C, 0x84, 0x10, 0x02, 0x21, 0x02, 0x21, 0x84, 0x10, 0xF8, 0x0F, 0x40,
 /*0x06, 0x0DF, (1A = :)*/ 0x44, 0x60, 0x18, 0x60, 0x18, 0x80,
 /*0x02, 0x0E5, (1B = ;)*/ 0xF0, 0x10,
 /*0x02, 0x0E7, (1C = <)*/ 0xF0, 0x10,
 /*0x02, 0x0E9, (1D = =)*/ 0xF0, 0x10,
 /*0x02, 0x0EB, (1E = >)*/ 0xF0, 0x10,
 /*0x0D, 0x0ED, (1F = ?)*/ 0x01, 0x18, 0x11, 0x04, 0x15, 0x02, 0x33, 0x82, 0x30, 0x44, 0x11, 0x38, 0x50,
 /*0x02, 0x0FA, (20 = @)*/ 0xF0, 0x10,
 /*0x0F, 0x0FC, (21 = A)*/ 0x1B, 0x38, 0x80, 0x07, 0x78, 0x04, 0x06, 0x04, 0x78, 0x04, 0x80, 0x07, 0x11, 0x38, 0x20,
 /*0x10, 0x10B, (22 = B)*/ 0x0E, 0xFE, 0x3F, 0x82, 0x20, 0x82, 0x20, 0x82, 0x20, 0x82, 0x20, 0x44, 0x11, 0x38, 0x0E, 0x20,
 /*0x10, 0x11B, (23 = C)*/ 0x0E, 0xF8, 0x0F, 0x04, 0x10, 0x02, 0x20, 0x02, 0x20, 0x02, 0x20, 0x04, 0x10, 0x18, 0x0C, 0x20,
 /*0x10, 0x12B, (24 = D)*/ 0x0E, 0xFE, 0x3F, 0x02, 0x20, 0x02, 0x20, 0x02, 0x20, 0x02, 0x20, 0x04, 0x10, 0xF8, 0x0F, 0x20,
 /*0x10, 0x13B, (25 = E)*/ 0x0E, 0xFE, 0x3F, 0x82, 0x20, 0x82, 0x20, 0x82, 0x20, 0x82, 0x20, 0x82, 0x20, 0x02, 0x20, 0x20,
 /*0x0F, 0x14B, (26 = F)*/ 0x03, 0xFE, 0x3F, 0x82, 0x11, 0x82, 0x11, 0x82, 0x11, 0x82, 0x11, 0x82, 0x11, 0x02, 0x30,
 /*0x10, 0x15A, (27 = G)*/ 0x0E, 0xF8, 0x0F, 0x04, 0x10, 0x02, 0x20, 0x02, 0x20, 0x02, 0x21, 0x04, 0x11, 0x18, 0x3F, 0x20,
 /*0x10, 0x16A, (28 = H)*/ 0x03, 0xFE, 0x3F, 0x80, 0x11, 0x80, 0x11, 0x80, 0x11, 0x80, 0x11, 0x80, 0x12, 0xFE, 0x3F, 0x20,
 /*0x08, 0x17A, (29 = I)*/ 0x46, 0x02, 0x20, 0xFE, 0x3F, 0x02, 0x20, 0x60,
 /*0x0D, 0x182, (2A = J)*/ 0x11, 0x0C, 0x11, 0x10, 0x11, 0x20, 0x11, 0x20, 0x13, 0x10, 0xFE, 0x0F, 0x40,
 /*0x10, 0x18F, (2B = K)*/ 0x02, 0xFE, 0x3F, 0x12, 0x01, 0x80, 0x18, 0x60, 0x01, 0x10, 0x06, 0x0C, 0x08, 0x02, 0x30, 0x20,
 /*0x10, 0x19F, (2C = L)*/ 0x02, 0xFE, 0x3F, 0x11, 0x20, 0x11, 0x20, 0x11, 0x20, 0x11, 0x20, 0x11, 0x20, 0x11, 0x20, 0x20,
 /*0x10, 0x1AF, (2D = M)*/ 0x03, 0xFE, 0x3F, 0x78, 0x12, 0x80, 0x07, 0x14, 0x38, 0x80, 0x07, 0x78, 0x12, 0xFE, 0x3F, 0x20,
 /*0x10, 0x1BF, (2E = N)*/ 0x03, 0xFE, 0x3F, 0x18, 0x11, 0x60, 0x12, 0x80, 0x01, 0x11, 0x06, 0x13, 0x18, 0xFE, 0x3F, 0x20,
 /*0x10, 0x1CF, (2F = O)*/ 0x0E, 0xF8, 0x0F, 0x04, 0x10, 0x02, 0x20, 0x02, 0x20, 0x02, 0x20, 0x04, 0x10, 0xF8, 0x0F, 0x20,
 /*0x0F, 0x1DF, (30 = P)*/ 0x03, 0xFE, 0x3F, 0x82, 0x11, 0x82, 0x11, 0x82, 0x11, 0x82, 0x11, 0x44, 0x11, 0x38, 0x30,
 /*0x10, 0x1EE, (31 = Q)*/ 0x0E, 0xF8, 0x0F, 0x04, 0x10, 0x02, 0x20, 0x02, 0x24, 0x02, 0x28, 0x04, 0x10, 0xF8, 0x2F, 0x20,
 /*0x10, 0x1FE, (32 = R)*/ 0x03, 0xFE, 0x3F, 0x82, 0x11, 0x82, 0x11, 0x82, 0x16, 0x82, 0x03, 0x44, 0x0C, 0x38, 0x30, 0x20,
 /*0x10, 0x20E, (33 = S)*/ 0x0E, 0x38, 0x0C, 0x44, 0x10, 0x42, 0x20, 0x82, 0x20, 0x82, 0x20, 0x04, 0x11, 0x18, 0x0E, 0x20,
 /*0x0F, 0x21E, (34 = T)*/ 0x01, 0x02, 0x11, 0x02, 0x11, 0x02, 0x13, 0xFE, 0x3F, 0x02, 0x11, 0x02, 0x11, 0x02, 0x30,
 /*0x10, 0x22D, (35 = U)*/ 0x02, 0xFE, 0x0F, 0x11, 0x10, 0x11, 0x20, 0x11, 0x20, 0x11, 0x20, 0x13, 0x10, 0xFE, 0x0F, 0x20,
 /*0x0E, 0x23D, (36 = V)*/ 0x01, 0x0E, 0x11, 0xF0, 0x21, 0x07, 0x11, 0x38, 0x12, 0x07, 0xF0, 0x11, 0x0E, 0x30,
 /*0x10, 0x24B, (37 = W)*/ 0x02, 0xFE, 0x01, 0x14, 0x3E, 0xE0, 0x01, 0x1E, 0x12, 0xE0, 0x01, 0x13, 0x3E, 0xFE, 0x01, 0x20,
 /*0x10, 0x25B, (38 = X)*/ 0x07, 0x06, 0x30, 0x18, 0x0C, 0x60, 0x03, 0x80, 0x16, 0x60, 0x03, 0x18, 0x0C, 0x06, 0x30, 0x20,
 /*0x0F, 0x26B, (39 = Y)*/ 0x01, 0x06, 0x11, 0x18, 0x11, 0x60, 0x13, 0x80, 0x3F, 0x60, 0x11, 0x18, 0x11, 0x06, 0x30,
 /*0x10, 0x27A, (3A = Z)*/ 0x0E, 0x02, 0x30, 0x02, 0x2C, 0x02, 0x23, 0x82, 0x20, 0x62, 0x20, 0x1A, 0x20, 0x06, 0x20, 0x20,
 /*0x02, 0x28A, (3B = [)*/ 0xF0, 0x10,
 /*0x02, 0x28C, (3C = \)*/ 0xF0, 0x10,
 /*0x02, 0x28E, (3D = ])*/ 0xF0, 0x10,
 /*0x02, 0x290, (3E = ^)*/ 0xF0, 0x10,
 /*0x02, 0x292, (3F = _)*/ 0xF0, 0x10,
 /*0x02, 0x294, (40 = `)*/ 0xF0, 0x10,
 /*0x10, 0x296, (41 = a)*/ 0x0C, 0x80, 0x18, 0x40, 0x24, 0x40, 0x22, 0x40, 0x22, 0x40, 0x12, 0x80, 0x3F, 0x11, 0x20, 0x20,
 /*0x10, 0x2A6, (42 = b)*/ 0x0C, 0xFE, 0x3F, 0x80, 0x10, 0x40, 0x20, 0x40, 0x20, 0x40, 0x20, 0x80, 0x10, 0x11, 0x0F, 0x20,
 /*0x0F, 0x2B6, (43 = c)*/ 0x1D, 0x0F, 0x80, 0x10, 0x40, 0x20, 0x40, 0x20, 0x40, 0x20, 0x40, 0x20, 0x80, 0x10, 0x20,
 /*0x0F, 0x2C5, (44 = d)*/ 0x1D, 0x0F, 0x80, 0x10, 0x40, 0x20, 0x40, 0x20, 0x40, 0x20, 0x80, 0x10, 0xFE, 0x3F, 0x20,
 /*0x0F, 0x2D4, (45 = e)*/ 0x1B, 0x0F, 0x80, 0x12, 0x40, 0x22, 0x40, 0x22, 0x40, 0x22, 0x80, 0x22, 0x11, 0x13, 0x20,
 /*0x0B, 0x2E3, (46 = f)*/ 0x01, 0x40, 0x11, 0x40, 0x13, 0xFC, 0x3F, 0x42, 0x11, 0x42, 0x70,
 /*0x0F, 0x2EE, (47 = g)*/ 0x1D, 0x30, 0x80, 0x4D, 0x40, 0x4A, 0x40, 0x4A, 0x40, 0x4A, 0x80, 0x49, 0x40, 0x30, 0x20,
 /*0x0F, 0x2FD, (48 = h)*/ 0x03, 0xFE, 0x3F, 0x80, 0x11, 0x40, 0x11, 0x40, 0x11, 0x40, 0x11, 0x80, 0x21, 0x3F, 0x20,
 /*0x04, 0x30C, (49 = i)*/ 0x62, 0xCC, 0x3F, 0x80,
 /*0x07, 0x310, (4A = j)*/ 0x31, 0x40, 0x13, 0x40, 0xCC, 0x3F, 0x80,
 /*0x10, 0x317, (4B = k)*/ 0x02, 0xFE, 0x3F, 0x11, 0x04, 0x11, 0x02, 0x15, 0x05, 0x80, 0x08, 0x40, 0x10, 0x11, 0x20, 0x20,
 /*0x04, 0x327, (4C = l)*/ 0x62, 0xFE, 0x3F, 0x80,
 /*0x10, 0x32B, (4D = m)*/ 0x03, 0xC0, 0x3F, 0x80, 0x11, 0x40, 0x13, 0x80, 0x3F, 0x40, 0x11, 0x40, 0x12, 0x80, 0x3F, 0x20,
 /*0x0F, 0x33B, (4E = n)*/ 0x03, 0xC0, 0x3F, 0x80, 0x11, 0x40, 0x11, 0x40, 0x11, 0x40, 0x11, 0x80, 0x21, 0x3F, 0x20,
 /*0x0F, 0x34A, (4F = o)*/ 0x1B, 0x0F, 0x80, 0x10, 0x40, 0x20, 0x40, 0x20, 0x40, 0x20, 0x80, 0x10, 0x11, 0x0F, 0x20,
 /*0x10, 0x359, (50 = p)*/ 0x0C, 0xC0, 0x7F, 0x80, 0x08, 0x40, 0x10, 0x40, 0x10, 0x40, 0x10, 0x80, 0x08, 0x11, 0x07, 0x20,
 /*0x0F, 0x369, (51 = q)*/ 0x1D, 0x07, 0x80, 0x08, 0x40, 0x10, 0x40, 0x10, 0x40, 0x10, 0x80, 0x08, 0xC0, 0x7F, 0x20,
 /*0x0B, 0x378, (52 = r)*/ 0x43, 0xC0, 0x3F, 0x80, 0x11, 0x40, 0x11, 0x40, 0x11, 0x40, 0x30,
 /*0x10, 0x383, (53 = s)*/ 0x0E, 0x80, 0x11, 0x40, 0x22, 0x40, 0x22, 0x40, 0x24, 0x40, 0x24, 0x40, 0x24, 0x80, 0x18, 0x20,
 /*0x0C, 0x393, (54 = t)*/ 0x01, 0x40, 0x11, 0x40, 0x16, 0xFC, 0x1F, 0x40, 0x20, 0x40, 0x20, 0x60,
 /*0x10, 0x39F, (55 = u)*/ 0x02, 0xC0, 0x0F, 0x11, 0x10, 0x11, 0x20, 0x11, 0x20, 0x11, 0x20, 0x13, 0x10, 0xC0, 0x3F, 0x20,
 /*0x0E, 0x3AF, (56 = v)*/ 0x01, 0xC0, 0x21, 0x03, 0x11, 0x0C, 0x11, 0x30, 0x11, 0x0C, 0x12, 0x03, 0xC0, 0x30,
 /*0x10, 0x3BD, (57 = w)*/ 0x02, 0xC0, 0x07, 0x11, 0x38, 0x13, 0x06, 0xC0, 0x01, 0x11, 0x06, 0x13, 0x38, 0xC0, 0x07, 0x20,
 /*0x10, 0x3CD, (58 = x)*/ 0x04, 0x40, 0x20, 0x80, 0x10, 0x11, 0x09, 0x11, 0x06, 0x15, 0x09, 0x80, 0x10, 0x40, 0x20, 0x20,
 /*0x0F, 0x3DD, (59 = y)*/ 0x02, 0xC0, 0x40, 0x11, 0x43, 0x11, 0x2C, 0x11, 0x10, 0x11, 0x0C, 0x12, 0x03, 0xC0, 0x30,
 /*0x10, 0x3EC, (5A = z)*/ 0x0E, 0x40, 0x30, 0x40, 0x28, 0x40, 0x24, 0x40, 0x22, 0x40, 0x21, 0xC0, 0x20, 0x40, 0x20, 0x20,
 /*0x02, 0x3FC, (5B = {)*/ 0xF0, 0x10,
 /*0x02, 0x3FE, (5C = |)*/ 0xF0, 0x10,
 /*0x02, 0x400, (5D = })*/ 0xF0, 0x10,

};
