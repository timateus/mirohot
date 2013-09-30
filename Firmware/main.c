#include <p30fxxxx.h>
#include <uart.h>
#include <delay.h>

#define __dsPIC30F__
#define ROBOTID 0xCA
//#define ROBOTID 0xCB
//-***************************************************************************
//Device Related Configuration
//-***************************************************************************

//-System clock set up
_FOSC(CSW_FSCM_OFF & XT_PLL8);		//according to p30f2010.h, this function sets the system clock
									//from reference manual
									//Fcy = FOSC /4 = 10MHZ * 8/4 = 20MHz
									//see code example CE015 





//-***************************************************************************
//Robot Kinematics Definitions
//-***************************************************************************
#define WHEEL_R			12.75			//radius of the wheel (mm)
#define CHASSIS_L		56.5          	//length from wheel to center of the chassis (mm)     (L)
#define V_MAX         	307.69          //maximum forward speed due to max wheel velocity (mm/s)
#define W_MAX           5.3982          //Maximum angular velocity (rad/s)
#define KRO             4.0             //Kro, gain for calculating velocity
#define KALPHA          8.4             //Kalpha, gain for calculating angular velocity
#define KBETA           -1.0            //Kbeta, gain for calculating angular velocity
#define EPSILON         5.4E-6          //the limit for deltax and deltay
#define SLOPE_POS  		0.8977			//linear gain constant relating PWM to velocity for going forward  (slope_v)
#define V_OFFSET_POS	-538.4347		//v offset calculated from MATLAB approximation for going forward (mm/s)
#define SLOPE_NEG		0.7249			//linear gain constant relating PWM to velocity for going backward  (slope_v) 
#define V_OFFSET_NEG	-296.7359		//v offset calculated from MATLAB approximation for going backward (mm/s)
#define DC_MIN			0
#define DC_MAX			940








typedef enum 
{	
	OFF = 0,
	ON =1
}LED_STATUS; 


typedef struct {
        unsigned int pwm1;              //duty cycle (%) for the left wheel
        unsigned int pwm2;              //duty cycle (%) for the right wheel
}pwm_DC;
pwm_DC pwmset;


//Global Variables
unsigned char ReceivedChar;
volatile unsigned char Rxdata[80];
//char * Receiveddata = Buf;
int i;
int DataAvailable = 0;
int headFlag = 0;
volatile short v = 0;
volatile short w = 0;
volatile float w_float;
int start_flag, ID_flag, cmd_flag, checksum_flag, v_flag, w_short_flag ,w_flag,end_flag ;
char test_char[2];
short test_v;


//-***************************************************
//Function Protype Declaration
//-***************************************************
void HardwareConfig();					//configures the IO pins

int delay(int value);					// delay
void SetLED(int i, LED_STATUS stat);	// turn LEDs ON and OFf
void Blink(int repitition, int i, int j);// Blink LEDs
void toggleLED(int LED);				// change LED states
void initComm();						// set up Baudrate and receive interrupt
void InitMotor();						//initialize motor control pwm
short combine(unsigned char * hi, unsigned char * lo);
float combineW( char * c3, char * c2, char * c1, char * c0);
short CombineShort(unsigned char * C1, unsigned char * C2);
unsigned char CheckSum(unsigned char * p, unsigned char len);
pwm_DC vToPWM(double v, double w);
void SetWheelsSpeed (double vref,double wref) ;


void ShooterHome();						//init servo to normal position
void Shoot();							//shoot 


//-******************************************
//MAIN FUNCTION 
//-******************************************
int main(void)
{
	
	HardwareConfig();					//sets up I/O pins
	initComm();
	InitMotor();
	test_char[0] = 0xFF;
	test_char[1] = 0xBA;
	test_v = CombineShort(&test_char[0],&test_char[1]);
	test_v = 0;
	while(1)
	{
		SetWheelsSpeed (v,w_float) ;
	//	SetWheelsSpeed(200,0);
		if(cmd_flag)
		{
			ShooterHome();
			cmd_flag = 0;
			initComm();
		}				
	}

	return 0;
} // end main





