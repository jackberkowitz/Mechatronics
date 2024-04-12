// Jack Berkowitz
// Lab 10, Mechatronics
// 4/11/24 

 //**Functional i2c Code copied from Dr. Clark

#include <avr/io.h>

/** Function Declarations ***/
void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_repeated_start(void);
void i2c_write_to_address(unsigned char address);
void i2c_read_from_address(unsigned char address);
void i2c_write_data(unsigned char data);
unsigned char i2c_read_data(unsigned char ack);

void wait(volatile int);
int sendMAX7221(unsigned char, unsigned char);

int main() {
	
	//variables needed:
	int temperatureC;
	int temperatureF;
	int temp0;
	int temp1;
	unsigned char UpperByte;
	unsigned char LowerByte;
	
	DDRB = 0b00101100; //Set pins SCK, MOSI, and SS as output
	SPCR = 0b01010001; //enable the SPI, set to main mode 0, SCK = Fosc/16, lead with MSB
	
	sendMAX7221(0b00001001, 0b11111111); //set decoding mode on (all bits)
	sendMAX7221(0b00001010, 0b00000111); //set intensity of light
	sendMAX7221(0b00001011, 0b00000010); //setting scan limit to 2
	sendMAX7221(0b00001100, 0b00000001); //turn on display
	
	unsigned char data;  
	
	// I2C (TWI) Setup
	//If needed, turn on TWI power: PRR = PRR & 0b01111111; // Ensure that TWI is powered on (PRTWI = 0)
	DDRC = 0b00000000; // Define all PORTC bits as input (specifically PC4 and PC5)
	PORTC = PORTB | 0b00110000;  // set internal pull-up resistors on SCL and SDA lines (PC5 and PC4) ofr I2C bus
	i2c_init();
	
	// I2C Write data as Main. Writing to Resolution Register of MCP9808 Temperature Sensor
	data = 0b00000000; // Sending Resolution of 0.5 to Resolution Register
	i2c_start();
	i2c_write_to_address(0b00011000); //Address the MCP9808 Chip
	i2c_write_data(0x08);  // Address 0x08: Resolution Register
	i2c_write_data(data);
	i2c_stop();
	
	while(1)
	{	
		//I2C Read data as Main. Reading from Ambient Temperature Register of MCP9808 Temperature Sensor
		i2c_start(); 
		i2c_write_to_address(0b00011000); //Address the MCP9808 Chip
		i2c_write_data(0x05);  // Address 0x05: Writing to Ambient Temperature Register
		i2c_repeated_start();
		i2c_read_from_address(0b00011000); //Address the MCP9808 Chip again
		UpperByte = i2c_read_data(1);  // read with ACK
		LowerByte = i2c_read_data(0);  // read with NO_ACK
		i2c_stop();
		
		//Convert UpperByte & LowerByte to Temp in Celsius
		UpperByte = UpperByte & 0x1F; //Clear upper three flag bits
		if((UpperByte & 0x10) == 0x10) //TA < 0 C
		{
			UpperByte = UpperByte & 0x0F; //Clear SIGN bit
			temperatureC = 256 - (UpperByte*16+LowerByte); //Convert reading into temperature in C
		}
		else
		{
			temperatureC = (UpperByte*16+LowerByte); //Convert reading into temperature in C
		}	
		
		//Convert to Temp in Fahrenheit
		temperatureF = temperatureC*(9/5)+32;
		
		temp0 = temperatureF/10%10; //tens digit of resulting temp
		temp1 = temperatureF%10; //ones digit of resulting temp
		
		sendMAX7221(0b00000001,temp0); //digit 0
		sendMAX7221(0b00000010,temp1); //digit 1
		
		wait(1000); //wait 1 second between intervals
	}
	
}

// Note that for coding efficiencies, some of the following functions can be combined, for example to write to or read from an secondary using a single function call.  
// This would eliminate the need for some of the while(test TWINT bit) loops that occur back-to-back when subsequent functions are called.

void i2c_init(void) { // initialize i2c
	TWSR = 0b00000000; // prescaler is set to 1
	TWBR = 72; // Put 72 into TWBR to define SCL frequency as 100kHz for 16MHz oscillator
	TWCR = 0b00000100; // TWEN = 1 (enable TWI)
}

