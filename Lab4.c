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
				
	
	
	while(1)
	{
		ADCSRA = ADCSRA | 0b01000000; //start A/D conversion (bit 6)
		
		while((ADCSRA & 0b00010000)==0) {} //wait for conversion to finish
			
		pvalue = ADCH; //keep high bits of 10-bit result (throw away last 2)
			
		if(pvalue < 51) //lowest voltage range 
		{
			PORTC = 0b11111110; //turn LED 0 (PC0) on, rest off 
		}
		else if(pvalue < 102) //2nd lowest voltage range
		{
			PORTC = 0b11111101; //turn LED 1 (PC1) on, rest off 
		}
		else if(pvalue < 153) //middle voltage range
		{
			PORTC = 0b11111011; //turn LED 2 (PC2) on, rest off 
		}
		else if(pvalue < 204) //2nd highest voltage range
		{
			PORTC = 0b11110111; //turn LED 3 (PC3) on, rest off 
		}
		else if(pvalue < 256) //highest voltage range
		{
			PORTC = 0b11101111; //turn LED 4 (PC4) on, rest off 
		}

					
	}
}	
		

		
	

		
