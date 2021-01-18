/*
*********************************************************************************************************
*                                              uC/Shell
*                                            Shell utility
*
*                    Copyright 2007-2020 Silicon Laboratories Inc. www.silabs.com
*
*                                 SPDX-License-Identifier: APACHE-2.0
*
*               This software is subject to an open source license and is distributed by
*                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
*                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                              TERMINAL
*
*                                    TEMPLATE COMMUNICATIONS PORT
*
* Filename : terminal_serial.c
* Version  : V1.04.00
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <terminal.h>


#include "sample_cfg.h"


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#include  <lib_str.h>
#include "user_uart_lib.h"


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static uint8_t rx_DMA_buffer[TERMINAL_CFG_MAX_CMD_LEN];
static uint8_t read_buffer[TERMINAL_CFG_MAX_CMD_LEN];
static volatile uint8_t tx_len;

static TX_SEMAPHORE rdByte_sem;

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                        TerminalSerial_Init()
*
* Description : Initialize serial communications.
*
* Argument(s) : none.
*
* Return(s)   : DEF_OK,   if interface was opened.
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Terminal_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  TerminalSerial_Init (void)
{   
    UINT status; 

    /* Create a counting semaphore whose initial value is 1. 
    This is typically the technique used to make a binary 
    semaphore. Binary semaphores are used to provide
    protection over a common resource. */ 
    status = tx_semaphore_create(&rdByte_sem, "Serial_byte_input_semaphore", 0);  

    /* If status equals TX_SUCCESS, my_semaphore is ready for use. */ 
    if (status != TX_SUCCESS){
        Error_Handler();

    }
    HAL_UART1_Receive_DMA(&huart1, rx_DMA_buffer, TERMINAL_CFG_MAX_CMD_LEN);
    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                        TerminalSerial_Exit()
*
* Description : Uninitialize serial communications.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Terminal_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  TerminalSerial_Exit (void)
{
    UINT status; 
    
    /* Delete counting semaphore. Assume that the counting 
    semaphore has already been created. */ 
    status = tx_semaphore_delete(&rdByte_sem); 
    
    /* If status equals TX_SUCCESS, the counting semaphore is  deleted. */ 
    if (status != TX_SUCCESS){
        Error_Handler();
    }

}


/*
*********************************************************************************************************
*                                         TerminalSerial_Wr()
*
* Description : Serial output.
*
* Argument(s) : pbuf        Pointer to the buffer to transmit.
*
*               buf_len     Number of bytes in the buffer.
*
* Return(s)   : none.
*
* Caller(s)   : Terminal_Out().
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT16S  TerminalSerial_Wr (void        *pbuf,
                               CPU_SIZE_T   buf_len)
{  
    HAL_UART1_Transmit_DMA(&huart1, pbuf, buf_len);
    return (0);
}


/*
*********************************************************************************************************
*                                       TerminalSerial_RdByte()
*
* Description : Serial byte input.
*
* Argument(s) : none.
*
* Return(s)   : Byte read from port.
*
* Caller(s)   : various.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT08U  TerminalSerial_RdByte (void)
{
    UINT status;
    static uint8_t c;
    if (tx_len == 0u){        
        status = tx_semaphore_get(&rdByte_sem, TX_WAIT_FOREVER); 
        if (status != TX_SUCCESS){
            Error_Handler();
        }
        c = 0u;
    }
    tx_len--;
    return (read_buffer[c++]);
}



/*
*********************************************************************************************************
*                                       TerminalSerial_WrByte()
*
* Description : Serial byte output.
*
* Argument(s) : c           Byte to write.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  TerminalSerial_WrByte (CPU_INT08U  c)
{

    HAL_UART1_Transmit_DMA(&huart1, &c, 1u);
        

}




void UART1_ReceiveDMA_Callback(DMA_HandleTypeDef *hdma)
{
    static size_t old_pos = 0u;
    size_t pos;
    /* Calculate current position in buffer */
    pos = TERMINAL_CFG_MAX_CMD_LEN - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_5);
    if (pos != old_pos) {                       /* Check change in received data */
        if (pos > old_pos) {                    /* Current position is over previous one */
            memcpy((char *)&read_buffer[0], (char const *)&rx_DMA_buffer[old_pos], pos - old_pos);
            tx_len = pos - old_pos;
        }
        else {
            memcpy((char *)&read_buffer[0], (char const *)&rx_DMA_buffer[old_pos], TERMINAL_CFG_MAX_CMD_LEN - old_pos);
            tx_len = TERMINAL_CFG_MAX_CMD_LEN - old_pos;     
            /* Check and continue with beginning of buffer */
            if (pos > 0) {
                memcpy((char *)&read_buffer[TERMINAL_CFG_MAX_CMD_LEN - old_pos], (char const *)&rx_DMA_buffer[0], pos);
                tx_len += pos;
            }
           
        }
        UINT status = tx_semaphore_put(&rdByte_sem);
        if (status != TX_SUCCESS){
            Error_Handler();
        }         
        old_pos = pos;                              /* Save current position as old */
        /* Check and manually update if we reached end of buffer */
        if (old_pos == TERMINAL_CFG_MAX_CMD_LEN) {
            old_pos = 0;
        }        
    }
    
}


