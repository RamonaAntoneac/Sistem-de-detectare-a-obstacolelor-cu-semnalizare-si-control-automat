#include <stdio.h>
#include "p33Fxxxx.h"
#include "adcDrv1.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"

#include "partest.h"

// Includes proprii
#include "new_lcd.h"
#include "pwm.h"

#define ptOUTPUT 	0			// toti pinii sunt iesiri
#define ptALL_OFF	0




// Select Internal FRC at POR
_FOSCSEL(FNOSC_FRC);
// Enable Clock Switching and Configure
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF);		// FRC + PLL
//_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT);		// XT + PLL
_FWDT(FWDTEN_OFF); 		// Watchdog Timer Enabled/disabled by user software


static void prvSetupHardware( void );

void Task1(void *params) {  			// task - senzor de proximitate

	for (;;){
		if (_RB6 == 1) {   				// 1 - nu exista obstacol ; 0 - exista obstacol 
			LCD_Goto(1, 10);  				// mutare pointer LCD pe linia 1 coloana 12
			LCD_printf("NU"); 			// mesaj obstacol nedetectat
			_RB7 = 1; 					// led stins 
		}
		else {
			LCD_Goto(2, 10);  			// mutare pointer LCD pe linia 2 coloana 12
			LCD_printf("DA");			// mesaj obstacol detectat
			_RB7 = 0; 					// led aprins
		}

		vTaskDelay(300);  				// perioada refresh ecran
	}		
}

void Task2(void *params) {  			// task2 - senzor analogic de distanta

	unsigned int over, under;
	for (;;){

		taskENTER_CRITICAL();			// protejarea datelor 
        over  = adc_peste_prag;			// valoare peste prag
        under = adc_sub_prag;			// valoare sub prag 
        adc_peste_prag = 0;				// resetare pentru contorizare
        adc_sub_prag = 0;
        taskEXIT_CRITICAL();

        if (over > under){				// verificare pozitionare fata de pragul stabilit 
			LCD_Goto(3, 1);
            LCD_printf("aproape");		// valoare peste prag 
			P2DC1 = 0;			// motor oprit 
		}
        else{
			LCD_Goto(4, 1);
            LCD_printf("departe");		// valoare sub prag
			P2DC1 = P2TPER;			//motor pornit
		}

		vTaskDelay(300);  				// perioada refresh ecran
	}		
}



int main( void )
{
	/* Configure any hardware required for this demo. */
	prvSetupHardware();

	xTaskCreate(Task1, (signed portCHAR *) "Ts1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(Task2, (signed portCHAR *) "Ts2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

	/* Finally start the scheduler. */
	vTaskStartScheduler();

	return 0;
}
/*-----------------------------------------------------------*/

void initPLL(void)
{
// Configure PLL prescaler, PLL postscaler, PLL divisor
	PLLFBD = 41; 		// M = 43 FRC
	//PLLFBD = 30; 		// M = 32 XT
	CLKDIVbits.PLLPOST=0; 	// N1 = 2
	CLKDIVbits.PLLPRE=0; 	// N2 = 2

// Initiate Clock Switch to Internal FRC with PLL (NOSC = 0b001)
	__builtin_write_OSCCONH(0x01);	// FRC
	//__builtin_write_OSCCONH(0x03);	// XT
	__builtin_write_OSCCONL(0x01);

// Wait for Clock switch to occur
	while (OSCCONbits.COSC != 0b001);	// FRC
	//while (OSCCONbits.COSC != 0b011);	// XT

// Wait for PLL to lock
	while(OSCCONbits.LOCK!=1) {};
}

static void prvSetupHardware( void )
{	
        RCONbits.SWDTEN=0;
        initPLL();
	ADPCFG = 0xFFFF;				// make ADC pins all digital - adaugat
	vParTestInitialise();
	
	PORTB = 0X0000; 								
	TRISB = 0x0000; 				// toti pinii: OUT
	_TRISB6 = 1;  				// RB6 - IN (senzor digital proximitate)
	_TRISB2 = 1;				// RB2 - IN (senzor analogic distanta)
// Peripheral Initialisation
//	initPLL();

   	initAdc1();             	// Initialize ADC
	initTmr3();					// Initialise TIMER 3

	LCD_init();

	LCD_line(1);
	LCD_printf("Obstacol:");
	init_PWM2();				// initializare PXW2

}


