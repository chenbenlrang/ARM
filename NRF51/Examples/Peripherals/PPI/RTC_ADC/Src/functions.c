/**
 * @brief       functions.c
 * @details     Functions file.
 *
 *
 * @return      NA
 *
 * @author      Manuel Caballero
 * @date        14/November/2017
 * @version     14/November/2017   The ORIGIN
 * @pre         This firmware was tested on the nrf51-DK with EmBitz 1.11 rev 0
 *              ( SDK 1.1.0 ).
 * @warning     NaN
 * @pre         This code belongs to AqueronteBlog ( http://unbarquero.blogspot.com ).
 */

#include "functions.h"



/**
 * @brief       void conf_LFCLK  ( void )
 * @details     It turns the internal LFCLK clock on for RTCs.
 *
 * @return      NA
 *
 * @author      Manuel Caballero
 * @date        12/November/2017
 * @version     12/November/2017   The ORIGIN
 * @pre         NaN
 * @warning     NaN.
 */
void conf_LFCLK  ( void )
{
    NRF_CLOCK->LFCLKSRC             =   ( CLOCK_LFCLKSRC_SRC_RC << CLOCK_LFCLKSRC_SRC_Pos );
    NRF_CLOCK->EVENTS_LFCLKSTARTED  =   0;
    NRF_CLOCK->TASKS_LFCLKSTART     =   1;

    while ( NRF_CLOCK->EVENTS_LFCLKSTARTED == 0 )       // [TODO] Insert a counter! otherwise if there is a problem it will get block!!!
    {
        //Do nothing.
    }

    NRF_CLOCK->EVENTS_LFCLKSTARTED  =   0;
}




/**
 * @brief       void conf_GPIO  ( void )
 * @details     It configures GPIO to work with the LEDs.
 *
 *
 * @return      NA
 *
 * @author      Manuel Caballero
 * @date        5/May/2017
 * @version     23/May/2017     All the LEDs OFF at the beginning.
 *              5/May/2017      The ORIGIN
 * @pre         NaN
 * @warning     NaN
 */
void conf_GPIO  ( void )
{
    uint32_t i = 0;

    for ( i = LED1; i < ( LED4 + 1 ); i++ ){
            NRF_GPIO->OUTSET        =   ( 1 << i );

            NRF_GPIO->PIN_CNF[i]    =   GPIO_PIN_CNF_DIR_Output         <<  GPIO_PIN_CNF_DIR_Pos    |
                                        GPIO_PIN_CNF_INPUT_Disconnect   <<  GPIO_PIN_CNF_INPUT_Pos  |
                                        GPIO_PIN_CNF_PULL_Disabled      <<  GPIO_PIN_CNF_PULL_Pos   |
                                        GPIO_PIN_CNF_DRIVE_S0S1         <<  GPIO_PIN_CNF_DRIVE_Pos  |
                                        GPIO_PIN_CNF_SENSE_Disabled     <<  GPIO_PIN_CNF_SENSE_Pos;
    }
}




/**
 * @brief       void conf_GPIOTE  ( void )
 * @details     It configures GPIOTE channels.
 *
 *                  - LED1. IN0 ( Task ).
 *
 *
 * @return      NA
 *
 * @author      Manuel Caballero
 * @date        8/November/2017
 * @version     8/November/2017   The ORIGIN
 * @pre         NaN
 * @warning     NaN
 */
void conf_GPIOTE  ( void )
{
    // Channel 0
    NRF_GPIOTE->CONFIG[0]    =    ( GPIOTE_CONFIG_POLARITY_Toggle   << GPIOTE_CONFIG_POLARITY_Pos   ) |
                                  ( LED1                            << GPIOTE_CONFIG_PSEL_Pos       ) |
                                  ( GPIOTE_CONFIG_MODE_Task         << GPIOTE_CONFIG_MODE_Pos       );



    // Reset the task for channel 0
    NRF_GPIOTE->TASKS_OUT[0]  =   0;


    /*
    NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_IN0_Enabled << GPIOTE_INTENSET_IN0_Pos;
    NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_IN1_Enabled << GPIOTE_INTENSET_IN1_Pos;
    NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_IN2_Enabled << GPIOTE_INTENSET_IN2_Pos;
    NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_IN3_Enabled << GPIOTE_INTENSET_IN3_Pos;
    */

    // Enable Interrupt
    // NVIC_EnableIRQ ( GPIOTE_IRQn );
}




