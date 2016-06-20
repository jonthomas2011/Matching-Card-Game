#include <msp430.h> 
#include "msp430x22x4.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/*///////////////////////////////


	 	 	DEFINITIONS


/////////////////////////////////
*/

void lcd_command(char);
void lcd_char(char);
volatile int repeatKey = 0;

char uf_lcd_temp;
char uf_lcd_temp2;
char uf_lcd_x;

char card[6];          //array of "cards" for the game
char random_array[6];    //array of 6 indeces
int selection;            //key selection
char position4, position5, position6, position7, position8, position9;
char choice1, choice2;     // hold values of 1st and 2nd choice
int counter = 0;               //counter to keep track of #of "pairs" of moves made

int choicecount = 1;		//counter used to check which move youre on
int gamecount = 0;	        //counter used to check once the second move has been made
int final_check = 0;           //counter used to check when final pair is made

int i;
char initial_array[6];
char final_array[6];



/*///////////////////////////////////

		 INITIALIZATIONS

/////////////////////////////////////
*/

void ADC_initialize(){

	ADC10CTL0 = ADC10SHT_3 + ADC10ON;    //64 x ADC10CLKs,  ADC10 on
	ADC10CTL1 = ADC10DIV_7;         //  ADC10CLK/2;
	ADC10AE0 = BIT0;		//Analog (Input) Enable ; enables corresponding pin for analog input - BIT0 = A0.

}

void lcd_init(void){

	/*LCD PIN1: Vss = GND
	LCD PIN2: Vdd = +5V
	LCD PIN3: Vo = Pot driven by +5V
	LCD PIN4: RS = 4.4
	LCD PIN5: R/W = GND
	LCD PIN6: E = 4.5
	LCD PIN11: DB4 = 4.0
	LCD PIN12: DB5 = 4.1
	LCD PIN13: DB6 = 4.2
	LCD PIN14: DB7 = 4.3
	*/

	lcd_command(0x33); 		//Initialize LCD Driver
	lcd_command(0x32);		//Four bit mode
	lcd_command(0x2C); 		//2 Line Mode
	lcd_command(0x0C); 		//Display On, Cursor Off (0x0F for curson on)_
	lcd_command(0x01); 		//Clear Screen, Cursor Home
}

void spi_init(){
	UCA0CTL1 =  UCSWRST;		//software reset enable; do before everything else

	P3SEL |= BIT0 + BIT4;	//set PORT 3 bit0 = UCA 0CLK, bit4 = UCASIMO
	P3DIR |= BIT0 + BIT4 + BIT2;			//sets bits 0 and 4 to outputs (/CS and Data output)

	// bit 3 = Master Mode
							// bit 2-1 = 3-pin SPI
							// bit 0 = Synchronous Mode
	UCA0CTL0 |= UCCKPL + UCMSB + UCMST + UCSYNC;
	 UCA0CTL1 |= UCSSEL_2;                     // SMCLK
	 UCA0BR0 |= 0x01;                            //baudrate/2
	 UCA0BR1 = 0;                              //

	UCA0CTL1 &=  ~UCSWRST;		//clears software enable reset to being tranmission

}

/*///////////////////////////////////

			FUNCTIONS

/////////////////////////////////////
*/

void Delay(){
	volatile int i;
	for(i = 0; i <= 1000; i++);

}
void delay_adc(){
	int i;

	for(i = 0; i < ADC10MEM*10 + 1000; i++);

}

void Out_String(char c[]){
	int i;
	//for (i = 0; i < strlen(c); i++){
	  for(i = 0; i < 6; i++){

		if (i == 3){
			Delay();
			lcd_command (0xC0); 		//used to determine if you are at the end of the line on the first row
			Delay();
			lcd_char(c[i]);
			Delay();
			lcd_char(' ');
										// and will jump down to the next line
		}

		else if( i == 32){
			Delay();
			lcd_char(c[i]);
			lcd_init();
		}
		else{
			Delay();
			lcd_char(c[i]);
			Delay();
			lcd_char(' ');
		}
	}
}

void Out_String2(char c[]){
	int i;
	for (i = 0; i <strlen(c); i++){
		if (i == 16){
			Delay();
			lcd_command (0xC0); 		//used to determine if you are at the end of the line on the first row
										// and will jump down to the next line
			Delay();
			lcd_char(c[i]);

		}
		else if( i == 32){
			Delay();
			lcd_char(c[i]);
			Delay();
			lcd_init();
		}
		else{
			Delay();
			lcd_char(c[i]);
			Delay();

		}
	}
}

