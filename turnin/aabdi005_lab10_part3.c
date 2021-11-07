/*	Author: Abdirahman Abdi
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #10  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Link Demo: https://youtu.be/ouB_pySUaHY
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
enum State4 {speakerOff, On} speaker;
unsigned char threeLedVal = 0x00;
unsigned char blinkLedVal = 0x00;
unsigned char speakerVal = 0x00;
unsigned char tickSpeaker = 0x00;


unsigned long threeLed_elapsedTime = 0;
unsigned long fourthLed_elapsedTime = 0;
unsigned long speaker_elapsedTime = 0;
const unsigned long timerPeriod = 1;


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
void TickSpeaker(){
	unsigned char press = ~PINA & 0x04;
	switch(speaker){
		case speakerOff:
			if(press) speaker = On;
            else speaker = speakerOff;
			break;

		case On:
			if(!press) speaker = speakerOff;
			else speaker = On;
	
			break;
        default:
            break;
	}
	switch(speaker){

		case speakerOff:
			speakerVal = 0x00;
			tickSpeaker = 0;
			break;

		case On:
			if(tickSpeaker <= 2)speaker = 0x10;
	        else if(tickSpeaker <= 4) speaker = 0x00;
			else tickSpeaker = 0;
			tickSpeaker++;
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
			PORTB = blinkLedVal | threeLedVal | speakerVal;
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
    speaker = Off;

    TimerSet(1);
    TimerOn();
 
    while (1) {
        if (threeLed_elapsedTime >= 300) {
            TickThreeLeds();
            threeLed_elapsedTime = 0;
        }
        if (fourthLed_elapsedTime >= 1000) {
            TickFourthLed();
            fourthLed_elapsedTime = 0;
        }
        if (speaker_elapsedTime >=2){
            TickSpeaker();
            speaker_elapsedTime = 0;
        }
        TickSpeaker();
        TickCombineLeds();
        while(!TimerFlag);
        TimerFlag = 0;
        threeLed_elapsedTime += timerPeriod;
        fourthLed_elapsedTime += timerPeriod;
        speaker_elapsedTime += timerPeriod;
    }
    return 1;
}