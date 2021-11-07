/*	Author: Abdirahman Abdi
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #10  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Link Demo: https://youtu.be/DPrukgnAhhU
 */
#include <avr/io.h>
#include "io.h"
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
 TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet (unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum State1 {Start1, firstLed, secondLed, thirdLed} firstLeds;
enum State2 {fourthLed, Off} secondLeds;
enum State3 {Start3, Combine} combineLeds;
unsigned char threeLedVal = 0x00;
unsigned char blinkLedVal = 0x00;

void TickThreeLeds(){
	switch(firstLeds){
		case Start1:
			firstLeds = firstLed;
			break;

		case firstLed:
			firstLeds = secondLed;
			break;

		case secondLed:
			firstLeds = thirdLed;
			break;
		case thirdLed:
			firstLeds = firstLed;
			break;
		default:
            break;
	}
	switch(firstLeds){
		case Start1:
			break;

		case firstLed:
			threeLedVal = 0x01;
			break;

		case secondLed:
			threeLedVal = 0x02;
			break;

		case thirdLed:
			threeLedVal = 0x04;
			break;
		default:
            break;
	}
}

void TickFourthLed(){
	switch(secondLeds){
		case Off:
			secondLeds = fourthLed;
			break;

		case fourthLed:
			secondLeds = Off;
			break;
		default:
            break;
	}
	switch(secondLeds){
		case Off:
			blinkLedVal = 0x00;
			break;

		case fourthLed:
			blinkLedVal = 0x08;
			break;
		default:
            break;
	}
}

void TickCombineLeds(){
	switch(combineLeds){
		case Start3:
			combineLeds = Combine;
			break;

		case Combine:
			combineLeds = Combine;
			break;
		default:
            break;
	}
	switch(combineLeds){
		case Start3:
			break;

		case Combine:
			PORTB = blinkLedVal | threeLedVal;
			break;
		default:
            break;
	}
}


int main(void) {
    DDRB=0xFF;  PORTB=0x00;
    firstLeds = Start1;
    secondLeds = Off;
    combineLeds = Start3;

    TimerSet(1000);
    TimerOn();
 
    
    while (1) {
        TickThreeLeds();
        TickFourthLed();
        TickCombineLeds();
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 1;
}