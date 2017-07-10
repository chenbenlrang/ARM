/**
 * @brief       i2c.h
 * @details     NRF51 I2C function libraries.
 *              Header file.
 *
 *
 * @return      NA
 *
 * @author      Manuel Caballero
 * @date        6/July/2017
 * @version     6/July/2017    The ORIGIN
 * @pre         NaN
 * @warning     This file is ONLY for NRF51 device.
 * @pre         This code belongs to AqueronteBlog ( http://unbarquero.blogspot.com ).
 */

#include "nrf.h"
#include "nrf51_bitfields.h"


/**
  * @brief   INTERNAL CONSTANTS
  */
#define I2C_SUCCESS      0x00
#define I2C_FAILURE      0x01


/**
  * @brief   FUNCTION PROTOTYPES
  */
uint32_t    i2c_write   ( uint8_t* i2c_buff, uint32_t length );
uint32_t    i2c_read    ( uint8_t* i2c_buff, uint32_t length );