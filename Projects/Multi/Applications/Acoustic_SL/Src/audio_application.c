/**
******************************************************************************
* @file    audio_application.c 
* @author  Central Labs
* @version V3.0.0
* @date    21-March-2018
* @brief   Source localization example. 
*******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
*
* Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at:
*
*        http://www.st.com/software_license_agreement_liberty_v2
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "audio_application.h"
#include "acoustic_sl.h"

/** @addtogroup X_CUBE_MEMSMIC1_Applications
* @{
*/

/** @addtogroup ACOUSTIC_SL_Example
* @{
*/ 
  
/** @defgroup AUDIO_APPLICATION 
* @{
*/

/** @defgroup AUDIO_APPLICATION_Exported_Variables 
* @{
*/
uint16_t PDM_Buffer[AUDIO_CHANNELS*AUDIO_SAMPLING_FREQUENCY/1000*80/8];
/**
* @}
*/

/** @defgroup AUDIO_APPLICATION_Private_Variables 
* @{
*/
/*Handler and Config structure for Source Localization*/
AcousticSL_Handler_t libSoundSourceLoc_Handler_Instance;
AcousticSL_Config_t  libSoundSourceLoc_Config_Instance;
volatile int16_t  SOURCE_Angle_Value = 0;
volatile uint16_t SOURCE_Enable_Value = 1;
UART_HandleTypeDef UartHandle;
volatile int32_t result[2];
/* Private variables ---------------------------------------------------------*/
static uint16_t PCM_Buffer[AUDIO_CHANNELS*AUDIO_SAMPLING_FREQUENCY/1000];
/**
* @}
*/

/** @defgroup AUDIO_APPLICATION_Exported_Function 
* @{
*/
/**
* @brief  Half Transfer user callback, called by BSP functions.
* @param  None
* @retval None
*/
void BSP_AUDIO_IN_HalfTransfer_CallBack(void)
{
  AudioProcess();
}

/**
* @brief  Transfer Complete user callback, called by BSP functions.
* @param  None
* @retval None
*/
void BSP_AUDIO_IN_TransferComplete_CallBack(void)
{
  AudioProcess();
}

/**
* @brief  User function that is called when 1 ms of PDM data is available.
* 		  In this application only PDM to PCM conversion and USB streaming
*                  is performed.
* 		  User can add his own code here to perform some DSP or audio analysis.
*                In this case audio libraries are used 
* @param  none
* @retval None
*/
void AudioProcess(void) 
{
  BSP_AUDIO_IN_PDMToPCM((uint16_t * )PDM_Buffer,PCM_Buffer);
  if(AcousticSL_Data_Input((int16_t *)&PCM_Buffer[BOTTOM_LEFT_MIC], (int16_t *)&PCM_Buffer[TOP_RIGHT_MIC],
                                  (int16_t *)&PCM_Buffer[BOTTOM_RIGHT_MIC], (int16_t *)&PCM_Buffer[TOP_LEFT_MIC], &libSoundSourceLoc_Handler_Instance))
  {   
    SW_Task2_Start(); /*Localization Processing Task*/
  }    
  
  Send_Audio_to_USB((int16_t *)PCM_Buffer, AUDIO_SAMPLING_FREQUENCY/1000*AUDIO_CHANNELS);
}

/**
* @brief  Initialize the audio libraries adopted
* @param  None
* @retval None
*/
char version[35];
 volatile uint32_t error_value = 0;