///Port config /////////////////////////////////////////////////////////////////////////////////////
void HardwareConfig() {

	ADPCFG=0xffff;						//ADC Port Configuration register set up
    //OSCCON = 0b00110010; 				//Oscillator control register set up

	//--------------DIGITAL OUTPUTS----------------------------
	//TRISD = port D direction control register
	TRISDbits.TRISD1 = 0;	// LEDl		//port D bit 1 is set for output	
	TRISBbits.TRISB5 = 0; 	// LED2		//port B bit 5 is set for output
    //Digital output printing 
	TRISBbits.TRISB4=0;					
	TRISBbits.TRISB3=0;
	TRISBbits.TRISB2=0;
	TRISBbits.TRISB1=0;
	TRISCbits.TRISC14=0;
	TRISCbits.TRISC13=0;
	TRISEbits.TRISE5=0;
	TRISEbits.TRISE4=0;//LED's 
    
	//TURN ON LEDS
	SetLED(1,ON);
	SetLED(2,ON);	
	
	//Digital ouptut printing 
	//print (0xff);
	/////Motor control/////////////////////////////
	TRISDbits.TRISD0 = 0;	// CE1
	TRISEbits.TRISE8=0;		//CE2
	TRISEbits.TRISE0=0;		//PWM1B		//set pin to output
	TRISEbits.TRISE1=0;		//PWM1A		//set pin to output
	TRISEbits.TRISE2=0;		//PWM2B		//set pin to output
	TRISEbits.TRISE3=0;		//PWM2A		//set pin to output
	PORTDbits.RD0 = 1;		// CE2		
	PORTEbits.RE8=1;		//CE1
	
}




int delay(int value)
{
	long int k= 0;
	for (k=0;k<400000;k++){};

}








void SetLED(int i, LED_STATUS stat)
{
	switch(i)
	{
		case 1:	
			PORTBbits.RB5 = (int)stat;
			break;
		case 2:
			PORTDbits.RD1 = (int)stat;
			break;
		default:
			break;
	}
}


void initComm()
{
	unsigned int baud;
	unsigned int U1MODEvalue;
	unsigned int U1STAvalue;
	baud = 259;			//4.8kbs
	unsigned int UART_TX_RX = 0xFBE7;
 	U1MODEvalue = UART_EN & UART_IDLE_CON & UART_TX_RX &UART_DIS_WAKE & UART_DIS_LOOPBACK & UART_DIS_ABAUD & UART_NO_PAR_8BIT & UART_1STOPBIT;

	U1STAvalue = UART_INT_TX_BUF_EMPTY & UART_TX_PIN_NORMAL & UART_TX_DISABLE & UART_INT_RX_CHAR & UART_ADR_DETECT_DIS & UART_RX_OVERRUN_CLEAR;
	//used to be UART_INT_RX_CHAR
	OpenUART1 (U1MODEvalue, U1STAvalue, baud);
	ConfigIntUART1(UART_RX_INT_EN & UART_RX_INT_PR7 & UART_TX_INT_DIS);
}


