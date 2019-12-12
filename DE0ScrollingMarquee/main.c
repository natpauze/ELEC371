/*

DE0 Scrolling Marquee 

Nathaniel Pauze 

This code is based on psudo code discused in class, based on specifcations provided by the instructor 


*/

#include "main.h"
#include "nios2_control.h"
#include "timer.h"
#include "chario.h"
#include "button.h"
#include "leds.h"
#include "7-Segment-ASCII_HEX.h"

#define SWITCHES	((volatile unsigned int *)0x10000040)
#define SEVEN_SEGMENT_DATA ((volatile unsigned int *)0x10000020)


//hex table for converting from a number to a digit on the hex display
static int hex_table[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67};
//refrence to the 7 segment display table in 7-Segment-ASCII_HEX.c that I found online 
//extern int SevenSegmentASCII[96];
//buffer for storing the current message.
char output_buffer[64];
//messages for testing 
char msg1[] = {'d','E','0','\0'};




unsigned int done_flag,msg_flag,lenght,current_index,scrolling_active,pause_flag;

void initiateScrolling(char msg_ptr[]);
void continueScrolling();

void Init(void)
{
	done_flag =1;//so we can imediatly start the next one 
	msg_flag = 0;
	lenght =0;
	current_index =0;
	scrolling_active =0;
	pause_flag =0;

	//read bottom 2 switches 
	int switches = *SWITCHES | 0x3;
	/*set timer acording to specifications:
		Device timer runs ar 50mhz
		11 = 0.25 s = 0x00BE BC20
		10 = 0.5 s  = 0x017D 7840
		01 = 1 s    = 0x02FA F080‬
		00 = 2 s    = 0x05F5 E100

		Set top and bottom half of timer device
	 */
	if(switches == 0x3){
		*TIMER_SNAP_HI = 0x00BE;
		*TIMER_START_LO = 0xBC20;
	}else if(switches == 0x2){
		*TIMER_SNAP_HI = 0x00BE;
		*TIMER_START_LO = 0xBC20;
	}
	else if(switches == 0x1){
		*TIMER_SNAP_HI = 0x02FA;
		*TIMER_START_LO = 0xF080;
	}
	else {
		*TIMER_SNAP_HI = 0x05F5;
		*TIMER_START_LO = 0xE100;
	}

	/* clear extraneous timer interrupt */
	*TIMER_STATUS = 0x0;

	/* set ITO, CONT, and START bits of timer control register 
	thats bits 0,1,2, 0111 = 0x7
	ITO enables interupts, CONT makes it loop, START makes it run
	*/
	*TIMER_CONTROL = 0x7;

	//set interupt mask on buttons for button 1 on bit 1
	*BUTTON_INTERUPT_MASK = 0x2;

	/* set device-specific bits for timer an buttons in Nios II ienable register 
	This makes the processor accept interupts from these sources.
	bit 0 is timer and bit 1 is buttons*/
	NIOS2_WRITE_IENABLE(0x3);

	/* set IE bit in Nios II status register, this enables interupts overall*/
	NIOS2_WRITE_STATUS(0x1);
	
}



int main(void)
{

	/* perform initialization */
	Init();

	PrintString("Scolling Markee exemple");
	PrintString("\n");
	int x;
	while (1)
	{
		if(done_flag == 1 && msg_flag ==1){
			PrintString("done");

		}

	  
	}

	return 0; /* never reached, but needed to avoid compiler warning */
}

void UpdateHexDisplays( int index)
{
	volatile unsigned int out;
	//place and shift
	out = 	(SevenSegmentASCII[output_buffer[index+0]] << 24) | 
			(SevenSegmentASCII[output_buffer[index+1]] << 16) |
			(SevenSegmentASCII[output_buffer[index+2]] << 8) |
			(SevenSegmentASCII[output_buffer[index+3]]);
		
	*SEVEN_SEGMENT_DATA = out;
}
/*Starts the scrolling for a new message
This isnt exaclty how i would implement this but i followed the pseudo code
*/
void initiateScrolling(char msg_ptr[]){
	//TODO
	msg_ptr = msg1;

	output_buffer[0] =' ';
	output_buffer[1] =' ';
	output_buffer[2] =' ';
	output_buffer[3] =' ';
	int j =4;
	int i =0;
	while (msg_ptr[i] != 0){
		output_buffer[j] = msg_ptr[i];
		j++;
		i++;
	}
	output_buffer[j] =' ';
	output_buffer[j+1] =' ';
	output_buffer[j+2] =' ';
	output_buffer[j+3] =' ';
	lenght = j+3;
	current_index =0;
	scrolling_active =1;
	return;
}

void continueScrolling(){
	current_index++;
	if (current_index  < lenght -3)
	{
		UpdateHexDisplays(current_index);
	}else{
		scrolling_active =0;
		done_flag =1;
	}
	
}

