#include "nios2_control.h"
#include "timer.h"
#include "chario.h"
#include "button.h"
#include "leds.h"

#define SEVEN_SEGMENT_DATA ((volatile unsigned int *)0x10000020)

unsigned int hours, minutes, seconds, one_sec_flag;
static int hex_table[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67};
void DisplayHoursMinutes();
volatile int interuptFlag = 0;
unsigned int display,printSecs,backSpaces,toggledBtn;

void Init(void)
{
	/* initialize software variables */
	hours = 23;
	minutes = 59;
	seconds = 55;

	/* set timer start value for interval of 1/10th (0.1 sec) */
	//half a second at 50mhz is 250000000 cyles in hex is 0x004C 4B40, we set top and bottom half acordingly

	*TIMER_START_LO = 0x4B40;
	*TIMER_SNAP_HI = 0x004C;

	/* clear extraneous timer interrupt */
	*TIMER_STATUS = 0x0;

	/* set ITO, CONT, and START bits of timer control register */
	//thats bits 0,1,2, 0111 = 0x7
	*TIMER_CONTROL = 0x7;

	//set interupt mask on buttons for button 1 on bit 1
	*BUTTON_INTERUPT_MASK = 0x2;

	/* set device-specific bit for timer in Nios II ienable register */
	//i think bit 0 is timer and bit 1 is buttons so
	NIOS2_WRITE_IENABLE(0x3);

	/* set IE bit in Nios II status register */
	NIOS2_WRITE_STATUS(0x1);
	
	DisplayHoursMinutes();
}



int main(void)
{

	/* perform initialization */
	Init();

	PrintString("ELEC371 Lab 4 \n \n \n \n");
	PrintString(" ");

	while (1)
	{
		if(toggledBtn && backSpaces){
			PrintChar('\b');
			PrintChar('\b');
			backSpaces =0;
			toggledBtn =0;
		}
		
		if (one_sec_flag)
		{
			one_sec_flag = 0;
			if(backSpaces){
				PrintChar('\b');
				PrintChar('\b');
				backSpaces =0;
				
			}
			
			if(printSecs){
				int secondsTens = getTensVal(seconds);
				int secondsOnes = getOnesVal(seconds);
				PrintHexDigit(secondsTens);
				PrintHexDigit(secondsOnes);
				backSpaces =1;
			}

		}	

	  
	}

	return 0; /* never reached, but needed to avoid compiler warning */
}

void DisplayHoursMinutes()
{
	volatile unsigned int out;
	if(display == 1){
		//calculate values
		int hoursTens = getTensVal(hours);
		int hoursOnes = getOnesVal(hours);
		int minutesTens = getTensVal(minutes);
		int minutesOnes = getOnesVal(minutes);
		
		//place and shift
		out = (hex_table[hoursTens] << 24) | 
							(hex_table[hoursOnes] << 16) |
							(hex_table[minutesTens] << 8) |
							(hex_table[minutesOnes]);
		/*
		out = hex_table[hoursTens];
		out << 8;
		out = out | hex_table[hoursOnes];
		out << 8;
		out = out | hex_table[minutesTens];
		out << 8;
		out = out | hex_table[minutesOnes];
		*/
	}else{
		out = 0x0;
	}
	*SEVEN_SEGMENT_DATA = out;
}

int getTensVal(int in)
{
	return ((in/10) % 10);
}

int getOnesVal(int in)
{
	return in % 10;
}
