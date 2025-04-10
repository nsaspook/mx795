/*******************************************************************************
  UART3 PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_uart3.c

  Summary:
    UART3 PLIB Implementation File

  Description:
    None

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#include "device.h"
#include "plib_uart3.h"
#include "interrupts.h"

// *****************************************************************************
// *****************************************************************************
// Section: UART3 Implementation
// *****************************************************************************
// *****************************************************************************

static volatile UART_OBJECT uart3Obj;

static void UART3_ErrorClear( void )
{
    UART_ERROR errors = UART_ERROR_NONE;
    uint8_t dummyData = 0u;

    errors = (UART_ERROR)(U3STA & (_U3STA_OERR_MASK | _U3STA_FERR_MASK | _U3STA_PERR_MASK));

    if(errors != UART_ERROR_NONE)
    {
        /* If it's a overrun error then clear it to flush FIFO */
        if((U3STA & _U3STA_OERR_MASK) != 0U)
        {
            U3STACLR = _U3STA_OERR_MASK;
        }

        /* Read existing error bytes from FIFO to clear parity and framing error flags */
        while((U3STA & _U3STA_URXDA_MASK) != 0U)
        {
            dummyData = (uint8_t)U3RXREG;
        }

        /* Clear error interrupt flag */
        IFS1CLR = _IFS1_U3EIF_MASK;

        /* Clear up the receive interrupt flag so that RX interrupt is not
         * triggered for error bytes */
        IFS1CLR = _IFS1_U3RXIF_MASK;
    }

    // Ignore the warning
    (void)dummyData;
}

void UART3_Initialize( void )
{
    /* Set up UxMODE bits */
    /* STSEL  = 0 */
    /* PDSEL = 0 */
    /* UEN = 0 */

    U3MODE = 0x8;

    /* Enable UART3 Receiver and Transmitter */
    U3STASET = (_U3STA_UTXEN_MASK | _U3STA_URXEN_MASK | _U3STA_UTXISEL1_MASK );

    /* BAUD Rate register Setup */
    U3BRG = 173;

    /* Disable Interrupts */
    IEC1CLR = _IEC1_U3EIE_MASK;

    IEC1CLR = _IEC1_U3RXIE_MASK;

    IEC1CLR = _IEC1_U3TXIE_MASK;

    /* Initialize instance object */
    uart3Obj.rxBuffer = NULL;
    uart3Obj.rxSize = 0;
    uart3Obj.rxProcessedSize = 0;
    uart3Obj.rxBusyStatus = false;
    uart3Obj.rxCallback = NULL;
    uart3Obj.txBuffer = NULL;
    uart3Obj.txSize = 0;
    uart3Obj.txProcessedSize = 0;
    uart3Obj.txBusyStatus = false;
    uart3Obj.txCallback = NULL;
    uart3Obj.errors = UART_ERROR_NONE;

    /* Turn ON UART3 */
    U3MODESET = _U3MODE_ON_MASK;
}

bool UART3_SerialSetup( UART_SERIAL_SETUP *setup, uint32_t srcClkFreq )
{
    bool status = false;
    uint32_t baud;
    uint32_t status_ctrl;
    uint32_t uxbrg = 0;

    if(uart3Obj.rxBusyStatus == true)
    {
        /* Transaction is in progress, so return without updating settings */
        return status;
    }
    if (uart3Obj.txBusyStatus == true)
    {
        /* Transaction is in progress, so return without updating settings */
        return status;
    }

    if (setup != NULL)
    {
        baud = setup->baudRate;

        if ((baud == 0U) || ((setup->dataWidth == UART_DATA_9_BIT) && (setup->parity != UART_PARITY_NONE)))
        {
            return status;
        }

        if(srcClkFreq == 0U)
        {
            srcClkFreq = UART3_FrequencyGet();
        }

        /* Calculate BRG value */
        uxbrg = (((srcClkFreq >> 2) + (baud >> 1)) / baud);
        /* Check if the baud value can be set with low baud settings */
        if (uxbrg < 1U)
        {
            return status;
        }

        uxbrg -= 1U;

        if (uxbrg > UINT16_MAX)
        {
            return status;
        }

        /* Turn OFF UART3. Save UTXEN, URXEN and UTXBRK bits as these are cleared upon disabling UART */

        status_ctrl = U3STA & (_U3STA_UTXEN_MASK | _U3STA_URXEN_MASK | _U3STA_UTXBRK_MASK);

        U3MODECLR = _U3MODE_ON_MASK;

        if(setup->dataWidth == UART_DATA_9_BIT)
        {
            /* Configure UART3 mode */
            U3MODE = (U3MODE & (~_U3MODE_PDSEL_MASK)) | setup->dataWidth;
        }
        else
        {
            /* Configure UART3 mode */
            U3MODE = (U3MODE & (~_U3MODE_PDSEL_MASK)) | setup->parity;
        }

        /* Configure UART3 mode */
        U3MODE = (U3MODE & (~_U3MODE_STSEL_MASK)) | setup->stopBits;

        /* Configure UART3 Baud Rate */
        U3BRG = uxbrg;

        U3MODESET = _U3MODE_ON_MASK;

        /* Restore UTXEN, URXEN and UTXBRK bits. */
        U3STASET = status_ctrl;

        status = true;
    }

    return status;
}