void Audio_Libraries_Init(void)
{
 error_value = 0;
  /* Enable CRC peripheral to unlock the library */
  __CRC_CLK_ENABLE();

   AcousticSL_GetLibVersion(version);
  
  /*Setup Source Localization static parameters*/
  libSoundSourceLoc_Handler_Instance.channel_number = 4;
  libSoundSourceLoc_Handler_Instance.M12_distance =DIAGONAL;
  libSoundSourceLoc_Handler_Instance.M34_distance =DIAGONAL;
  libSoundSourceLoc_Handler_Instance.sampling_frequency = AUDIO_SAMPLING_FREQUENCY;  
  libSoundSourceLoc_Handler_Instance.algorithm = ACOUSTIC_SL_ALGORITHM_GCCP;
  libSoundSourceLoc_Handler_Instance.ptr_M1_channels = 4;
  libSoundSourceLoc_Handler_Instance.ptr_M2_channels = 4;
  libSoundSourceLoc_Handler_Instance.ptr_M3_channels = 4;
  libSoundSourceLoc_Handler_Instance.ptr_M4_channels = 4;  
  libSoundSourceLoc_Handler_Instance.samples_to_process = 512;  
  AcousticSL_getMemorySize( &libSoundSourceLoc_Handler_Instance);  
  libSoundSourceLoc_Handler_Instance.pInternalMemory=(uint32_t *)malloc(libSoundSourceLoc_Handler_Instance.internal_memory_size);
  error_value += AcousticSL_Init( &libSoundSourceLoc_Handler_Instance);
  
  /*Setup Source Localization dynamic parameters*/  
  libSoundSourceLoc_Config_Instance.resolution=10;
  libSoundSourceLoc_Config_Instance.threshold=24;  
  error_value += AcousticSL_setConfig(&libSoundSourceLoc_Handler_Instance, &libSoundSourceLoc_Config_Instance);
  
  /*Error Management*/
  if(error_value != 0){
    while(1);     
  }  
  
  /*Malloc Failure*/
  if(libSoundSourceLoc_Handler_Instance.pInternalMemory == NULL)
  {
    while(1); /*Malloc Failure*/
  } 
  
}

/**
* @brief  Initializes two SW interrupt with different priorities
* @param  None
* @retval None
*/
void SW_IRQ_Tasks_Init(void){
  
  HAL_NVIC_SetPriority((IRQn_Type)EXTI1_IRQn, 0x0D, 0);  
  HAL_NVIC_EnableIRQ((IRQn_Type)EXTI1_IRQn);  
  
  HAL_NVIC_SetPriority((IRQn_Type)EXTI2_IRQn, 0x0E, 0);  
  HAL_NVIC_EnableIRQ((IRQn_Type)EXTI2_IRQn); 
  
  
}

/**
* @brief  Initializes uart communications
* @param  None
* @retval None
*/
void Uart_Init(void){
  
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  USARTx_TX_GPIO_CLK_ENABLE();
  USARTx_RX_GPIO_CLK_ENABLE();
  
  /* Enable USARTx clock */
  USARTx_CLK_ENABLE(); 
  
  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = USARTx_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
  GPIO_InitStruct.Alternate = USARTx_TX_AF;
  
  HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);
  
  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = USARTx_RX_PIN;
  GPIO_InitStruct.Alternate = USARTx_RX_AF;
  
  HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct); 
  UartHandle.Instance          = USARTx;  
  UartHandle.Init.BaudRate     = 9600;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
  
  HAL_UART_Init(&UartHandle);
  
  
  
}

/**
* @brief  Highest priority interrupt handler routine
* @param  None
* @retval None
*/
void SW_Task1_Callback(void){
  
}

/**
* @brief Lower priority interrupt handler routine
* @param  None
* @retval None
*/

void SW_Task2_Callback(void){
  
  
  AcousticSL_Process((int32_t *)&result, &libSoundSourceLoc_Handler_Instance);
  
  if(result[0]==ACOUSTIC_SL_NO_AUDIO_DETECTED){
    result[0] = -1;
  }
  
  if(result[0] != -1)
  {
  char output[4];
  int n = sprintf(output,"%i",result[0]);
  HAL_UART_Transmit(&UartHandle, (uint8_t *)"    ", 4, 0xFFFF);
  HAL_UART_Transmit(&UartHandle, (uint8_t *)"\r", 1, 0xFFFF); 
  HAL_UART_Transmit(&UartHandle, (uint8_t *)output, n, 0xFFFF);    
  }  
}

/**
* @brief Throws Highest priority interrupt
* @param  None
* @retval None
*/
void SW_Task1_Start(void){
  
  HAL_NVIC_SetPendingIRQ(EXTI1_IRQn);
  
}

void SW_Task2_Start(void){
  
  HAL_NVIC_SetPendingIRQ(EXTI2_IRQn);
  
}

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