void lcd_command(char uf_lcd_x){
	P4DIR = 0xFF;
	uf_lcd_temp = uf_lcd_x;
	P4OUT = 0x00;
	__delay_cycles(5000);
	uf_lcd_x = uf_lcd_x >> 4;
	uf_lcd_x = uf_lcd_x & 0x0F;
	uf_lcd_x = uf_lcd_x | 0x20;
	P4OUT =  uf_lcd_x;
	__delay_cycles(5000);
	uf_lcd_x = uf_lcd_x & 0x0F;
	P4OUT =  uf_lcd_x;
	__delay_cycles(5000);
	P4OUT = 0x00;
	__delay_cycles(5000);
	uf_lcd_x = uf_lcd_temp;
	uf_lcd_x = uf_lcd_x & 0x0F;
	uf_lcd_x = uf_lcd_x | 0x20;
	P4OUT = uf_lcd_x;
	__delay_cycles(5000);
	uf_lcd_x = uf_lcd_x & 0x0F;
	P4OUT = uf_lcd_x;
	__delay_cycles(5000);
}

void lcd_char(char uf_lcd_x){
	P4DIR = 0xFF;
	uf_lcd_temp = uf_lcd_x;
	P4OUT = 0x10;
	__delay_cycles(5000);
	uf_lcd_x = uf_lcd_x >> 4;
	uf_lcd_x = uf_lcd_x & 0x0F;
	uf_lcd_x = uf_lcd_x | 0x30;
	P4OUT = uf_lcd_x;
	__delay_cycles(5000);
	uf_lcd_x = uf_lcd_x & 0x1F;
	P4OUT = uf_lcd_x;
	__delay_cycles(5000);
	P4OUT = 0x10;
	__delay_cycles(5000);
	uf_lcd_x = uf_lcd_temp;
	uf_lcd_x = uf_lcd_x & 0x0F;
	uf_lcd_x = uf_lcd_x | 0x30;
	P4OUT = uf_lcd_x;
	__delay_cycles(5000);
	uf_lcd_x = uf_lcd_x & 0x1F;
	P4OUT = uf_lcd_x;
	__delay_cycles(5000);
}

void led_blink(){

	ADC10CTL0 |= ENC + ADC10SC;

	P1OUT ^= BIT2;		//Toggles the first LED (Red)
	delay_adc();

	P1OUT ^= BIT1;		//Toggles the second LED (Yellow)
	delay_adc();

	P1OUT ^= BIT0;		//Toggles the third LED (Green)
	delay_adc();

}

void led_blink_green(){

	ADC10CTL0 |= ENC + ADC10SC;

	P1OUT = BIT2;		//Toggles the first LED (Red)
	delay_adc();
	P1OUT ^= BIT2;

}

void led_blink_yellow(){

	ADC10CTL0 |= ENC + ADC10SC;

	P1OUT = BIT1;		//Toggles the second LED (Yellow)
	delay_adc();
	_delay_cycles(100000);
	P1OUT ^= BIT1;
}

void led_blink_red(){

	ADC10CTL0 |= ENC + ADC10SC;

	P1OUT = BIT0;		//Toggles the third LED (Green)
	delay_adc();
	P1OUT ^= BIT0;

}
void dac_write(int data){

int output;
int upper_bits;
int lower_bits;
output = 0x0000;
data <<= 6;
data >>= 4;
data |= 0xF000;
output |= data;

upper_bits = output >> 8;
upper_bits &= 0xFF;
lower_bits = output;
lower_bits &= 0xFF;		//truncates upper 8 bits

P3OUT = 0x00;		//set /CS to low

//while ((IFG2 & UCA0TXIFG) == 0);
UCA0TXBUF = upper_bits;
while (UCA0STAT & UCBUSY);			//continues loop while busy flag is set
//while ((IFG2 & UCA0TXIFG) == 0);
UCA0TXBUF = lower_bits;
while (UCA0STAT & UCBUSY);			//"same as above"

P3OUT = BIT2;						//turn /CS to high

}

void triangle_write(int x){
	int value;
	int i;

		for(i = 0; i < 64; i++){

			if (i < 32)
			value = (1023/x)*i;

			else
			value =(1023/x)*(64-i);

			dac_write(value);

		}
}

void key_beep(int x){

	/* x = 16 for low beep (failure to match a pair)
       x = 96 for medium beep (normal presses)
       x = 256 for high beep (correct match)
	*/
	for(i = 0; i < 150; i++){

		triangle_write(x);
	}
}

