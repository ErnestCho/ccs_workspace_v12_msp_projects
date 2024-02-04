#ifndef PCD8544_H_
#define PCD8544_H_

#define PCD8544_POWERDOWN				0x04
#define PCD8544_ENTRYMODE				0x02
#define PCD8544_EXTENDEDINSTRUCTION		0x01

#define PCD8544_DISPLAYBLANK			0x0
#define PCD8544_DISPLAYNORMAL			0x4
#define PCD8544_DISPLAYALLON			0x1
#define PCD8544_DISPLAYINVERTED			0x5

// H = 0
#define PCD8544_FUNCTIONSET				0x20
#define PCD8544_DISPLAYCONTROL			0x08
#define PCD8544_SETYADDR				0x40
#define PCD8544_SETXADDR				0x80
#define PCD8544_HPIXELS					84      // number of horizontal pixel
#define PCD8544_VBANKS					6       // number of bank (=group of display memory)
#define PCD8544_MAXBYTES				504 // PCD8544_HPIXELS * PCD8544_VBANKS

// H = 1
#define PCD8544_SETTEMP					0x04
#define PCD8544_SETBIAS					0x10
#define PCD8544_SETVOP					0x80

//transform
#define NONE							0x00
#define FLIP_H							0x01
#define FLIP_V							0x02
#define ROTATE							0x04 // 90 deg CW
#define ROTATE_90_CW					ROTATE
#define ROTATE_90_CCW					(FLIP_H | FLIP_V | ROTATE)
#define ROTATE_180						(FLIP_H | FLIP_V)


#endif /*PCD8544_H_*/