/* This is UART1 receive ISR */
void __attribute__((__interrupt__)) _U1RXInterrupt(void)
{
	//unsigned int counter = 0;
 	static unsigned int j;
	unsigned char tmp;
   	IFS0bits.U1RXIF = 0;
   	//while(!DataRdyUART1());
	
	while (DataRdyUART1())
	{
   		Rxdata[j++] = ReadUART1() & 0xFF;
		if (Rxdata[0] != 0xAA)
		{	
			j= 0;
		}
		if (j >8)
		{
			j = 0;
		}	
	}//end while	

//	while (DataRdyUART1())
//	{
//		tmp = ReadUART1() & 0xFF;
//		if(tmp == 0xAA)
//		{
//			j = 0;
//			Rxdata[j] = tmp;
//			
//			break;
//		}			
//	}//end while
//	
//	while(DataRdyUART1())
//	{	
//		j++;
//		Rxdata[j] = ReadUART1() & 0xFF;
//		if (j > 8)
//			j = 0;
//	}
		

	if (Rxdata[0] == 0xAA)
	{	
		start_flag = 1;
		//verified the start byte
		if(Rxdata[1] ==ROBOTID)
		{	
			ID_flag = 1;
			//verified the ID matches with the robot id
			if (Rxdata[7] == CheckSum(&Rxdata[1], 0x06))	//CheckSum(&Rxdata[2], 0x05) modified 9/24
			{	
				checksum_flag = 1;
				//the checksum has been verified. Send data correct
				if (Rxdata[2] == 0x11)		// IN LAST VERSION THIS BIT HAS BEEN CHANGED TO 0X11
				{
					//we shoot
					cmd_flag = 1;
				}
				else
				{
					v = CombineShort(&Rxdata[4], &Rxdata[3]);
					w = CombineShort(&Rxdata[6], &Rxdata[5]);
					w_float = ((float)w)/100.0;
				}
			}
			
		}
		//else we don't care
	}		

	PORTBbits.RB5 = !PORTBbits.RB5;

}  




//-********************************************************
//-initCommunication()
//--------------------
//-This functions sets up the data transfer protocol to be
//-8-bit non parity, no framing, and sets the baud rate to 
//-be the desired rate.  In addition, this function enables
//-the receiver interrupt to receive information.
//-********************************************************
void initCommunication()
{
	U1MODE = 0x0000;		//disable the UART1 and set up communication
	/* The following are set up for UART1 mode of operation by the line above
	U1MODEbits.UARTEN = 0;	//disable UART1
	U1MODEbits.USIDL = 0;	//Continue operation in Idle mode
	U1MODEbits.ALTIO = 0;	//UART communicates using UxTX and UxRX I/O pins
	U1MODEbits.WAKE = 0;	//Wake-up disabled
	U1MODEbits.LPBACK = 0;	//Loopback mode is disabled
	U1MODEbits.ABAUD = 0;	//Input to Capture module from ICx pin
	U1MODEbits.PDSEL = 0b00;//8-bit data, no parity
	U1MODEbits.STSEL = 0;	//1 Stop bit
	*/

	//U1STA =0x0000;
	U1STAbits.UTXEN = 0;	//transmiter disabled.
	U1STAbits.UTXBRK = 0; 	//UxTX pin operates normally
	U1STAbits.URXISEL = 0b00;	//Interrupt flag bit is set when a character is received
	U1STAbits.ADDEN = 0;	//Address Detect mode disabled
	U1STAbits.OERR = 0;		//Receive Buffer Overrun Error Status bit (Read/Clear Only) 1 = overflown

	/*
	U1STAbits.UTXISEL = 0;	//Interrupt when a character is transferred to the Transmit Shift register 
							//(this implies that there is at least one character open in the transmit buffer)
	U1STAbits.UTXBRK = 0; 	//UxTX pin operates normally
	U1STAbits.UTXEN = 0;	//transmiter disabled.
	U1STAbits.UTXBF = 0;	//Transmit Buffer Full Status bit (Read Only)
	U1STAbits.TRMT = 0;		//Transmit Shift Register is Empty bit (Read Only)
	U1STAbits.URXISEL = 0b00;	//Interrupt flag bit is set when a character is received
	U1STAbits.ADDEN = 0;	//Address Detect mode disabled
	U1STAbits.RIDLE = 0;	//Receiver Idle bit (Read Only) 1 = idle
	U1STAbits.PERR = 0;		//Parity Error Status bit (Read Only) 0 = no error
	U1STAbits.FERR = 0;		//Framing Error Status bit (Read Only);	0 = no error
	U1STAbits.OERR = 0;		//Receive Buffer Overrun Error Status bit (Read/Clear Only) 1 = overflown
	U1STAbits.URXDA = 0;	//Receive Buffer Data Available bit (Read Only)
	*/	
	//U1STA = 0x0010;		// 1 is for setting RIDLE, to indicate that there is nothing in Receiver Shift Register RSR
						// should be read only, but to initialize it will avoiding keep going into interrupt
	
	// Set Baud Rate
	//U1BRG = 520;		// set baudrate to 2400 BAUD rate		see p507/772 from dspic30f reference
 	U1BRG = 259;		// set baudrate to 4800 BAUD rate
	//U1BRG = 172;  	// set baudrate to 72100 BAUD rate
	//U1BRG = 129;	 	// set baudrate to 9600 BAUD rate
	                        
   	//Configure receiving interrupts
	IFS0bits.U1RXIF = 0;				/* Clear interrupt flag */
	IFS0bits.INT0IF = 0;				/* clear external interrupt flag*/
    IPC2bits.U1RXIP = 7;				/* Set to highest priority */
	IEC0bits.INT0IE = 0;
    //IEC0 = 0x0200;
	IEC0bits.U1RXIE = 1;				/* Enable receive interrupts */
	//IEC0bits.U1RXIE = 0;				/* Disable receive interrupts */
    U1MODEbits.UARTEN = 1;            	/* Enable UART module 1 */
	

}




