#if defined(__dsPIC33F__)
#include "p33fxxxx.h"
#endif

#include "pwm.h"

void init_PWM2()
{
 P2TCONbits.PTOPS = 0; // Timer base output scale
 P2TCONbits.PTMOD = 0; // Free running

 P2TMRbits.PTDIR = 0; // Numara in sus pana cand timerul = perioada
 P2TMRbits.PTMR = 0; // Baza de timp

 P2DC1 = 0x2710; 
 P2TPER = 0x4E20;

 PWM2CON1bits.PMOD1 = 1; // Canalele PWM2H si PWM2L sunt independente

 PWM2CON1bits.PEN1H = 1; // Pinul PWM2H1 setat pe iesire PWM RB8
 PWM2CON1bits.PEN1L = 0; // Pinul PWM2L1 pe I/O general purpose RB9

 PWM2CON2bits.UDIS = 0  ; // Disable Updates from duty cycle and period buffers
 P2TCONbits.PTEN = 1; /* Enable the PWM Module */
} 

