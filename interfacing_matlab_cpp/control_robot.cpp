#include <mex.h>
#include "ftdi.h"
#include "C:\Users\tnosov\Dropbox\CO-OP2013\not shared\Code\interfacing_matlab_cpp\CDM 2.08.30 WHQL Certified\CDM v2.08.30 WHQL Certified\ftd2xx.h"


/*
 * Control_robot.cpp - example found in API guide
 *
 * Control_robot(a, b):
 * a - velocity for the left motor
 * b - velocity for the right motor
 *
 * the function drives motors with speed a and b for a second
 * run this command to compile a MEX file (FTD2XX drivers are needed):
 * mex -lFTD2XX -L"C:\Users\tnosov\Downloads\CDM 2.08.30 WHQL Certified (1)\CDM v2.08.30 WHQL Certified\amd64" helloworld.cpp ftdi.cpp
 *
 * This is a MEX-file for MATLAB.
 *
 */


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

/* function to calculate checksum*/
unsigned char CheckSum(unsigned char * p, unsigned char len);

void sendcommand(double y[], double x[], double y2[], double x2[])
{
	//data structure where a mesage to the robot stored
    tPack data; 
    
    //default values:
    int robotID = 0xCB;  // this should be a parameter passed from matlab
    int command = 0x00;  // and this
	int linVel = x[0]; // this is x[0]
	int angVel = x2[0]; // this is x2[0]
	
    //Initialize FTDI Connection
    if (((usb.OpenDevice(0)<0)||usb.SetDataCharacteristics(FT_BITS_8,FT_STOP_BITS_1,FT_PARITY_NONE)<0)||(usb.SetDeviceDataRate(FT_BAUD_4800)!=FT_OK))
    {
        mexPrintf("Error initializing device\n");
    }
    else
    {
        mexPrintf("Hello World!\n");    
    }
    
    //set sending data
    data.start = 0xAA; //this is a start byte
    data.ID = robotID;
    data.cmd = command;
    data.v = linVel;
    data.w = angVel;
    data.checksum = CheckSum((unsigned char *)&data+1, 0x06);
    data.end = 0x55; 
    
    //now send data to transmitter 
    for(int k=0;k<2;k++)
    {
        usb.WriteToDevice((unsigned char*)&data,sizeof(data));
        mexPrintf("i'm sending stuff!\n");
        Sleep(500);
    }    
    
    //reset motors back to zero:
    data.v = 0;
    data.w = 0;
    
    for(int k=0;k<2;k++)
    {    
        usb.WriteToDevice((unsigned char*)&data,sizeof(data));
        mexPrintf("i'm trying to stop!\n");
    }

    
    usb.ftdi::~ftdi(); //destructor for FTDI    
}




void timestwo(double y[], double x[], double y2[], double x2[])
{
  y[0] = 1.0*x[0];
  y2[0] = 1.0*x2[0];
}

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  double *x,*y,*x2,*y2;
  size_t mrows, ncols, mrows2, ncols2;
  
  /* Check for proper number of arguments. */
  if(nrhs!=2) {
    mexErrMsgIdAndTxt( "MATLAB:timestwo:invalidNumInputs",
            "two inputs required.");
  } else if(nlhs>2) {
    mexErrMsgIdAndTxt( "MATLAB:timestwo:maxlhs",
            "Too many output arguments.");
  }
  
  /* The input must be a noncomplex scalar double.*/
  mrows = mxGetM(prhs[0]);
  ncols = mxGetN(prhs[0]);
  if( !mxIsDouble(prhs[0]) || mxIsComplex(prhs[0]) ||
      !(mrows==1 && ncols==1) ) {
    mexErrMsgIdAndTxt( "MATLAB:timestwo:inputNotRealScalarDouble",
            "Input 1 must be a noncomplex scalar double.");
  }
  mrows2 = mxGetM(prhs[1]);
  ncols2 = mxGetN(prhs[1]);
  if( !mxIsDouble(prhs[1]) || mxIsComplex(prhs[1]) ||
      !(mrows2==1 && ncols2==1) ) {
    mexErrMsgIdAndTxt( "MATLAB:timestwo:inputNotRealScalarDouble",
            "Input 2 must be a noncomplex scalar double.");
  }
  
  
  
  /* Create matrix for the return argument. */
  plhs[0] = mxCreateDoubleMatrix((mwSize)mrows, (mwSize)ncols, mxREAL);
  plhs[1] = mxCreateDoubleMatrix((mwSize)mrows2, (mwSize)ncols2, mxREAL);
  
  /* Assign pointers to each input and output. */
  x = mxGetPr(prhs[0]);
  y = mxGetPr(plhs[0]);
  
  x2 = mxGetPr(prhs[1]);
  y2 = mxGetPr(plhs[1]);
  
  /* Call the timestwo subroutine. */
  timestwo(y,x, y2, x2);

  /* Call actual writing to usb subroutine. */
  sendcommand(y,x, y2, x2);
}





unsigned char CheckSum(unsigned char * p, unsigned char len)
{
    unsigned char result = 0x00;
    unsigned char i;
    unsigned char * pointer;
    pointer = p;
    for (i = 0; i < len; i++)
    {
        result ^= *pointer;
        pointer ++;
    }
    return result;
}




