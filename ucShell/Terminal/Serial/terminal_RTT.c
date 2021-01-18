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


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
//#define RX_BUF_SIZE             256u

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


static TX_SEMAPHORE rdByte_sem;

static TX_TIMER rdData_timer;
static int readCtr = 0u;


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static void timer_receive_data(ULONG data);

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
 

/*
"my_timer_function" after 100 ticks initially and then 
after every 25 ticks. This timer is specified to start 
immediately! */ 
    status = tx_timer_create(&rdData_timer,"read data timer", 
        timer_receive_data, (ULONG)&readCtr, 50, 300, 
        TX_AUTO_ACTIVATE); 
/* If status equals TX_SUCCESS, my_timer_function will 
be called 100 timer ticks later and then called every 
25 timer ticks. Note that the value 0x1234 is passed to 
my_timer_function every time it is called. */ 
    if ( status != TX_SUCCESS){
        Error_Handler();
    
    }



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
    status = tx_timer_delete(&rdData_timer); 
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

    SEGGER_RTT_Write(0,  pbuf, buf_len);

    return (-1);
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
    int c;
    c = SEGGER_RTT_GetKey();
    if ( c  == -1 ){ 
        status = tx_timer_activate(&rdData_timer);    
        if ((status != TX_SUCCESS)&&( status != TX_ACTIVATE_ERROR )){ 
          SEGGER_RTT_printf(0, "tx_timer_activate status vlaue: << %u >>\r\n", status);
            Error_Handler();
        }
        status = tx_semaphore_get(&rdByte_sem, TX_WAIT_FOREVER); 
        if (status != TX_SUCCESS){
            Error_Handler();
        }
       return(0u);
    }
    return ((char)c);
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

     SEGGER_RTT_Write(0,  &c, 1u);


}




static void timer_receive_data(ULONG data)
{
 int r;
 UINT status;
//  0.3秒读一下数据.
//  将接收到的数据全部传给线程处理.

    r = SEGGER_RTT_HasKey();
    if (r) {
        status = tx_semaphore_put(&rdByte_sem);
        if (status != TX_SUCCESS){
            Error_Handler();
        } 
        status = tx_timer_deactivate(&rdData_timer);   
        if (status != TX_SUCCESS){
            Error_Handler();
        }    
    }


}


