#pragma once
#include "ftd2xx.h"
class ftdi
{
public:
	int		m_nDeviceCount;
	FT_HANDLE	m_ftHandle;
	DWORD numDevs;
	int GetDeviceCount(void);
	// Open Device - Currently only 1 device can be opened at a time.
	int OpenDevice(int nDevice);
	// Close Current Device
	int CloseDevice(void);
	// Set Device Mode, Pass Zero For Serial Mode or Pass One For Parallel Mode
	// Set Direction Mask Bit To Zero For Input, One For Output
	int SetDeviceMode(unsigned char ucParOneSerZero, unsigned char ucDirectionMask);
	//SetDeviceCharacteriscs
	int SetDataCharacteristics(unsigned char uWordLength,unsigned char uStopBits,unsigned char uParity);
	// Write Byte Stream To Device
	int WriteToDevice(unsigned char * pData , unsigned int unDataLength);
	// Write Byte To Device
	int WriteByteToDevice(unsigned char ucData );
	// Read Data or Data Stream From Device
	int ReadFromDevice(unsigned char * pData, unsigned int & unDataBytesRead);
	// Read Single Byte From Device
	int ReadByteFromDevice( unsigned char * pData );
	// Retrieve Serial Number Of A Connected Device
	int GetDeviceSerialNumber(int nDeviceNumber, char * pSerialNumber, unsigned int unMaxLength);
	// Sets Data Rate, if serial data (bits/sec), if parallel data (16Bytes/sec)
	int SetDeviceDataRate(DWORD dwDataRate);

	ftdi(void);
	~ftdi(void);
};