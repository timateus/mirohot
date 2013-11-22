#include "ftdi.h"
#include "ftd2xx.h"


ftdi::ftdi(void)
{
	m_nDeviceCount = 0;
	m_ftHandle = NULL;
}

ftdi::~ftdi(void)
{
	CloseDevice();
}


int ftdi::GetDeviceCount(void)
{
	FT_STATUS	ftStatus;
	DWORD		dwDeviceCount;

	//Get Device Count
	ftStatus = FT_ListDevices( &dwDeviceCount, NULL, FT_LIST_NUMBER_ONLY );
	if( ftStatus == FT_OK )
	{
		m_nDeviceCount = ( int ) dwDeviceCount;
		return m_nDeviceCount;
	}

	//Error
	return -1;
}

// Open Device - Currently only 1 device can be opened at a time.
int ftdi::OpenDevice(int nDevice)
{
	FT_STATUS	ftStatus;

	//Make Sure We have a valid device number
	if( nDevice > m_nDeviceCount )
		return -1;

	ftStatus = FT_Open( nDevice, &m_ftHandle );
	if( ftStatus == FT_OK )
	{
		return 0;
		
	}

	return -1;
}

// Close Current Device
int ftdi::CloseDevice(void)
{
	FT_STATUS	ftStatus;

	if( m_ftHandle == NULL )
		return -1;

	ftStatus = FT_Close( m_ftHandle );
	if( ftStatus == FT_OK )
	{
		m_ftHandle = NULL;
		return 0;
	}

	return -1;
}

// Set Device Mode, Pass Zero For Serial Mode or Pass One For Parallel Mode
// Set Direction Mask Bit To Zero For Input, One For Output
int ftdi::SetDeviceMode( unsigned char ucParOneSerZero, unsigned char ucDirectionMask)
{
	FT_STATUS	ftStatus;
	
	if( m_ftHandle == NULL )
		return -1;

	ftStatus = FT_SetBitMode( m_ftHandle, ucDirectionMask, ucParOneSerZero );
	if( ftStatus == FT_OK )
	{
		return 0;
	}

	return -1;
}
//SetDataCharacteristics
int ftdi::SetDataCharacteristics(unsigned char uWordLength,unsigned char uStopBits,unsigned char uParity)
{
	FT_STATUS ftStatus;

	if( m_ftHandle == NULL )
		return -1;

	ftStatus=FT_SetDataCharacteristics(m_ftHandle,uWordLength,uStopBits,uParity);
	
	if( ftStatus == FT_OK )
	{
		return 0;
	}

	return -1;
		
 

}
// Write Byte Stream To Device
int ftdi::WriteToDevice(unsigned char * pData , unsigned int unDataLength)
{
	FT_STATUS	ftStatus;
	DWORD		dwBytesWritten = 0;
	
	if( m_ftHandle == NULL )
		return -1;

	ftStatus = FT_Write( m_ftHandle, (LPVOID) pData, (DWORD) unDataLength, &dwBytesWritten );
	if( ftStatus == FT_OK )
	{
		return (int) dwBytesWritten;
	}

	return -1;
}

// Write Byte To Device
int ftdi::WriteByteToDevice(unsigned char ucData )
{
	FT_STATUS	ftStatus;
	DWORD		dwBytesWritten = 0;

	
	if( m_ftHandle == NULL )
		return -1;

	ftStatus = FT_Write( m_ftHandle, (LPVOID) &ucData, 1, &dwBytesWritten );
	if( ftStatus == FT_OK )
	{
		return (int) dwBytesWritten;
	}

	return -1;
}

// Read Data From Device
int ftdi::ReadFromDevice(unsigned char * pData, unsigned int & unDataBytesRead)
{
	FT_STATUS	ftStatus;
	DWORD		dwBytesRead = 0;
	
	if( m_ftHandle == NULL )
		return -1;

	ftStatus = FT_Read( m_ftHandle, (LPVOID) pData, unDataBytesRead, &dwBytesRead );
	if( ftStatus == FT_OK )
	{
		unDataBytesRead = (unsigned int) dwBytesRead;
		return 0;
	}

	return -1;
}

// Read Single Byte From Device
int ftdi::ReadByteFromDevice( unsigned char * pData )
{
	FT_STATUS	ftStatus;
	DWORD		dwBytesRead = 0;
	
	if( m_ftHandle == NULL )
		return -1;

	ftStatus = FT_Read( m_ftHandle, (LPVOID) pData, 1, &dwBytesRead );
	if( ftStatus == FT_OK )
	{		
		return  (int) dwBytesRead;
	}

	return -1;
}

// Retrieve Serial Number Of A Connected Device
int ftdi::GetDeviceSerialNumber(int nDeviceNumber, char * pSerialNumber, unsigned int unMaxLength)
{
	FT_STATUS	ftStatus;
	char		cBuffer[64];
	unsigned int unSerLength;

	//Make Sure We have a valid device number
	if( nDeviceNumber > m_nDeviceCount )
		return -1;

	//Clear Return Buffer
	memset( pSerialNumber, 0x00, unMaxLength );

	//Get Serial Number
	ftStatus = FT_ListDevices( (PVOID) (DWORD) nDeviceNumber, cBuffer, FT_LIST_BY_INDEX | FT_OPEN_BY_SERIAL_NUMBER );
	if( ftStatus == FT_OK )
	{
		unSerLength = (unsigned int) strlen( cBuffer );
		unSerLength = min( unSerLength, unMaxLength-1 );
		memcpy( pSerialNumber, cBuffer, unSerLength );
		return 0;
	}

	return -1;
}

// Sets Data Rate, if serial data (bits/sec), if parallel data (16Bytes/sec)
int ftdi::SetDeviceDataRate(DWORD dwDataRate)
{
	FT_STATUS	ftStatus;

	ftStatus = FT_SetBaudRate( m_ftHandle, dwDataRate );
	if( ftStatus == FT_OK )
	{
		return 0;
	}

	return -1;
}