/**
 * @brief       void conf_RTC1  ( void )
 * @details     Tick will create an event every 125ms.
 *
 *              RTC1:
 *                  * Prescaler:            4095   ( f_RTC0 = ( 32.768kHz / ( 4095 + 1 ) ) = 8Hz ( 125ms ) ).
 *
 * @return      NA
 *
 * @author      Manuel Caballero
 * @date        15/November/2017
 * @version     15/November/2017   RTC1 as a ticker
 *              12/November/2017   The ORIGIN
 * @pre         NaN
 * @warning     NaN.
 */
void conf_RTC1  ( void )
{
    NRF_RTC1->TASKS_STOP  =   1;
    NRF_RTC1->PRESCALER   =   4095;                                                                       // f_RTC1 = ( 32.768kHz / ( 4095 + 1 ) ) = 8Hz ( 125ms )
    NRF_RTC1->TASKS_CLEAR =   1;                                                                          // clear the task first to be usable for later.


    NRF_RTC1->EVTENSET   |=   ( RTC_EVTENSET_TICK_Enabled << RTC_EVTENSET_TICK_Pos );
}




/**
 * @brief       void conf_PPI  ( void )
 * @details     It sets up two PPI channels.
 *
 *              Channel 0:
 *                  * Event: NRF_RTC1->EVENTS_TICK.
 *                  * Task:  NRF_ADC->TASKS_START.
 *
 *              Channel 1:
 *                  * Event: NRF_ADC->EVENTS_END.
 *                  * Task:  NRF_GPIOTE->TASKS_OUT[0].
 *
 * @return      NA
 *
 * @author      Manuel Caballero
 * @date        15/November/2017
 * @version     15/November/2017   The ORIGIN
 * @pre         NaN
 * @warning     NaN.
 */
void conf_PPI  ( void )
{
    //NRF_PPI->CHG[0].DIS  =   1;

    NRF_PPI->CH[0].EEP   =   ( uint32_t )&NRF_RTC1->EVENTS_TICK;
    NRF_PPI->CH[0].TEP   =   ( uint32_t )&NRF_ADC->TASKS_START;

    NRF_PPI->CH[1].EEP   =   ( uint32_t )&NRF_ADC->EVENTS_END;
    NRF_PPI->CH[1].TEP   =   ( uint32_t )&NRF_GPIOTE->TASKS_OUT[0];

    // Enable PPI channel 0 and channel 1
    NRF_PPI->CHEN        =   ( PPI_CHEN_CH0_Enabled << PPI_CHEN_CH0_Pos )|
                             ( PPI_CHEN_CH1_Enabled << PPI_CHEN_CH1_Pos );
}




/**
 * @brief       void conf_ADC  ( void )
 * @details     It configures the ADC to measure the VDD.
 *
 *              ADC:
 *                  * 8-bits.
 *                  * VDD/3
 *                  * VBG Enabled
 *
 *              Input Voltage Range:
 *                  * 1.2V VBG ( Reference )   1/3 ( Prescaling )   0 - 3.6V ( Range )
 *
 * @return      NA
 *
 * @author      Manuel Caballero
 * @date        15/November/2017
 * @version     15/November/2017   The ORIGIN
 * @pre         Be aware of the Input Voltage Range ( NRF51 Reference Manual p.164
 *              31.1.4 Input voltage range ).
 * @warning     NaN
 */
void conf_ADC  ( void )
{
    NRF_ADC->TASKS_STOP  =   1;
    NRF_ADC->ENABLE      =   ( ADC_ENABLE_ENABLE_Disabled << ADC_ENABLE_ENABLE_Pos );


    NRF_ADC->CONFIG      =   ( ADC_CONFIG_RES_8bit                        << ADC_CONFIG_RES_Pos    ) |
                             ( ADC_CONFIG_INPSEL_SupplyOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos ) |
                             ( ADC_CONFIG_REFSEL_VBG                      << ADC_CONFIG_REFSEL_Pos );


    // NRF_ADC->INTENSET    =   ( ADC_INTENSET_END_Enabled << ADC_INTENSET_END_Pos );



    NRF_ADC->ENABLE      =   ( ADC_ENABLE_ENABLE_Enabled << ADC_ENABLE_ENABLE_Pos );


    // NVIC_EnableIRQ ( ADC_IRQn );                                                                         // Enable Interrupt for the ADC in the core.
}
