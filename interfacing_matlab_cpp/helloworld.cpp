#include <mex.h>
#include "ftdi.h"
#include "C:\Users\tnosov\Dropbox\CO-OP2013\not shared\Code\interfacing_matlab_cpp\CDM 2.08.30 WHQL Certified\CDM v2.08.30 WHQL Certified\ftd2xx.h"


ftdi usb;	//initialize an instance of class ftdi

#pragma pack(push)
#pragma pack(1)

struct tPack					
{
	unsigned char start;
	unsigned char ID;
	unsigned char cmd;
	short v;
	short w;
	unsigned char checksum;
	unsigned char end;
};
#pragma pack(pop)

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
int a,b,c;

  
    //Initialize FTDI Connection
    if (((usb.OpenDevice(0)<0)||usb.SetDataCharacteristics(FT_BITS_8,FT_STOP_BITS_1,FT_PARITY_NONE)<0)||(usb.SetDeviceDataRate(FT_BAUD_4800)!=FT_OK))
    {
        mexPrintf("Error initializing device\n");
    }
    else
    {
        mexPrintf("Hello World!\n");
    }
  
usb.ftdi::~ftdi(); //destructor for FTDI

}