#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "SSD1306.h"
//#include "SSD1306.c"
#include "i2c.h"
// Function prototypes
void PWM_20_Duty ();
void PWM_50_Duty();
void T25_cycle_delay();
void T100_cycle_delay();
volatile int counter;
int PWM = 0; //initialize PWM count
volatile int rpm;
int main(void){
  DDRA = 0xFF; // set PORTA as Output
  DDRB = 0x00; // set PORTB as input
  DDRD = 0x00; // set PORTB as input
  DDRC = 0x00; // set PORTB as input
  DDRE = 0xff; // set PORTA as Output
  PORTB = 0x01; // activate pull up of PB0
  PORTC |= (1<<6); // Set Pull up SW0 on PC6 (SW0)
  PORTD |= (1<<2); // Set Pull up resistor PD2 (INT0)
  EIMSK=(1<<INT0)|(1<<INT1);
  EICRA=0b00001111; //INT0 and INT1 WILL ACTIVATE Rising EDGE TRIGGER
  sei(); // Set Interrupt Flag
  
  //Timer1 1 second delay
  TCNT1L = 0xf7; //Set counter low byte
  TCNT1H = 0xC2; //Set counter higher byte
  TCCR1A = 0x00; //normal mode
  TCCR1B = 0x05; //prescalar
  TIMSK1=(1<<TOIE1); //set timer overflow interrupt
  sei(); // Set Interrupt Flag
  OLED_Init(); //initialize the OLED
  _delay_ms(1); //delay of 1 millisecond
  OLED_Clear(); //clear the display (for good measure)
  OLED_SetCursor(3, 0); //set cursor at column 3, row 0
  OLED_Printf(" PWM: "); //print to display
  OLED_DisplayNumber(C_DECIMAL_U8, PWM,3); //set number of decimals to 3
  OLED_SetCursor(4, 0); //set cursor at column 4, row 0
  OLED_Printf(" count: "); //print to display
  OLED_DisplayNumber(C_DECIMAL_U8, counter,8); //set number of decimals to 8
  OLED_SetCursor(5, 0); //set cursor at column 5, row 0
  OLED_Printf(" RPM: "); //print to display
  OLED_DisplayNumber(C_DECIMAL_U8, rpm,8); //set number of decimals to 8
  //Start of while loop
  while(1){
  //Start of switch-case
    switch(PWM){
      case 1:
        PWM_10_Duty(); //Duty cycle of 10%
        break;
      case 2:
        PWM_20_Duty(); //Duty cycle of 20%
        break;
      case 3:
        PWM_30_Duty(); //Duty cycle of 30%
        break;
      case 4:
        PWM_40_Duty(); //Duty cycle of 40%
        break;
      case 5:
        PWM_50_Duty(); //Duty cycle of 50%
        break;
      case 6:
        PWM_60_Duty(); //Duty cycle of 60%
        break;
      case 7:
        PWM_70_Duty(); //Duty cycle of 70%
        break;
      case 8:
        PWM_80_Duty(); //Duty cycle of 80%
        break;
      case 9:
        PWM_90_Duty(); //Duty cycle of 90%
        break;
    } // end of switch-case
  } //end of while loop
} //end of main method

// INTERRUPT SERVICE ROUTINES
// D2 interrupt 0
ISR(INT0_vect,ISR_BLOCK) {
  _delay_ms(125); //delay of 125 milliseconds
  if (PWM >= 10) //check if PWM is greater than or equal to 10
  {
    PWM = 0; //set PWM to 0
  }
  PWM++ ; //increment PWM
  PORTA = PWM; //send PWM number to PORTA
  _delay_ms(125); //delay of 125 milliseconds
  return; //return to previous task
}

// D3 interrupt 1
ISR(INT1_vect,ISR_BLOCK) {
  OLED_SetCursor(3, 0);
  OLED_Printf(" PWM: ");
  OLED_DisplayNumber(C_DECIMAL_U8, PWM,3);
  OLED_SetCursor(4, 0);
  OLED_Printf(" count: ");
  OLED_DisplayNumber(C_DECIMAL_U8, counter,8);
  OLED_SetCursor(5, 0);
  OLED_Printf(" RPM: ");
  OLED_DisplayNumber(C_DECIMAL_U8, rpm,8);
  counter++;// increase count for rotations
  return;
}

