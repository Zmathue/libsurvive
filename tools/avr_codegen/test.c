#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

void delay_ms(uint32_t time) {
  uint32_t i;
  for (i = 0; i < time; i++) {
    _delay_ms(1);
  }
}

#define NOOP asm volatile("nop" ::)


static void setup_clock( void )
{
	/*Examine Page 33*/

	CLKPR = 0x80;	/*Setup CLKPCE to be receptive*/
	CLKPR = 0x00;	/*No scalar*/
	OSCCAL = 0x60; //B8 is bottom E8 is top. 

	//This is gonna be about 1.8 MHz, turns out that's probably better than the 2MHz suggestion from the lighthouse-redox people.
}

int main( )
{
	cli();
	setup_clock();
	DDRB = _BV(4);
	uint8_t marker;

	while(1)
	{
		//0x05 says 0xa9
		//0x06 says 0xb3, 0xb4
		//0x07 says 0xc1
		//0x08 says 0xd4
		// 0xd4 - 0xa9 = 2b (43 base 10) 

		//"marker" = Code from Vive (Operating at OSCCAL = 0x80 = 8 MHz)
		//0x08 = 0xe1
		//0x05 = 0xb4
		//at OSCCAL = 0x70
		//0x05 = 0xa6
		//0x08 = 0xcb
		//  12 = 
		//  13 = 0x85 0x01
		//  14 = 0x95 0x01
		//	15 = 0xa1 0x01
		//0x10 = 0xab 0x01
		marker = 10;
		do{
			PORTB = _BV(4);
			marker--;
			PORTB = 0;
		} while( marker );
		_delay_us(8);
		marker = 10;
		do{
			PORTB = _BV(4);
			marker--;
			PORTB = 0;
		} while( marker );
		_delay_us(16);
		marker = 20;
		do{
			PORTB = _BV(4);
			marker--;
			PORTB = 0;
		} while( marker );

		//  t b6 b5 b4 b3 b2 b1 b0    t b13 b12 b11 b10 b9 b8 b7
		//  1 0  0  0  0  1  0  1     0  0   0   0   1   0  0  0
		//			8       5               0             8
		//  
		//One pulse;
		//08 [a5] ...  @05
		//08 [da 01] 54 a9 c6 a0 35 fe 2a  @20?   == 218 -> 436
		//08 [e2] e2 46 b3 14 49 3c b2  @10
		//08 [e9] e2 46 b3 14 49 3c b2  @11
		//08 [f5] @12
		//08 [82 01] @13
		//08 [a7 08] @93
		//08 [f7 03] @40 --> 1f7 --. 500(base 10) * 2 -> 750
		//08 [85 08] @80 --> 1029

		//evverything after those 08 08 is two variable length fields of data 
		//one starting after the 0808, and one going in reverse from the end 

		//For two pulses:
		//08 08 [fc]    [a2 01] [a6 01] / a3 42 ca / 4d e0 84 ef (@12) @10us between
		//08 08 [9f 01] [a4 01] [c6 01] e6 53 33 / 76 7c c4 04 (@15)
		//08 08 [a0 01] [b9 01] [c5 01] aa e9 cd / e6 da 0b cc (@15) @10us between
		//08 08 [da 01] [b8 01] [c5 01] 90 9e 4a / d8 8a e1 55 (@20/@15) @10us between
		//08 08 [db 01] [b8 01] [80 02] 1a f1 4a / a2 04 98 a0 (@20/@20) @10us between!
		//08 08 [db 01] [a8] [9f 02] 99 27 07 5b / 7e 42 64 (@20/@20) @5us between!
		//[sensor codes] [pulse time1] [length of time between pulses] [pulse time2]

		//3 pulses, equal length:
		// [78 78 78] [f0 01] [c7] [aa 02] [c7] [aa 02] 9f df 90 7d 35 11 c5 
		// (78) (78) (78) [b3 02] [94 01] [9b 02] [95 01] [9b 02]   97 5b 8c 14 af 08 43 
		

		//_delay_us(1000000/60);
		_delay_ms(10);
	}

	return 0;
} 