void card_creation(){

  int i;
  int temp;
  int randomindex;
  card[0] = 'U';
  card[1] = 'U';

  card[2] = 'F';
  card[3] = 'F';

  card[4] = 'L';
  card[5] = 'L';


  for(i = 0; i < 6; i++){
    random_array[i] = card[i];

  }

  for(i = 0; i < 6; i++){
    initial_array[i] = '#';
    final_array[i] = '#';
  }

 srand( time(NULL));  //allows for a different set of randomized variables each time you run the program

  /* the randomizer below takes iterates through random_array and stores each value in temp
     randomindex grabs a random integer variable from 0 - 5 using the rand() function
     the index of i in random_array is then set to the index of random_array[randomindex]
     then random_array[randomindex] takes the index value of random_array[i] therefore swapping the values
     in the array while maintaining all the values and not losing or creating any duplicates.
   */


  for(i = 0; i < 6; i++){                                  // below is an example of how it works:
    temp = random_array[i];                                // temp = random_array[0] = 'U'
    randomindex = rand() % 6;                              // randomindex = 5

    random_array[i] = random_array[randomindex];           //random_array[0] = random_array[5] = 'F'
    random_array[randomindex] = temp;                      //random_array[5] = temp = 'U'

  }
  Delay();
  Out_String(initial_array);
}

void game_initialize(){
Delay();
choicecount = 1;
Delay();
gamecount = 0;
Delay();
final_check = 0;
}