// Software interrupt From timer1 after one second
ISR(TIMER1_OVF_vect,ISR_BLOCK){
  OLED_SetCursor(3, 0);
  OLED_Printf(" PWM: ");
  OLED_DisplayNumber(C_DECIMAL_U8, PWM,3);
  rpm = counter*60;
  counter = 0;
  TIFR1 = (1<<TOV1); // Clear TIFR1 (Timer Flag Register)
  TCNT1L = 0xf7;
  TCNT1H = 0xC2;
  return;
}

// PWM FUNCTIONS
void PWM_10_Duty (){
  while(1){
    PORTE = PORTE ^ (1<<PE2);
    _delay_us(6.25);
    PORTE = PORTE ^ (1<<PE2);
    _delay_us(56.25);
    if (PWM != 1) {
      break;
    }
  }
  return;
}

void PWM_20_Duty (){
  while(1){
    PORTE = PORTE ^ (1<<PE2);
    T25_cycle_delay();
    PORTE = PORTE ^ (1<<PE2);
    T100_cycle_delay();
    if (PWM != 2) {
      break;
    }
  }
  return;
}

void PWM_30_Duty (){
  while(1){
    PORTE = PORTE ^ (1<<PE2);
    _delay_us(18.75);
    PORTE = PORTE ^ (1<<PE2);
    _delay_us(43.75);
    if (PWM != 3){
      break;
    }
  }
  return;
}

void PWM_40_Duty (){
   while(1){
    PORTE = PORTE ^ (1<<PE2);
    _delay_us(25);
    PORTE = PORTE ^ (1<<PE2);
    _delay_us(37.5);
    if (PWM != 4) {
      break;
    }
  }
  return;
}

void PWM_50_Duty(){
  while(1){
    PORTE = PORTE ^ (1<<PE2);
    TCNT0 = -62;
    TCCR0A = 0x00;
    TCCR0B = 0x02; // prescaler
    while((TIFR0&(1<<TOV0))==0);
    TCCR0B = 0;
    TIFR0 = (1<<TOV0);
    if (PWM != 5) {
      break;
    }
  }
}

void PWM_60_Duty (){
  while(1){
    PORTE = PORTE ^ (1<<PE2);
    _delay_us(37.5);
    PORTE = PORTE ^ (1<<PE2);
    _delay_us(25);
    if (PWM != 6){
      break;
    }
  }
  return;
}

void PWM_70_Duty (){
  while(1){
    PORTE = PORTE ^ (1<<PE2);
    _delay_us(43.75);
    PORTE = PORTE ^ (1<<PE2);
    _delay_us(18.75);
    if (PWM != 7){
      break;
    }
  }
  return;
}
void PWM_80_Duty (){
  while(1){
    PORTE = PORTE ^ (1<<PE2);
    T100_cycle_delay();
    PORTE = PORTE ^ (1<<PE2);
    T25_cycle_delay();
    if (PWM != 8){
      break;
    }
  }
  return;
}

void PWM_90_Duty (){
  while(1){
    PORTE = PORTE ^ (1<<PE2);
    _delay_us(56.25);
    PORTE = PORTE ^ (1<<PE2);
    _delay_us(6.25);
    if (PWM != 9){
      break;
    }
  }
  return;
}

void T25_cycle_delay(){
  TCNT0 = -25;
  TCCR0A = 0x00;
  TCCR0B = 0x02; // prescaler
  while((TIFR0&(1<<TOV0))==0);
  TCCR0B = 0;
  TIFR0 = (1<<TOV0);
}

void T100_cycle_delay(){
  TCNT0 = -100;
  TCCR0A = 0x00;
  TCCR0B = 0x02; // prescaler
  while((TIFR0&(1<<TOV0))==0);
  TCCR0B = 0;
  TIFR0 = (1<<TOV0);
}
