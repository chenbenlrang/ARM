/**
 * @brief       main.c
 * @details     This project shows how to work with the external light sensor BH1750.
 *              It performs a new measurement every three seconds and transmits the data
 *              through the UART.
 *
 *              This firmware is just an example about how to use the I2C on the nrf51422.
 *
 * @return      NA
 *
 * @author      Manuel Caballero
 * @date        2/August/2017
 * @version     2/August/2017    The ORIGIN
 * @pre         This firmware was tested on the nrf51-DK with EmBitz 1.11 rev 0
 *              ( SDK 1.1.0 ).
 * @warning     Softdevice S310 was used although the file's name is S130. The softdevice
 *              is not used in this example anyway because of Bluetooth was not used.
 * @pre         This code belongs to AqueronteBlog ( http://unbarquero.blogspot.com ).
 */

#include "nrf.h"
#include "ble.h"
#include "variables.h"
#include "functions.h"
#include "BH1750.h"


int main( void )
{
    // float    myLux         =       0;
    uint8_t  myRawLux[]    =       { 0, 0 };
    uint32_t aux           =       0;


    conf_GPIO   ();
    conf_UART   ();
    conf_TWI0   ();
    conf_TIMER0 ();


    mySTATE                  =   0;                 // Reset counter

    NRF_TIMER0->TASKS_START  =   1;                 // Start Timer0


    while( 1 )
    {
        //NRF_POWER->SYSTEMOFF = 1;
        NRF_POWER->TASKS_LOWPWR = 1;                // Sub power mode: Low power.

        // Enter System ON sleep mode
		__WFE();
		// Make sure any pending events are cleared
		__SEV();
		__WFE();

		switch ( mySTATE ){
        default:
        case 1:
            aux = BH1750_TriggerMeasurement ( NRF_TWI0, BH1750_ADDR_L, BH1750_ONE_TIME_H_RESOLUTION_MODE );
            break;

        case 2:
            aux = BH1750_ReadRawData            ( NRF_TWI0, BH1750_ADDR_L, &myRawLux[0] );
            // aux = BH1750_ReadLux            ( NRF_TWI0, BH1750_ADDR_L, &myLux );
            break;

        case 3:
            // Start transmitting through the UART
            NRF_GPIO->OUTCLR             =   ( 1UL << LED1 );       // Turn the LED1 on

            myPtr                        =   &myRawLux[0];
            TX_inProgress                =   YES;
            NRF_UART0->TASKS_STARTTX     =   1;
            NRF_UART0->TXD               =   *myPtr++;              // MSB to be transmitted

            // Wait until the message is transmitted
            while ( TX_inProgress == YES ){
                __WFE();
                // Make sure any pending events are cleared
                __SEV();
                __WFE();
            }

            mySTATE =   0;
            break;
		}

        //__NOP();

    }
}
