#include "nios2_control.h"
#include "leds.h"
#include "timer.h"
#include "button.h"

#define SWITCHES	((volatile unsigned int *)0x10000040)

extern volatile char interuptFlag;
extern unsigned int hours, minutes, seconds,one_sec_flag;
unsigned int counter =0;
extern unsigned int display,printSecs,backSpaces,toggledBtn;


void interrupt_handler(void)
{
	/* read current value in ipending register, using macro */
	unsigned int ipending;
	
	ipending = NIOS2_READ_IPENDING();
	
	
	
	
	if(ipending & 0x1){
		
		//clear timer interupt bit 
		unsigned int timerStatus = *TIMER_STATUS;//get the timer status word
		timerStatus = timerStatus ^ TIMER_TO_BIT;//flip the timer interrupt bit
		*TIMER_STATUS = timerStatus;//write it back into mem/IO
		
		
		counter ++;
		//move leds 
		*LEDS = *LEDS >> 1;
		
		if(*SWITCHES == 0x01){
			//PrintString("switch is high    ");
			display = 1;
			DisplayHoursMinutes();
		}else{
			//PrintString("switch is low    ");
			display =0;
			DisplayHoursMinutes();
		}
		
		
		
		if(counter >=10){//cahnge back to 10 for real
			*LEDS = 0x200;//move leds back
			one_sec_flag =1;
			counter =0;
			seconds++;
			if(seconds >=60){
				seconds =0;
				minutes ++;
				if(minutes >=60){
					minutes= 0;
					hours++;
					if(hours>=24){
						hours =0;
					}
				}
				DisplayHoursMinutes();
			}
			
		}
	}
	
	if(ipending & 0x2){
		//clear timer interupt bit 
		unsigned int buttonStatus = *BUTTON_DATA;//get the button status 
		*BUTTON_EDGE_CAPTURE = 0x0;//set the edgecapture register back to zero
		//*BUTTON_DATA =0x0;//set button back to 0, is this even neccesary/ possible?
		
		//flip led bits
		//*LEDS = *LEDS ^ 0x2; //bitwise XOR on contents of word at LEDS address (led control bits), toggles lowest LED 
		if(printSecs ==1){
			printSecs =0;
			toggledBtn = 1;
		}else{
			printSecs =1;
			//toggledBtn = 1;
		}
		
		
		//interuptFlag = 1;
	}
	
	
	
	return;
}
