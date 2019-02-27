#include "avr.h"
#include <stdio.h>
#include <string.h>
#include "lcd.h"

#define D5 851
#define E5 758
#define Fsh5 675
#define G5 637
#define A5 568
#define B5 506
#define C6 477
#define D6 425
#define B 2002.47
#define E 1336.49
#define Fsh 1500.16
#define A 1784
#define Eb 1261.48
#define G 1589.36
#define D 1190.68
#define Db 1123.85
#define C 1060.77
#define Ab 1683.87
#define Bb 1890.08
#define DUR 40

void avr_init(void)
{
	WDTCR = 15;
}

void
avr_wait(unsigned short msec)
{
	TCCR0 = 3;
	while (msec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.0001);
		SET_BIT(TIFR, TOV0);
		WDR();
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}
char str[17];
char out[17];

void PlayNote(float freq, unsigned int duration, float vol){
	float wav = (1/freq)* 1000;
	unsigned int cycles = duration/wav;
	float period = wav *100;
	lcd_clr();
	lcd_pos(0,1);
	sprintf(str,"per:%2.2f v:%2.2f", period, vol);
	lcd_puts2(str);
	lcd_pos(1,1);
	sprintf(str,"cycles:%d f:%2.2f", cycles, freq);
	lcd_puts2(str);
	
	while(cycles > 0){
		PORTA |= (1<<PA1);
		avr_wait(vol*period);
		PORTA &= ~(1<<PA1);
		avr_wait((1-vol)*period);
		cycles--;
	}
}
struct note{
	float freq;
	int dur;
	};
void PlaySong(){
	lcd_clr();
	lcd_pos(0,1);
	lcd_puts2("Playing Song");
	float tempo= 1;
	float vol=0.5;
	struct note notes[] = {{B, 20}, {E, 40}, {G, 10}, {Fsh, 15}, {E, 40}, {B, 20}, {A, 40}, {Fsh, 40},
		{E, 20}, {G, 10}, {Fsh, 15}, {Eb, 40}, {E, 20}, {B,40},
		{B,20}, {E, 30}, {G, 10}, {Fsh, 15}, {E,40}, {B,20}, {D, 40}, {Db, 20}, {C, 30},
		{Ab,30}, {C,35}, {B,10}, {Bb,15}, {Fsh, 40}, {G, 20}, {E, 100},
		{G, 20}, {B, 40}, {G,20}, {B,40}, {G,20}, {C,40}, {B,20}, {Bb,30},
		{Fsh,30}, {G,35}, {B,10}, {Bb,15}, {Bb,40}, {B,20}, {B,100},
		{G,20}, {E,40}, {G,20}, {B,40}, {G,20}, {D,40}, {Db,20}, {C,30},
	{Ab,30}, {C,35}, {B,10}, {Bb,15}, {Fsh,40}, {G,20}, {E,120}};
	int i;
	for(i=0; i<(sizeof(notes)/sizeof(notes[0])); i++){
		PlayNote(notes[i].freq, tempo*notes[i].dur, vol);
		int key = get_key();
		if(key==8){
			tempo=tempo*.75;
			vol=vol*1.1;
		}
		if(key==12){
			
		}
		if(key==16){
			if(vol>0.15){
				vol=vol-.1;
			}
		}
	}
	lcd_pos(1,1);
	lcd_puts2("Song Finished");
}

int main(void){
	// Initialize everything
	
	avr_init();	
	lcd_init();
	lcd_clr();
	lcd_pos(0,1);
	
	DDRA |= (1<< PA1);
	lcd_clr();
	lcd_pos(0,1);
	lcd_puts2("Press A to play song!");
	
	for(;;){
		//Main loop will check if key pressed, and if it is A or B, do something
		avr_wait(85);
		int key = get_key();
		switch(key){
			// Set date and time
			case 4:
				PlaySong();
				lcd_clr();
				lcd_pos(0,1);
				lcd_puts2("Press A to play song!");
				break;
			default:
				break;
		}
	}
}

/************************************************************************/
/* Gets actual keypad value (numbers 0-9)                               */
/************************************************************************/
int get_num(void){
	int num = 0;
	for(;;){
		int key = get_key();
		switch(key){
			// # is enter key
			case 15:
				return num;
			case 0:
			case 4:
			case 8:
			case 12: 
			case 13:
			case 16: // do nothing in this case
				break;
			default:
				key = key - ((key-1)/4);
				if(key == 11) key = 0;
				num = (num * 10) + key;
				sprintf(out, "%d", key);
				lcd_puts2(out);
				avr_wait(200);
				break;
		}
	}
}

/************************************************************************/
/* Check for if a certain button is pressed                             */
/************************************************************************/
int is_pressed(int row, int col){
	//set all rows, cols to n/c
	DDRC=0;
	PORTC=0;
	//set col to strong 0
	SET_BIT(DDRC, col+4);
	//set row to weak 1
	SET_BIT(PORTC, row);
	avr_wait(1);
	return !GET_BIT(PINC, row);
}

/************************************************************************/
/* Get raw key pressed, different than get_num which does some conversion*/
/* for keypad numbers                                                   */
/************************************************************************/
int get_key(){
	int r,c;
	for(r=0;r<4;++r){
		for(c=0;c<4;++c){
			if(is_pressed(r,c)){
				return 1+(r*4)+c;
			}
		}
	}
	return 0;
}