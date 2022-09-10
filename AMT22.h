#ifndef AMT22_H
#define AMT22_H

#define AMT22_NOP       0x00
#define AMT22_RESET     0x60
#define AMT22_ZERO      0x70
#define NEWLINE         0x0A
#define TAB             0x09
#define RES12           12
#define RES14           14

#define DEF_SPI_MOSI        11
#define DEF_SPI_MISO        12
#define DEF_SPI_SCLK        13

#include <SPI.h>
class AMT22 
{
	public:
	AMT22(uint8_t CS, uint8_t MOSI = DEF_SPI_MOSI, uint8_t MISO = DEF_SPI_MISO, uint8_t SCLK = DEF_SPI_SCLK);
	~AMT22();

	uint16_t getPosition();
	void resetAMT22();
	void setZeroSPI();
	void setResolution(uint8_t res);

	private:	
	uint8_t	encoder;
	uint8_t resolution = RES14;
	uint8_t spiWriteRead(uint8_t sendByte, uint8_t releaseLine);
	void setCSLine(uint8_t csLine);
};

#endif // AMT22_H