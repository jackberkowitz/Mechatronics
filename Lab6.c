// Jack Berkowitz
// Lab 4, Mechatronics
// 2/22/24 


#include <avr/io.h>

//Global Variables
char pvalue = 0; //value read from potentiometer (0-255 since we'll only read upper 8 bits of the 10-bit number)

int main(void)
{
	//Set Up
	DDRC = 0b00011111; //data register, pins PCO-PC4 set as output for LEDs
									  //pin PC5 set as input for potentiometer
	PORTC = 0b00011111; //setting output of PORTC high to turn off LEDs (assuming wired as active low)
	
	DDRD = 0b01000000; //data register, pin PD6 set as output (PWM OC0A pin)
	
	DDRB = 0b00000110; //data register, setting PB1 & PB2 as output (Motor Direction Pins)
	
	
	//Set Up ADC
	ADMUX = 0b00100101; 
				//left justified (bit 5 = ADLAR = 1)
				//sets ADC channel to PC5 (ADC 5) 		  [bits 0-3]
				//sets voltage reference to AREF (Pin 21) [bits 6-7]
	PRR = 0b00000000;
				//clears PRADC bit (bit 0) to power on ADC
	ADCSRA = 0b10000111;
				//enables ADC (bit 7) 
				//ADC prescaler divider of 128 (bits 0-2)
				//NOTE: bit 4 is flag, can either say conversion done OR
				//we can enable ADC to trigger interrupt
		
	//Set Up Motor Control			
	OCR0A = 0x00; //Load $00 into OCR0 to set initial duty cycle to 0 (motor off)
	TCCR0A = 0b10000011; //Set non-inverting mode on OC0A pin (COMA1:0 = 10)
						 //Fast PWM (WGM1:0 bits = bits 1:0 = 11)
						 //(Note: that we are not affecting OC0B because COMB0:1 bits
						 //stay at default = 00)
	TCCR0B = 0b00000011; //Set base PWM frequency (CS02:0 bits 2-0 = 011 for prescalar
						 //of 64, for approximately 1 KHz base frequency
		//PWM is now running on selected pin at selected base frequency. Duty cycle
		//is set by loading/changing value in OCR0A register
						
				
	
	
	while(1)
	{
		ADCSRA = ADCSRA | 0b01000000; //start A/D conversion (bit 6)
		
		while((ADCSRA & 0b00010000)==0) {} //wait for conversion to finish
			
		pvalue = ADCH; //keep high bits of 10-bit result (throw away last 2)
			
		if(pvalue < 51) //lowest voltage range (-60% to -100% duty cycle)
		{
			PORTC = 0b11111110; //turn LED 0 (PC0) on, rest off 
			PORTB = 0b00000010; //motor reversed
			OCR0A = ((pvalue*2)-255)*-1;
		}
		else if(pvalue < 102) //2nd lowest voltage range (-20% to -60% duty cycle)
		{
			PORTC = 0b11111101; //turn LED 1 (PC1) on, rest off 
			PORTB = 0b00000010; //motor reversed
			OCR0A = ((pvalue*2)-255)*-1;
		}
		else if(pvalue < 128) //middle voltage range (-20% to 0% duty cycle)
		{
			PORTC = 0b11111011; //turn LED 2 (PC2) on, rest off 
			PORTB = 0b00000010; //motor reversed
			OCR0A = ((pvalue*2)-255)*-1;
		}
		else if(pvalue == 128) //middle voltage range (exactly 0% duty cycle)
		{
			PORTC = 0b11111011; //turn LED 2 (PC2) on, rest off
			PORTB = 0b00000000; //motor stopped
			OCR0A = 0;
		}
		else if(pvalue < 153) //middle voltage range (0% to 20% duty cycle)
		{
			PORTC = 0b11111011; //turn LED 2 (PC2) on, rest off 
			PORTB = 0b00000100; //motor forward
			OCR0A = (pvalue*2)-255;
		}
		else if(pvalue < 204) //2nd highest voltage range (20% to 60% duty cycle)
		{
			PORTC = 0b11110111; //turn LED 3 (PC3) on, rest off 
			PORTB = 0b00000100; //motor forward
			OCR0A = (pvalue*2)-255;
		}
		else if(pvalue < 256) //highest voltage range (60% to 100% duty cycle)
		{
			PORTC = 0b11101111; //turn LED 4 (PC4) on, rest off
			PORTB = 0b00000100; //motor forward 
			OCR0A = (pvalue*2)-255;
		}

					
	}
}	
		

		
	

		
