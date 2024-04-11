// Jack Berkowitz
// Lab 3, Mechatronics
// 2/5/24 


#include <avr/io.h>
#include <avr/interrupt.h>

//Initializing Functions
void wait(volatile int);

//Global Variables
int flag = 0;
int flag2 = 0;
int counter = 0;
int counter2 = 0;

int main(void)
{
	DDRD = 0b00000000; //data register, all Port D pins set as inputs (includes PD2 & PD3 set as input for interrupts)
	DDRC = 0b00111111; //data register, pins PCO-PC5 set as output for LEDs
	DDRB = 0b00000110; //data register, pins PB1 & PB2 set as output for LEDs
	PORTC = 0b00111111; //setting output of PORTC high to turn off LEDs (assuming wired as active low)
	PORTB = 0b00000110; //setting output of PORTB high to turn off LEDs (assuming wired as active low)
	
	
	//Set Up Interrupts
	EICRA = 0b00001010; //Trigger INT0 & INT1 on Falling Edge
	EIMSK = 0b00000011; //Enable INTO & INT1
	sei(); //Enable Global Interrupt
	
	
	while(1)
	{
		if (flag == 0) //if LED 6 is off
		{
			
			PORTC = PORTC & 0b11111110; //Turn on LED 0, PCO
			wait(200);
			PORTC = PORTC | 0b00000001; //Turn off LED 0, PC0
		
			PORTC = PORTC & 0b11111101; //Turn on LED 1, PC1
			wait(200);
			PORTC = PORTC | 0b00000010; //Turn off LED 1, PC1
		
			PORTC = PORTC & 0b11111011; //Turn on LED 2, PC2
			wait(200);
			PORTC = PORTC | 0b00000100; //Turn off LED 2, PC2
		}	
		
		PORTC = PORTC & 0b11110111; //Turn on LED 3, PC3
		wait(200);
		PORTC = PORTC | 0b00001000; //Turn off LED 3, PC3
		
		PORTC = PORTC & 0b11101111; //Turn on LED 4, PC4
		wait(200);
		PORTC = PORTC | 0b00010000; //Turn off LED 4, PC4
		
		PORTC = PORTC & 0b11011111; //Turn on LED 5, PC5
		wait(200);
		PORTC = PORTC | 0b00100000; //Turn off LED 5, PC5
		
	}
}	
		
//ISR, INT0
ISR(INT0_vect) //entered if Switch 0 is turned on
{
	wait(50); //switch debounce
	
	if(counter == 0) //seeing if switch is being pressed for first time
	{
		if(!(PORTC & 0b00000100)) //seeing if is LED 2 on
		{
			PORTB = PORTB & 0b11111101; //Turn on LED 6, PB1
			PORTC = PORTC | 0b00000100; //Turn off LED 2, PC2
			
			flag++;
			flag2++;
			counter++;
		}
	}
	
	EIFR = 0b00000001;	//clear the INT0 flag	
}

//ISR, INT1
ISR(INT1_vect) //entered if Switch 1 is turned on
{
	wait(50); //switch debounce
	
	if(flag2 == 1 && counter2 == 0) //seeing if LED 6 is on, 1st time switch being pressed
	{
			
		if(!(PORTC & 0b00100000)) //seeing if LED 5 is on
		{
			counter2++;
				
			PORTB = PORTB & 0b11111011; //Turn on LED 7, PB2
			wait(3000);
			PORTB = PORTB | 0b00000110; //Turn off LEDs 6-7, PB1-PB2
			PORTC = PORTC | 0b00111111; //Turn off LEDs 0-5, PC0-PC5
			
			flag = 0;
			flag2 = 0;
			counter = 0;
			counter2 = 0;
				
		}
	}
	
	EIFR = 0b00000010;	//Clear the INT1 flag
}

void wait(volatile int multiple)
{
	while(multiple > 0)
	{
		TCCR0A = 0x00;
		TCNT0 = 0;
		TCCR0B = 0b00000011;
		while(TCNT0 < 0xFA);
		TCCR0B = 0x00;
		multiple--;
	}
}
		
	

		
