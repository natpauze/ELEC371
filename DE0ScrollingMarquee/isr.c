#include "main.h"
#include "nios2_control.h"
#include "leds.h"
#include "timer.h"
#include "button.h"




//extern unsigned int done_flag,msg_flag,lenght,current_index,scrolling_active,pause_flag;

//extern char msg1[];

void initiateScrolling(char msg_ptr[]);
void continueScrolling();

extern char msg1[];

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
		
		if(scrolling_active ==0 && msg_flag ==1){
			msg_flag =0;
			initiateScrolling(msg1);
		}
		else if (scrolling_active ==1 && pause_flag ==0){
			continueScrolling();
		}
		
	}
	
	if(ipending & 0x2){
		//clear button interupt bit 
		unsigned int buttonStatus = *BUTTON_DATA;//get the button status 
		// *BUTTON_EDGE_CAPTURE = 0x0;//set the edgecapture register back to zero (this works on hardware, not simulator)
		*BUTTON_EDGE_CAPTURE = 0xFFFFFFFF;//set the edgecapture register back to zero by writing ones to all postions (this is needed to work on simulator)
		//*BUTTON_DATA =0x0;//set button back to 0, is this even neccesary/ possible?
		
		
	}
	
	
	
	return;
}
