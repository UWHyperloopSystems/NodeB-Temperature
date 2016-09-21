/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>       /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>   /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include <stdio.h> 

#endif

#include "can.h"
#include "uart.h"
#include "i2c.h"
#include "user.h"
#include "config.h"
#include "interrupts.h"
#include "ADC_Config.h"    /* ADC Channel selects and read function */


/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/* i.e. uint8_t <variable_name>; */
// Adjust to system clock
#define _XTAL_FREQ 16000000

/******************************************************************************/
/* Function Prototypes                                                        */
/******************************************************************************/
//void print_can_message(Message* mess);

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
Message data; //global message used to send can signals
unsigned int temp0 = 0;

void main(void)
{
    /* Initialization functions called below */
    ConfigureOscillator();
    
    //uart_init(9600);
    ecan_init();
    //i2c_init();
    setAnalogIn();
     
    TRISAbits.TRISA0 = 0;    // set pin digital 0 as an output
    TRISAbits.TRISA6 = 0;
    TRISAbits.TRISA7 = 0;
    TRISDbits.TRISD2 = 0;

    int wait = 10;
    char firstByte = 1;
    char secondByte = 1;
    
    while(1)
    {  
        LATAbits.LATA7 ^= 1; // switches every iteration of while loop
        wait_ms(wait);
        while(!(RXB0CONbits.RXB0FUL)){};
        
        Message newMessage;
        ecan_receive_rxb0(&newMessage);
        selectAN4();
        temp0 = readADC();
        
        if(temp0 > 1){
           LATDbits.LATD2 ^= 1;
            firstByte = temp0/256;  
            secondByte = temp0%256;
        }

        if(newMessage.data[0] == 0x02){
            LATAbits.LATA0 ^= 1;
            data.sid = 0x001;
            data.len = 2;
            data.data[0] = firstByte;
            data.data[1] = secondByte;
            ecan_send(&data);  
        }                    
        ecan_rxb0_clear();          // Clear flag
        wait_ms(wait);
    }
}
