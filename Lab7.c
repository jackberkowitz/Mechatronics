// Jack Berkowitz
// Lab 7, Mechatronics
// 3/7/24 

//FUNCTIONS CODE COPIED FROM DR. CLARK ON GITHUB

#include <avr/io.h>

//Initialize Functions
void step_CW(void);
void step_CCW(void);
void wait(volatile int multiple, volatile char time_choice);
void delay_T_msec_timer0(char choice);

//** GLOBAL VARIABLES **
int phase_step = 1; // We will use wave stepping, so the steps are numbered 1-4, then will repeat.  

int main(void)
{
	//Set Up
	DDRC = 0b00000000; //data register, pin PCO set as input for switch
	
	DDRB = 0b00000011; //data register, pin PB0-PB1 set as output for LEDs
	PORTB = 0b00000011; //setting output of PORTB high to turn off LEDs (assuming wired as active low)
	
	DDRD = 0b11110000; //data register, pins PD4-PD7 set as output for stepper motor control
   
   while(1)
   {
		if(!(PINC & 0b00000001)) //If Switch PCO is pressed
		{
			for(int i=1;i<=36;i++) //CW 270 degrees (270/7.5 step = 36)
			{
				step_CW(); //clockwise step
				wait(55,2); //delay so motor doesn't skip steps
				PORTB = 0b11111110; //turn LED 0 turn (PB0)
			}
			
			wait(500,2); //pause for half a second
			
			for(int k=1;k<=12;k++) //CCW 90 degrees (90/7.5 step = 12)
			{
				step_CCW(); //counterclockwise step
				wait(83,2); //delay so motor doesn't skip steps
				PORTB = 0b11111101; //turn LED 1 on (PB1)
			}
			
			wait(500,2); //pause for half a second
			
			for(int j=1;j<=24;j++) //CW 180 degrees (180/7.5 step = 24)
			{
				step_CW(); //clockwise step
				wait(83,2); //delay so motor doesn't skip steps
				PORTB = 0b11111110; //turn LED 0 turn (PB0)
			}
	
		}
	}
}

void step_CCW()
{
	// This function advances the motor counter-clockwise one step.  Follow the full-wave stepping table in Stepper Motor Control.ppt for MEMS 1049 course.
	// phase1a = PORTD_7, phase1b = PORTD_6, phase2a = PORTD_5, phase2b = PORTD_4

	switch (phase_step) {
		case 1:
		// step to 2
		PORTD = 0b00010000;
		phase_step = 2;
		break;
		case 2:
		// step to 3
		PORTD = 0b01000000;
		phase_step = 3;
		break;
		case 3:
		// step to 4;
		PORTD = 0b00100000;
		phase_step = 4;
		break;
		case 4:
		// step to 1;
		PORTD = 0b10000000;
		phase_step = 1;
	break;  }
}// end step_CCW

void step_CW() {
	// This function advances the motor clockwise one step.  Follow the full-wave stepping table in Stepper Motor Control.ppt for MEMS 1049 course. Our stepper motor phases are assigned to Port pins as follows:
	// phase1a = PORTD_7, phase1b = PORTD_6, phase2a = PORTD_5, phase2b = PORTD_4

	switch (phase_step) {
		case 1:
		// step to 4
		PORTD = 0b00100000;
		phase_step = 4;
		break;
		case 2:
		// step to 1
		PORTD = 0b10000000;
		phase_step = 1;
		break;
		case 3:
		// step to 2;
		PORTD = 0b00010000;
		phase_step = 2;
		break;
		case 4:
		// step to 3;
		PORTD = 0b01000000;
		phase_step = 3;
	break;  }
}// end step_CW

void wait(volatile int multiple, volatile char time_choice) {
	/* This subroutine calls others to create a delay.
		 Total delay = multiple*T, where T is in msec and is the delay created by the called function.
	
		Inputs: multiple = number of multiples to delay, where multiple is the number of times an actual delay loop is called.
		Outputs: None
	*/
	
	while (multiple > 0) {
		delay_T_msec_timer0(time_choice); 
		multiple--;
	}
} // end wait()

void delay_T_msec_timer0(volatile char choice) {
    //*** delay T ms **
    /* This subroutine creates a delay of T msec using TIMER0 with prescaler on clock, where, for a 16MHz clock:
    		for Choice = 1: T = 0.125 msec for prescaler set to 8 and count of 250 (preload counter with 5)
    		for Choice = 2: T = 1 msec for prescaler set to 64 and count of 250 (preload counter with 5)
    		for Choice = 3: T = 4 msec for prescaler set to 256 and count of 250 (preload counter with 5)
    		for Choice = 4: T = 16 msec for prescaler set to 1,024 and count of 250 (preload counter with 5)
			for Choice = Default: T = .0156 msec for no prescaler and count of 250 (preload counter with 5)
	
			Inputs: None
			Outputs: None
	*/
	
	TCCR0A = 0x00; // clears WGM00 and WGM01 (bits 0 and 1) to ensure Timer/Counter is in normal mode.
	TCNT0 = 0;  // preload value for testing on count = 250
	// preload value for alternate test on while loop: TCNT0 = 5;  // preload load TIMER0  (count must reach 255-5 = 250)
	
	switch ( choice ) { // choose prescaler
		case 1:
			TCCR0B = 0b00000010; //1<<CS01;	TCCR0B = 0x02; // Start TIMER0, Normal mode, crystal clock, prescaler = 8
		break;
		case 2:
			TCCR0B =  0b00000011; //1<<CS01 | 1<<CS00;	TCCR0B = 0x03;  // Start TIMER0, Normal mode, crystal clock, prescaler = 64
		break;
		case 3:
			TCCR0B = 0b00000100; //1<<CS02;	TCCR0B = 0x04; // Start TIMER0, Normal mode, crystal clock, prescaler = 256
		break; 
		case 4:
			TCCR0B = 0b00000101; //1<<CS02 | 1<<CS00; TCCR0B = 0x05; // Start TIMER0, Normal mode, crystal clock, prescaler = 1024
		break;
		default:
			TCCR0B = 0b00000001; //1<<CS00; TCCR0B = 0x01; Start TIMER0, Normal mode, crystal clock, no prescaler
		break;
	}
	
	while (TCNT0 < 0xFA); // exits when count = 250 (requires preload of 0 to make count = 250)
	// alternate test on while loop: while ((TIFR0 & (0x1<<TOV0)) == 0); // wait for TOV0 to roll over:
	// How does this while loop work?? See notes
	
	TCCR0B = 0x00; // Stop TIMER0
	//TIFR0 = 0x1<<TOV0;  // Alternate while loop: Clear TOV0 (note that this is a nonintuitive bit in that it is cleared by writing a 1 to it)
	
} // end delay_T_msec_timer0()
		

		
	

		