void Keypad(){
volatile int keyVal = 0;
int col = 0;
int rowShift = 0;
int row = 0;
//Determine the column of the key pressed:
	while(1){ //toggle each column, and when input is received, break and determine course of action
		P2OUT = 0b0111;
		Delay();
		col = 1;
		keyVal = P2IN;
		Delay();
		if ( ((keyVal>>4) & (0xF)) != 0b1111) {
		break; //key was pressed
		}
		P2OUT = 0b1011;
		Delay();
		col = 2;
		keyVal = P2IN;
		Delay();
		if ( ((keyVal>>4) & (0xF)) != 0b1111) {
		break; //key was pressed
		}
		P2OUT = 0b1101; //switched backwards due to some error
		Delay();
		col = 3;
		keyVal = P2IN;
		Delay();
		if ( ((keyVal>>4) & (0xF)) != 0b1111) {
		break; //key was pressed
		}
		P2OUT = 0b1110; //switched backwards due to some error
		Delay();
		col = 4;
		keyVal = P2IN;
		Delay();
		if ( ((keyVal>>4) & (0xF)) != 0b1111) {
		break; //key was pressed
		}
		repeatKey = 0;
	}
//Determine the row of the key pressed:
	if(repeatKey == 0){
		rowShift = (keyVal>>4) & 0xF; //shift to check the row depending on highest 4 bits of P2IN
		if (rowShift == 0b0111) {
		row = 1;
		}
		else if (rowShift == 0b1011) {
		row = 2;
		}
		else if (rowShift == 0b1101) {
		row = 3;
		}
		else {
		row = 4;
		}
		//determine the operation based on the row and column (Key) pressed:
		if (row == 1 && col == 1){
		/*(Delay();
		card_creation();
		Delay();
		game_initialize();
		Delay();
		//lcd_init();
		*/
		repeatKey = 1;
		}
		else if (row == 1 && col == 2){
		Delay();
		Out_String("2");
		//lcd_init();
		repeatKey = 1;
		}
		else if (row == 1 && col == 3){
		Delay();
		Out_String("3");
		//lcd_init();
		repeatKey = 1;
		}
		else if (row == 1 && col == 4){
		Delay();
		Out_String("A");
		//lcd_init();
		repeatKey = 1;
		}
		else if (row == 2 && col == 1){											//KEY 4
			Delay();
					lcd_init();
					Delay();
					initial_array[0] = random_array[0];
					if(choicecount % 2 != 0){
					choice1 = initial_array[0];
					}

					else{
					choice2 = initial_array[0];
					}
					Out_String(initial_array);

					led_blink_yellow();
					key_beep(96);
					choicecount++;

		repeatKey = 1;
		}
		else if (row == 2 && col == 2){											//KEY 5
			Delay();
					lcd_init();
					Delay();
					initial_array[1] = random_array[1];

					if(choicecount % 2 != 0){
					choice1 = initial_array[1];
					}

					else{
					choice2 = initial_array[1];
					}
					Out_String(initial_array);

					led_blink_yellow();
					key_beep(96);
					choicecount++;

					repeatKey = 1;
		}
		else if (row == 2 && col == 3){											//KEY 6
			Delay();
					lcd_init();
					Delay();
					initial_array[2] = random_array[2];

					if(choicecount % 2 != 0){
					choice1 = initial_array[2];
					}

					else{
					choice2 = initial_array[2];
					}
					Out_String(initial_array);

					led_blink_yellow();
					key_beep(96);
					choicecount++;

					repeatKey = 1;
		}
		else if (row == 2 && col == 4){
		Delay();
		//Out_String("B");
		//lcd_init();
		repeatKey = 1;
		}
		else if (row == 3 && col == 1){											//KEY 7
			Delay();
					lcd_init();
					Delay();
					initial_array[3] = random_array[3];

					if(choicecount % 2 != 0){
					choice1 = initial_array[3];
					}

					else{
					choice2 = initial_array[3];
					}

					Out_String(initial_array);

					led_blink_yellow();
					key_beep(96);
					choicecount++;

					repeatKey = 1;
		}
		else if (row == 3 && col == 2){											//KEY 8
			Delay();
					lcd_init();
					Delay();
					initial_array[4] = random_array[4];

					if(choicecount % 2 != 0){
					choice1 = initial_array[4];
					}

					else{
					choice2 = initial_array[4];
					}

					Out_String(initial_array);

					led_blink_yellow();
					key_beep(96);
					choicecount++;

					repeatKey = 1;
		}
		else if (row == 3 && col == 3){											//KEY 9
			Delay();
					lcd_init();
					Delay();
					initial_array[5] = random_array[5];

					if(choicecount % 2 != 0){
					choice1 = initial_array[5];
					}

					else{
					choice2 = initial_array[5];
					}

					Out_String(initial_array);

					led_blink_yellow();
					key_beep(96);
					choicecount++;

					repeatKey = 1;
		}
		else if (row == 3 && col == 4){
		Delay();
		//Out_String("C");
		//lcd_init();
		repeatKey = 1;
		}
		else if (row == 4 && col == 1){
		Delay();
		//Out_String("*");
		//lcd_init();
		repeatKey = 1;
		}
		else if (row == 4 && col == 2){
		Delay();
		//Out_String("0");
		//lcd_init();
		repeatKey = 1;
		}
		else if (row == 4 && col == 3){
		Delay();
		//Out_String("#");
		//lcd_init();
		repeatKey = 1;
		}
		else if (row == 4 && col == 4){
		Delay();
		//Out_String("D");
		//lcd_init();
		repeatKey = 1;
		}
	}
	gamecount++;

	if(gamecount % 2 == 0){
		if(choice1 != choice2){
			 for(i = 0; i < 6; i++){
				initial_array[i] = final_array[i];
			 }
			 Delay();
			 key_beep(16);		//low beep to show failure to match
			 Delay();
			 led_blink_red();
			 Delay();
			 lcd_init();
			 Delay();
			 Out_String(initial_array);
		}

		else{
			final_check++;
			for(i = 0; i < 6; i++){
				final_array[i] = initial_array[i];
			 }
			Delay();
			 key_beep(256);		//low beep to show failure to match
			 Delay();
			 led_blink_green();
			 Delay();
			 lcd_init();
			 Delay();
			 Out_String(initial_array);

		}
	}

	else{};

	if(final_check == 3){
		Delay();
		lcd_init();
		Delay();
		Out_String2("Congrats! You Won! in ");
		lcd_char(0x30+gamecount/2);
		Out_String2(" Moves");
		for(i = 0; i < 10; i++){
			led_blink();

		}
		Delay();
		game_initialize();
		Delay();
		lcd_init();
		Delay();
		card_creation();

	}
	else{};
}

/*///////////////////////////////////

		 	 	MAIN

/////////////////////////////////////
*/


int main(void) {

	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	DCOCTL = 0;					// Select lowest DC0x and MODx
	BCSCTL1 = CALBC1_16MHZ;		//Set range
	DCOCTL = CALDCO_16MHZ;		// set DC0 step + modulation

		lcd_init();
		ADC_initialize();
		spi_init();

		P1DIR = 0x07;			//sets pins 2, 1, 0 on PORT 1 to be outputs for LED's
		P1OUT = 0x00;			//initialize output port to 0 for toggling

		P2DIR = 0x0F;			//sets bits 7 - 4 as inputs; bits 3 - 1 as outputs
		P2SEL = 0x00;

		Delay();
		//Out_String("Your board is working!");

		//led_blink();

		card_creation();

		while(1){

			Keypad();
			Delay();
		//	lcd_init();
		//triangle_write(96);
		//led_blink();
		}

		return 0;
}


