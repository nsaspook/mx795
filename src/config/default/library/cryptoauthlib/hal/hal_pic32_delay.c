/**
 * \file
 * \brief HAL delay related functionality implementation.
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#include <stdint.h>
#include <xc.h>
#include <cp0defs.h>

#include "atca_config.h"
#include "atca_hal.h"


/**
 * \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */


#define GetSystemClock() 80000000
#define us_SCALE   (GetSystemClock() / 2000000)


void delay_us(uint32_t delay)
{
    uint32_t startCnt = _CP0_GET_COUNT();
    uint32_t waitCnt = delay * us_SCALE;

    while ((_CP0_GET_COUNT() - startCnt) < waitCnt)
    {
        ;
    }
}


/**
 * \brief This function delays for a number of microseconds.
 *
 * \param[in] delay number of 0.001 milliseconds to delay
 */
void atca_delay_us(uint32_t delay)
{
    delay_us(delay);
}

/**
 * \brief This function delays for a number of tens of microseconds.
 *
 * \param[in] delay number of 0.01 milliseconds to delay
 */
void atca_delay_10us(uint32_t delay)
{
    atca_delay_us(delay * 10);
}

/**
 * \brief This function delays for a number of milliseconds.
 *
 *        You can override this function if you like to do
 *        something else in your system while delaying.
 *
 * \param[in] delay number of milliseconds to delay
 */
#ifndef ATCA_USE_RTOS_TIMER
void atca_delay_ms(uint32_t delay)
#else
void atca_delay_ms_internal(uint32_t delay)
#endif
{

    atca_delay_us(delay * 1000);

}

/** @} */