bool UART3_AutoBaudQuery( void )
{
    bool autobaudqcheck = false;
    if((U3MODE & _U3MODE_ABAUD_MASK) != 0U)
    {

       autobaudqcheck = true;
    }
    return autobaudqcheck;
}

void UART3_AutoBaudSet( bool enable )
{
    if( enable == true )
    {
        U3MODESET = _U3MODE_ABAUD_MASK;
    }

    /* Turning off ABAUD if it was on can lead to unpredictable behavior, so that
       direction of control is not allowed in this function.                      */
}

bool UART3_Read(void* buffer, const size_t size )
{
    bool status = false;

    if(buffer != NULL)
    {
        /* Check if receive request is in progress */
        if(uart3Obj.rxBusyStatus == false)
        {
            /* Clear error flags and flush out error data that may have been received when no active request was pending */
            UART3_ErrorClear();

            uart3Obj.rxBuffer = buffer;
            uart3Obj.rxSize = size;
            uart3Obj.rxProcessedSize = 0;
            uart3Obj.rxBusyStatus = true;
            uart3Obj.errors = UART_ERROR_NONE;
            status = true;

            /* Enable UART3_FAULT Interrupt */
            IEC1SET = _IEC1_U3EIE_MASK;

            /* Enable UART3_RX Interrupt */
            IEC1SET = _IEC1_U3RXIE_MASK;
        }
    }

    return status;
}

bool UART3_Write( void* buffer, const size_t size )
{
    bool status = false;

    if(buffer != NULL)
    {
        /* Check if transmit request is in progress */
        if(uart3Obj.txBusyStatus == false)
        {
            uart3Obj.txBuffer = buffer;
            uart3Obj.txSize = size;
            uart3Obj.txProcessedSize = 0;
            uart3Obj.txBusyStatus = true;
            status = true;

            size_t txProcessedSize = uart3Obj.txProcessedSize;
            size_t txSize = uart3Obj.txSize;

            /* Initiate the transfer by writing as many bytes as we can */
            while(((U3STA & _U3STA_UTXBF_MASK) == 0U) && (txSize > txProcessedSize) )
            {
                if (( U3MODE & (_U3MODE_PDSEL0_MASK | _U3MODE_PDSEL1_MASK)) == (_U3MODE_PDSEL0_MASK | _U3MODE_PDSEL1_MASK))
                {
                    /* 9-bit mode */
                    U3TXREG = ((uint16_t*)uart3Obj.txBuffer)[txProcessedSize];
                    txProcessedSize++;
                }
                else
                {
                    /* 8-bit mode */
                    U3TXREG = ((uint8_t*)uart3Obj.txBuffer)[txProcessedSize];
                    txProcessedSize++;
                }
            }

            uart3Obj.txProcessedSize = txProcessedSize;

            IEC1SET = _IEC1_U3TXIE_MASK;
        }
    }

    return status;
}

UART_ERROR UART3_ErrorGet( void )
{
    UART_ERROR errors = uart3Obj.errors;

    uart3Obj.errors = UART_ERROR_NONE;

    /* All errors are cleared, but send the previous error state */
    return errors;
}

void UART3_ReadCallbackRegister( UART_CALLBACK callback, uintptr_t context )
{
    uart3Obj.rxCallback = callback;

    uart3Obj.rxContext = context;
}

bool UART3_ReadIsBusy( void )
{
    return uart3Obj.rxBusyStatus;
}

size_t UART3_ReadCountGet( void )
{
    return uart3Obj.rxProcessedSize;
}

bool UART3_ReadAbort(void)
{
    if (uart3Obj.rxBusyStatus == true)
    {
        /* Disable the fault interrupt */
        IEC1CLR = _IEC1_U3EIE_MASK;

        /* Disable the receive interrupt */
        IEC1CLR = _IEC1_U3RXIE_MASK;

        uart3Obj.rxBusyStatus = false;

        /* If required application should read the num bytes processed prior to calling the read abort API */
        uart3Obj.rxSize = 0U;
        uart3Obj.rxProcessedSize = 0U;
    }

    return true;
}

void UART3_WriteCallbackRegister( UART_CALLBACK callback, uintptr_t context )
{
    uart3Obj.txCallback = callback;

    uart3Obj.txContext = context;
}

bool UART3_WriteIsBusy( void )
{
    return uart3Obj.txBusyStatus;
}

size_t UART3_WriteCountGet( void )
{
    return uart3Obj.txProcessedSize;
}

static void __attribute__((used)) UART3_FAULT_InterruptHandler (void)
{
    /* Save the error to be reported later */
    uart3Obj.errors = (U3STA & (_U3STA_OERR_MASK | _U3STA_FERR_MASK | _U3STA_PERR_MASK));

    /* Disable the fault interrupt */
    IEC1CLR = _IEC1_U3EIE_MASK;

    /* Disable the receive interrupt */
    IEC1CLR = _IEC1_U3RXIE_MASK;

    /* Clear rx status */
    uart3Obj.rxBusyStatus = false;

    UART3_ErrorClear();

    /* Client must call UARTx_ErrorGet() function to get the errors */
    if( uart3Obj.rxCallback != NULL )
    {
        uintptr_t rxContext = uart3Obj.rxContext;

        uart3Obj.rxCallback(rxContext);
    }
}