void Blink(int repetition, int i, int j)
{	
	int k = 0;
	if (j == 1)
	{
		for (k = 0; k < repetition; k++)
		{
			SetLED(1,ON);
			SetLED(2,ON);
			delay(1);
			SetLED(1,OFF);
			SetLED(2,OFF);
			delay(1);
		}//end for
	}
	else
	{
		for (k = 0; k < repetition; k++)
		{
			SetLED(i,ON);
			
			delay(1);
			SetLED(i,OFF);
			
			delay(1);
		}//end for
	}
}	

void toggleLED(int LED)
{
	switch(LED)
	{
		case 1: PORTBbits.RB5 = !PORTBbits.RB5;
				break;
		case 2: PORTDbits.RD1 = !PORTDbits.RD1;
				break;
		default:break;	
	}
}

short combine( unsigned char * hi, unsigned char * lo)
{
	return (0x0000 | ((*hi)<<4) | (*lo));
}

//float combineW(unsigned char * c3, unsigned char * c2, unsigned char * c1, unsigned char * c0)
//{
//	return(0x00000000 | ((*c3)<<12) | ((*c2)<<8) | ((*c1)<<4) | (*c0));
//}

short CombineShort(unsigned char * C1, unsigned char * C2)
{
 unsigned char Buff[2];
 Buff[0] = *C2;
 Buff[1] = *C1;
 return( *((short *)Buff));
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


//wheel transformation /////////////////////////////////////////////////////////////////////////////////////
pwm_DC vToPWM(double v, double w)
{
	pwm_DC result;
	double dc1, dc2;
	double slope, v_offset;
	
	//both velocity and angular velocity can be negative
	/*IMPORTANT
        for angular velocity, the positive is defined as CCW, which is right
        wheel =>pwm2
        */
	double v_wheel_l;
    double v_wheel_r;

	if (v == 0)
	{
		if (w == 0)
		{
			result.pwm1 = 550;
			result.pwm2 = 550;
			return result;
		}
		else
		{
			//v == 0 and w != 0, here we rotate the robot around its center
			v_wheel_l = -w*CHASSIS_L/WHEEL_R;  //from general kinematics equation
			v_wheel_r = w*CHASSIS_L/WHEEL_R;  //from general kinematics equation

		}
	}
	else
	{
		v_wheel_l = v/WHEEL_R - w*CHASSIS_L/WHEEL_R;  //from general kinematics equation
		v_wheel_r = v/WHEEL_R + w*CHASSIS_L/WHEEL_R;  //from general kinematics equation
	}
        
	if (v > 0)
	{
		dc1 = 1/(SLOPE_POS/WHEEL_R) *(v_wheel_l - V_OFFSET_POS/WHEEL_R);
		dc2 = 1/(SLOPE_POS/WHEEL_R) *(v_wheel_r - V_OFFSET_POS/WHEEL_R);
	}
	else
	{
		if (v < 0)
		{
			dc1 = 1/(SLOPE_NEG/WHEEL_R) *(v_wheel_l - V_OFFSET_NEG/WHEEL_R);
			dc2 = 1/(SLOPE_NEG/WHEEL_R) *(v_wheel_r - V_OFFSET_NEG/WHEEL_R);
		}
		else
		{
			//v == 0
			if (w > 0)
			{
				dc1 = 1/(SLOPE_NEG/WHEEL_R) *(v_wheel_l - V_OFFSET_NEG/WHEEL_R);
				dc2 = 1/(SLOPE_POS/WHEEL_R) *(v_wheel_r - V_OFFSET_POS/WHEEL_R);
			}
			else
			{
				dc1 = 1/(SLOPE_POS/WHEEL_R) *(v_wheel_l - V_OFFSET_POS/WHEEL_R);
				dc2 = 1/(SLOPE_NEG/WHEEL_R) *(v_wheel_r - V_OFFSET_NEG/WHEEL_R);
			}

		}
	}
	
	
    
	dc1 = (int)dc1;
	dc2 = (int)dc2;

	if (dc1 < DC_MIN)
	{
		dc1 = DC_MIN;
	}
	if (dc2 < DC_MIN)
	{
		dc2 = DC_MIN;
	}

	if (dc1 > DC_MAX)
	{
		dc1 = DC_MAX;
	}

	if (dc2 > DC_MAX)
	{
		dc2 = DC_MAX;
	}


	
	
  
	result.pwm1 =  dc1;//*0x14;//0x147;//*0x14;
	result.pwm2 =  dc2;//*0x14;//0x147;//*0x14;
	
	return result;
}


void SetWheelsSpeed (double vref,double wref) 
{
	pwmset=vToPWM( vref, wref);
	SetDCMCPWM(1,pwmset.pwm1,0);
	SetDCMCPWM(2,pwmset.pwm2,0);
}

void InitMotor()
{
	PTCONbits.PTMOD =  0b10;	//set the motor control pwm in up/down counting mode
	//PTCONbits.PTMOD =  0b00;	//set the motor control pwm in free running mode
	PTCONbits.PTCKPS = 0b00;// PTMR prescale 1:1
	PWMCON1=0x0033;		//enable only PWM2H 2L 1L 1H for output, in the complementary output mode	
	PTPER = 0x01F3;			//20kHz up/down counting mode
	SetWheelsSpeed(0,0);
	PTCONbits.PTEN = 1;		//Enable Time base
}



//Servoshooter funcions///////////////////////////////////////////////////////////////////////////////////////
void ShooterHome()
{
	long int i;
	PTCONbits.PTEN = 0;
	PTCONbits.PTMOD = 0b10;	//in up/down counting mode
	PTCONbits.PTCKPS = 0b11;// PTMR prescale  1:64
	PTCONbits.PTOPS = 0x0;  //post scale 1:1
	PWMCON1=0x0440;			//enable only PWM3H for output, in the independent output mode
	
	PTPER = 0x0C34;			//50Hz up/down mode	
	//SetDCMCPWM(3,0x0200, 0);
	SetDCMCPWM(3,0x0165,0);//224
	//SetDCMCPWM(3,0x0224,0);//224
	PTCONbits.PTEN=1;
	for ( i=0;i<1000000;i++);
	SetDCMCPWM(3,0x0200, 0);
	for ( i=0;i<1000000;i++);
	
	PTCONbits.PTEN=0;

	InitMotor();	//Pass Control to Wheels
	
}




void Shoot()
{
	long int j;
	PTCONbits.PTEN=1;
	PWMCON1=0x0440;						//Set the PEN3H to 1, PWM3 is enabled
	PTPER = 0x0C34;			//50Hz up/down mode
	SetDCMCPWM(3,0x0165,0);			//1.75ms for 50Hz up/down counting
	PTCONbits.PTEN=1;
	for ( j=0;j<4000000;j++);
	SetDCMCPWM(3,0x0200, 0);	//1.25ms for 50Hz up down mode
	for ( j=0;j<4000000;j++);

	//ptconbits.pten=0;
	//ptper=0x7fff;
	//PWMCON1=0x0033;
	//PTCONbits.PTEN = 1;		//Enable Time base
}