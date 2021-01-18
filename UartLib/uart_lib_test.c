
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "dma.h"
#include "rtc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "user_uart_lib.h"


#define USART_BUFFER_MAX_SIZE               64u


static uint8_t rx_DMA_buffer[USART_BUFFER_MAX_SIZE];
static uint8_t tx_DMA_buffer[USART_BUFFER_MAX_SIZE];
static volatile uint8_t tx_len;
/* USER CODE END Includes */


void test_task(void)
{
    HAL_UART1_Receive_DMA(&huart1, rx_DMA_buffer, USART_BUFFER_MAX_SIZE);

    while(1){
        
        if (tx_len != 0u){                         
            HAL_UART1_Transmit_DMA(&huart1, tx_DMA_buffer, tx_len);
            tx_len = 0; 
        }

    }



}


void UART1_ReceiveDMA_Callback(DMA_HandleTypeDef *hdma)
{
    static size_t old_pos = 0u;
    size_t pos;
    /* Calculate current position in buffer */
    pos = USART_BUFFER_MAX_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_5);
    if (pos != old_pos) {                       /* Check change in received data */
        if (pos > old_pos) {                    /* Current position is over previous one */
            memcpy((char *)&tx_DMA_buffer[0], (char const *)&rx_DMA_buffer[old_pos], pos - old_pos);
            tx_len = pos - old_pos;
        }
        else {
            memcpy((char *)&tx_DMA_buffer[0], (char const *)&rx_DMA_buffer[old_pos], USART_BUFFER_MAX_SIZE - old_pos);
            tx_len = USART_BUFFER_MAX_SIZE - old_pos;     
            /* Check and continue with beginning of buffer */
            if (pos > 0) {
                memcpy((char *)&tx_DMA_buffer[USART_BUFFER_MAX_SIZE - old_pos], (char const *)&rx_DMA_buffer[0], pos);
                tx_len += pos;
            }
           
        }
    }
    old_pos = pos;                              /* Save current position as old */

    /* Check and manually update if we reached end of buffer */
    if (old_pos == USART_BUFFER_MAX_SIZE) {
        old_pos = 0;
    }
}