static void __attribute__((used)) UART3_RX_InterruptHandler (void)
{
    if(uart3Obj.rxBusyStatus == true)
    {
        size_t rxSize = uart3Obj.rxSize;
        size_t rxProcessedSize = uart3Obj.rxProcessedSize;

        while((_U3STA_URXDA_MASK == (U3STA & _U3STA_URXDA_MASK)) && (rxSize > rxProcessedSize) )
        {
            if (( U3MODE & (_U3MODE_PDSEL0_MASK | _U3MODE_PDSEL1_MASK)) == (_U3MODE_PDSEL0_MASK | _U3MODE_PDSEL1_MASK))
            {
                /* 9-bit mode */
                ((uint16_t*)uart3Obj.rxBuffer)[rxProcessedSize] = (uint16_t)(U3RXREG);
            }
            else
            {
                /* 8-bit mode */
                ((uint8_t*)uart3Obj.rxBuffer)[rxProcessedSize] = (uint8_t)(U3RXREG);
            }
            rxProcessedSize++;
        }

        uart3Obj.rxProcessedSize = rxProcessedSize;

        /* Clear UART3 RX Interrupt flag */
        IFS1CLR = _IFS1_U3RXIF_MASK;

        /* Check if the buffer is done */
        if(uart3Obj.rxProcessedSize >= rxSize)
        {
            uart3Obj.rxBusyStatus = false;

            /* Disable the fault interrupt */
            IEC1CLR = _IEC1_U3EIE_MASK;

            /* Disable the receive interrupt */
            IEC1CLR = _IEC1_U3RXIE_MASK;


            if(uart3Obj.rxCallback != NULL)
            {
                uintptr_t rxContext = uart3Obj.rxContext;

                uart3Obj.rxCallback(rxContext);
            }
        }
    }
    else
    {
        /* Nothing to process */
    }
}

static void __attribute__((used)) UART3_TX_InterruptHandler (void)
{
    if(uart3Obj.txBusyStatus == true)
    {
        size_t txSize = uart3Obj.txSize;
        size_t txProcessedSize = uart3Obj.txProcessedSize;

        while(((U3STA & _U3STA_UTXBF_MASK) == 0U) && (txSize > txProcessedSize) )
        {
            if (( U3MODE & (_U3MODE_PDSEL0_MASK | _U3MODE_PDSEL1_MASK)) == (_U3MODE_PDSEL0_MASK | _U3MODE_PDSEL1_MASK))
            {
                /* 9-bit mode */
                U3TXREG = ((uint16_t*)uart3Obj.txBuffer)[txProcessedSize];
            }
            else
            {
                /* 8-bit mode */
                U3TXREG = ((uint8_t*)uart3Obj.txBuffer)[txProcessedSize];
            }
            txProcessedSize++;
        }

        uart3Obj.txProcessedSize = txProcessedSize;

        /* Clear UART3TX Interrupt flag */
        IFS1CLR = _IFS1_U3TXIF_MASK;

        /* Check if the buffer is done */
        if(uart3Obj.txProcessedSize >= txSize)
        {
            uart3Obj.txBusyStatus = false;

            /* Disable the transmit interrupt, to avoid calling ISR continuously */
            IEC1CLR = _IEC1_U3TXIE_MASK;

            if(uart3Obj.txCallback != NULL)
            {
                uintptr_t txContext = uart3Obj.txContext;

                uart3Obj.txCallback(txContext);
            }
        }
    }
    else
    {
        // Nothing to process
        ;
    }
}

void __attribute__((used)) UART_3_InterruptHandler (void)
{
    /* As per 13_5_violation using this temp variable */
    uint32_t temp = 0;

    temp = IEC1;
    /* Call Error handler if Error interrupt flag is set */
    if (((IFS1 & _IFS1_U3EIF_MASK) != 0U) && ((temp & _IEC1_U3EIE_MASK) != 0U))
    {
        UART3_FAULT_InterruptHandler();
    }
    temp = IEC1;
    /* Call RX handler if RX interrupt flag is set */
    if (((IFS1 & _IFS1_U3RXIF_MASK) != 0U) && ((temp & _IEC1_U3RXIE_MASK) != 0U))
    {
        UART3_RX_InterruptHandler();
    }
    temp = IEC1;
    /* Call TX handler if TX interrupt flag is set */
    if (((IFS1 & _IFS1_U3TXIF_MASK) != 0U) && ((temp & _IEC1_U3TXIE_MASK) != 0U))
    {
        UART3_TX_InterruptHandler();
    }
}


bool UART3_TransmitComplete( void )
{
    bool transmitComplete = false;

    if((U3STA & _U3STA_TRMT_MASK) != 0U)
    {
        transmitComplete = true;
    }

    return transmitComplete;
}