// Jack Berkowitz
// Lab 9, Mechatronics
// 4/1/24 


#include <avr/io.h>

//Initializing Functions
void wait(volatile int);
int sendMAX7221(unsigned char, unsigned char);

int main(void)
{
	DDRB = 0b00101100; //Set pins SCK, MOSI, and SS as output
	SPCR = 0b01010001; //enable the SPI, set to main mode 0, SCK = Fosc/16, lead with MSB
	
	sendMAX7221(0b00001001, 0b00000000); //set decoding mode turned off for all
	sendMAX7221(0b00001011, 0b00000010); //setting scan limit to 2
	sendMAX7221(0b00001100, 0b00000001); //turn on display
	
	while(1)
	{	
		//month
		sendMAX7221(0b00000001, 0b01111110); //digit 0, manually setting number 0
		sendMAX7221(0b00000010, 0b00110011); //digit 1, manually setting number 4
		wait(500);
		
		//day
		sendMAX7221(0b00000001, 0b01111110); //digit 0, manually setting number 0
		sendMAX7221(0b00000010, 0b01111001); //digit 1, manually setting number 3
		wait(500);
		
		//blank display
		sendMAX7221(0b00000001, 0b00000000); //digit 0, manually setting blank
		sendMAX7221(0b00000010, 0b00000000); //digit 1, manually setting blank
		wait(2000);
	}
}	

int sendMAX7221(unsigned char command, unsigned char data)
{
	PORTB = PORTB & 0b11111011; //clear PB2, SS bit, so transmission can start
	
	SPDR = command; //Send command
	while(!(SPSR & (1<<SPIF))); //Wait for transmission to finish
	SPDR = data; //Send data
	while(!(SPSR & (1<<SPIF))); //Wait for transmission to finish
	
	PORTB = PORTB | 0b00000100; //Set PB2 to 1, SS bit, to end transmission
	
	return 0;
}
void wait(volatile int multiple)
{
	while(multiple > 0)
	{
		TCCR0A = 0x00; //clear WMG00 and WGM01 registers to ensure timer/counter in normal mode
		TCNT0 = 0; //preload value testing on count = 250
		TCCR0B = 0b00000011; //Start TIMER0, normal mode, crystal clock, prescaler = 64
		while(TCNT0 < 0xFA); //exits loop when count = 250 (requires preload of 0 to make count = 250)
		TCCR0B = 0x00; //Stop TIMER0
		multiple--;
	}
}
		
	

		
