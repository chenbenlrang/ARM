/**
 * @brief       main.c
 * @details     This example shows hot to work with the internal peripheral PPI: RTC and TEMP.
 *              A new temperature mearument is performed every 1 second by PPI.
 *
 *              The microcontroller is in low power the rest of the time.
 *
 *
 * @return      N/A
 *
 * @author      Manuel Caballero
 * @date        14/July/2018
 * @version     14/July/2018    The ORIGIN
 * @pre         This firmware was tested on the nrf52-DK with Segger Embedded Studio v3.40
 *              ( SDK 14.2.0 ).
 * @warning     The softdevice (s132) is taken into account, Bluetooth was not used although.
 * @pre         This code belongs to AqueronteBlog ( http://unbarquero.blogspot.com ).
 */
#include <stdio.h>

#include "nordic_common.h"
#include "nrf.h"
#include "board.h"
#include "functions.h"
#include "interrupts.h"


/**@brief Constants.
 */
#define TX_BUFF_SIZE  32                        /*!<   UART buffer size                               */


/**@brief Variables.
 */
volatile uint8_t  myMessage[ TX_BUFF_SIZE ];     /*!<   Message to be transmitted through the UART     */
volatile uint8_t  *myPtr;                        /*!<   Pointer to point out myMessage                 */
volatile uint32_t myState;                       /*!<   Variable to performe a new reading             */


/**@brief Function for application main entry.
 */
int main(void)
{
  uint32_t myTEMP;                        
  float    myTemperature   =   0;


  conf_CLK    ();
  conf_LFCLK  ();
  conf_GPIO   ();
  conf_UART0  ();
  conf_PPI    ();
  conf_RTC2   ();
  conf_TEMP   ();



  NRF_RTC2->TASKS_START = 1;          // Start RTC2

//  NRF_POWER->SYSTEMOFF = 1;
  NRF_POWER->TASKS_LOWPWR = 1;        // Sub power mode: Low power.
  while( 1 )
  {
    // Enter System ON sleep mode
    __WFE();
    // Make sure any pending events are cleared
    __SEV();
    __WFE();

    
    if ( myState == 1 )
    {
      NRF_GPIO->OUTCLR         =   ( 1 << LED1 );         // Tun the LED1 on
      
      /* Get the temperature value   */
      myTEMP   =   NRF_TEMP->TEMP;

      /* Parse the data  */
      myTemperature  =   ( myTEMP / 4.0 ) + 0.5;
      sprintf ( (char*)myMessage, "Temperature: %d C\r\n", (int)myTemperature );

      /* Send data through the UART  */
      NRF_UART0->TASKS_STOPRX  =   1;
      NRF_UART0->TASKS_STOPTX  =   1;
      myPtr                    =   &myMessage[0];

      NRF_UART0->TASKS_STARTTX =   1;
      NRF_UART0->TXD           =   *myPtr;


      /* Reset the variables   */
      myState                  =   0;
      NRF_GPIO->OUTSET         =   ( 1 << LED1 );           // Turn the LED1 off
    }
  }
}


/**
 * @}
 */
