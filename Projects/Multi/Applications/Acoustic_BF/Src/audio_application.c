/**
******************************************************************************
* @file    audio_application.c 
* @author  Central Labs
* @version V3.0.0
* @date    21-March-2018
* @brief   Beam forming example. 
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
#include "acoustic_bf.h"


/** @addtogroup X_CUBE_MEMSMIC1_Applications
* @{
*/

/** @addtogroup ACOUSTIC_BF_Example
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
/*Handler and Config structure for BeamForming*/
AcousticBF_Handler_t libBeamforming_Handler_Instance;
AcousticBF_Config_t lib_Beamforming_Config_Instance;
int16_t USBOUT[16*4];
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
    if(AcousticBF_FirstStep(&((uint8_t *)(PDM_Buffer))[0],&((uint8_t *)(PDM_Buffer))[1], (int16_t *)USBOUT, &libBeamforming_Handler_Instance))
    {
      SW_Task1_Start();          
    } 
 
  Send_Audio_to_USB((int16_t *)USBOUT, AUDIO_SAMPLING_FREQUENCY/1000*AUDIO_CHANNELS);
}


/**
* @brief  Initialize the audio libraries adopted
* @param  None
* @retval None
*/
void Audio_Libraries_Init(void)
{ 

  volatile uint32_t error_value = 0;
  /* Enable CRC peripheral to unlock the library */
  __CRC_CLK_ENABLE();  
  /*Setup Beamforming static parameters*/
  error_value = 0;  
  libBeamforming_Handler_Instance.algorithm_type_init=ACOUSTIC_BF_TYPE_STRONG;
  libBeamforming_Handler_Instance.ref_mic_enable=ACOUSTIC_BF_REF_ENABLE;
  libBeamforming_Handler_Instance.ptr_out_channels=2;
  libBeamforming_Handler_Instance.data_format=ACOUSTIC_BF_DATA_FORMAT_PDM;
  libBeamforming_Handler_Instance.sampling_frequency=1280;  
  libBeamforming_Handler_Instance.ptr_M1_channels=2;
  libBeamforming_Handler_Instance.ptr_M2_channels=2;
  libBeamforming_Handler_Instance.delay_enable = 1;
  AcousticBF_getMemorySize(&libBeamforming_Handler_Instance);
  libBeamforming_Handler_Instance.pInternalMemory =(uint32_t *)malloc(libBeamforming_Handler_Instance.internal_memory_size);
  error_value = AcousticBF_Init(&libBeamforming_Handler_Instance);
  
  /*Error Management*/
  if(error_value != 0){
    while(1); /*Error Management*/    
  }  
  
  /*Malloc Failure*/
  if(libBeamforming_Handler_Instance.pInternalMemory == NULL)
  {
    while(1); /*Malloc Failure*/
  } 

  
  /*Setup Beamforming dynamic parameters*/
  lib_Beamforming_Config_Instance.algorithm_type = ACOUSTIC_BF_TYPE_STRONG;
  lib_Beamforming_Config_Instance.M2_gain=0;  
  lib_Beamforming_Config_Instance.mic_distance=150;  
  lib_Beamforming_Config_Instance.volume = 24;
  error_value = AcousticBF_setConfig(&libBeamforming_Handler_Instance, &lib_Beamforming_Config_Instance);
  
  /*Error Management*/
  if(error_value != 0){
    while(1); /*Error Management*/    
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
* @brief  Highest priority interrupt handler routine
* @param  None
* @retval None
*/
void SW_Task1_Callback(void)
{	
AcousticBF_SecondStep(&libBeamforming_Handler_Instance); 
}

/**
* @brief Lower priority interrupt handler routine
* @param  None
* @retval None
*/
void SW_Task2_Callback(void)
{ 
}

/**
* @brief Throws Highest priority interrupt
* @param  None
* @retval None
*/
void SW_Task1_Start(void)
{  
  HAL_NVIC_SetPendingIRQ(EXTI1_IRQn);  
}

/**
* @brief Throws Lower priority interrupt
* @param  None
* @retval None
*/
void SW_Task2_Start(void)
{  
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