void i2c_start(void) { // send start command
	//while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000));   // wait for idle condition -- TWINT bit must be high to proceed -- not needed if single main is used
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);  //TWCR | 0b10100100;       // initiate START condition -- write 1 to TWINT to clear it and initiate action, set TWSTA bit, set TWEN bit
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000));   // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != START) // error checking -- need to predefine START = 0x08 and ERROR() function.
		// ERROR();
}

void i2c_stop(void) { // send stop command
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO); //TWCR = TWCR | 0b10010100;       // initiate STOP condition -- write 1 to TWINT to clear it and initiate action, set TWSTO bit and set TWEN bit
}

void i2c_repeated_start(void) {
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);  //TWCR | 0b10100100;       // initiate START condition -- write 1 to TWINT to clear it and initiate action, set TWSTA bit, set TWEN bit
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000));   // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != START) // error checking -- need to predefine START = 0x10 and ERROR() function.
	// ERROR();
}

void i2c_write_to_address( unsigned char address) { //Write an address byte to the I2C2 bus in form of SLA_W (address to write to)
	unsigned char SLA_W = address<<1; // create SLA_W byte by shifting address and leaving R/W bit clear
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for idle condition -- TWINT bit must be high to proceed
	TWDR = SLA_W;       // Load TWDR with address plus R/W bit
	TWCR = (1<<TWINT) | (1<<TWEN); //TWCR = TWCR | 0b10000100;       // initiate Write -- write 1 to TWINT to clear it and initiate action, and set TWEN
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != MT_SLA_ACK) // error checking -- need to predefine MT_SLA_ACK and ERROR() function depending on possible outcomes: 0x18, 0x20, or 0x38.
		//ERROR();	
}

void i2c_read_from_address(unsigned char address) { //Write an address byte to the I2C bus in form of SLA_R (address to read from)
	unsigned char SLA_R = address<<1 | 1; // create SLA_R byte by shifting address and setting R/W bit
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for idle condition -- TWINT bit must be high to proceed
	TWDR = SLA_R;       // Load TWDR with address plus R/W bit
	TWCR = (1<<TWINT) | (1<<TWEN); //TWCR = TWCR | 0b10000100;       // initiate Write -- write 1 to TWINT to clear it and initiate action, and set TWEN
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != MR_SLA_ACK) // error checking -- need to predefine MR_SLA_ACK and ERROR() function depending on possible outcomes: 0x38, 0x40, or 0x48.
		//ERROR();
}

void i2c_write_data( unsigned char data) { //Write data byte to the I2C2 bus
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for idle condition -- TWINT bit must be high to proceed
	TWDR = data;       // Load TWDR with data to be sent
	TWCR = (1<<TWINT) | (1<<TWEN); //TWCR = TWCR | 0b10000100;       // initiate Write -- write 1 to TWINT to clear it and initiate action, and set TWEN
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != MT_DATA_ACK) // error checking -- need to predefine MT_DATA_ACK and ERROR() function depending on possible outcomes: 0x28 or 0x30.
		//ERROR();
}


unsigned char i2c_read_data(unsigned char ACK) { //Read a byte of data from a secondary on the I2C2 bus
	unsigned char data;
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for idle condition -- TWINT bit must be high to proceed
	if (ACK) // check for whether ACK or NO_ACK should be sent upon receipt of byte from secondary
		TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN); //TWCR = TWCR | 0b11000100;       // initiate Read with ACK -- write 1 to TWINT to clear it and initiate action, and set TWEA and TWEN
	else
		TWCR = (1<<TWINT) | (1<<TWEN); //TWCR = TWCR | 0b10000100;       // initiate Read with NO_ACK-- write 1 to TWINT to clear it and initiate action, and set TWEN
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != MR_SLA_ACK) // error checking -- need to predefine MR_SLA_ACK and ERROR() function depending on possible outcomes: 0x50 or 0x58.
	//ERROR();
	//If multiple bytes are to be read, this function can be repeated with proper ACK or NO_ACK until done.
	data = TWDR;  // read the received data from secondary
	return(data);
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
	
