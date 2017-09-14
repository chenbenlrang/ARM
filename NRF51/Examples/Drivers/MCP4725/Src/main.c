/**
 * @brief       main.c
 * @details     This example shows how to work with the external device MCP4725 a 12-Bit
 *              Digital-to-Analog Converter with EEPROM Memory.
 *
 *              There will be three different output voltages:
 *
 *                  1. Vout ~ 0V
 *                  2. Vout ~ ( Vref * 0.5 )
 *                  3. Vout ~ Vref
 *
 *              The voltage will change every 0.5 seconds by the timer, the rest of the time, the
 *              microcontroller is in low power.
 *
 * @return      NA
 *
 * @author      Manuel Caballero
 * @date        7/September/2017
 * @version     7/September/2017    The ORIGIN
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
#include "MCP4725.h"


int main( void )
{
    MCP4725_status_t        aux;
    Vector_new_dac_value_t  myNewDACData;
    Vector_data_t           myDefaultData;

    conf_GPIO   ();
    conf_TWI0   ();
    conf_TIMER0 ();

    // Reset and wake the device up
    aux = MCP4725_Reset  ( NRF_TWI0 );
    aux = MCP4725_WakeUp ( NRF_TWI0 );

    // Read the default data in both EEPROM and DAC
    aux = MCP4725_GetDAC_Data    ( NRF_TWI0, MCP4725_ADDRESS_LOW, &myDefaultData );
    aux = MCP4725_GetEEPROM_Data ( NRF_TWI0, MCP4725_ADDRESS_LOW, &myDefaultData );


    mySTATE                  =   1;                 // Reset counter

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
        // Vout ~ 0V
            NRF_GPIO->OUTCLR             =   ( 1UL << LED1 );       // Turn the LED1 on
            myNewDACData.DAC_New_Value   =   0;
            aux = MCP4725_SetNewValue ( NRF_TWI0, MCP4725_ADDRESS_LOW, FAST_MODE, myNewDACData );
            break;

        case 2:
        // Vout = ~ ( Vref * 0.5 )
            myNewDACData.DAC_New_Value   =   2048;
            aux = MCP4725_SetNewValue ( NRF_TWI0, MCP4725_ADDRESS_LOW, WRITE_DAC_AND_EEPROM_REGISTER_MODE, myNewDACData );
            break;

        case 3:
        // Vout ~ Vref
            myNewDACData.DAC_New_Value   =   4095;
            aux = MCP4725_SetNewValue ( NRF_TWI0, MCP4725_ADDRESS_LOW, WRITE_DAC_REGISTER_MODE, myNewDACData );

            mySTATE =   0;
            NRF_GPIO->OUTSET             =   ( 1UL << LED1 );       // Turn the LED1 off
            break;
		}

        //__NOP();

    }
}