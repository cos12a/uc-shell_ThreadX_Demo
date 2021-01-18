/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_UART_LIB_H
#define __USER_UART_LIB_H

 /* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "main.h"
#include "string.h"

#ifdef __cplusplus
 extern "C" {
#endif




void UART1_DMATransmitCplt(DMA_HandleTypeDef *hdma);
void UART1_DMAError(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef User_DMA_Abort_IT(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_UART1_Transmit_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART1_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_DMA_Tx_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMA_Rx_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
void HAL_DMA_Rx_IRQHandler(DMA_HandleTypeDef *hdma);
void HAL_DMA_Tx_IRQHandler(DMA_HandleTypeDef *hdma);
void HAL_UART1_IRQHandler(UART_HandleTypeDef *huart);

void UART1_ReceiveDMA_Callback(DMA_HandleTypeDef *hdma);


    
/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern UART_HandleTypeDef huart1;




#ifdef __cplusplus
}
#endif

#endif /* __STM32_ASSERT_H */

