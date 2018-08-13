/**
  @page BeamformingApplication Beamforming Application
  
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    readme.txt  
  * @author  Central Labs
  * @version V3.0.0
  * @date    21-March-2018
  * @brief   Description of the Beamforming application example.
  ******************************************************************************
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
  ******************************************************************************
  @endverbatim

@par Application Description 

This document provides a description of the sound source localization application based 
on Nucleo boards and X-NUCLEO-CCA02M1 expansion board. It allows acquisition of 
2 digital MEMS microphones that are used to run beamforming library in order to  
create a directional virtual microphone. The audio is then streamed via the 
X-NUCLEO-CCA02M1 usb port to an host PC running a standard audio editing tool.

NOTE: to fully take advantage of this example, only the two on-board microphone are used.
Solder bridge configuration must be set for 2 microphone acquisition, as depicted in the 
X-CUBE-MEMSMIC1 user manual.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Directory contents 

  - Acoustic_BF/Src/stm32xx_it.c          Interrupt handlers file for STM32F4
  - Acoustic_BF/Src/system_stm32f4xx.c      STM32F4xx system clock configuration file
  - Acoustic_BF/Src/usbd_audio_if.c		    USBD Audio-input interface  
  - Acoustic_BF/Src/usbd_conf_f4.c    		USB device driver Configuration file for STM32F4
  - Acoustic_BF/Src/usbd_desc.c    			USB device AUDIO-input descriptor   
  - Acoustic_BF/Src/cube_hal_f4.c           Clock configuration file for STM32F4
  - Acoustic_BF/Src/main.c                  Main program	
  - Acoustic_BF/Src/audio_application.c  	Audio application file
  
  - Acoustic_BF/Inc/stm32f4xx_hal_conf.h    HAL configuration file for STM32F4
  - Acoustic_BF/Inc/stm32xx_it.h          Interrupt handlers header file for STM32F4
  - Acoustic_BF/Inc/main.h                  Main program header file
  - Acoustic_BF/Inc/cube_hal.h			    X-CUBE includes file
  - Acoustic_BF/Inc/audio_application.h  	Application header file
  - Acoustic_BF/Inc/usbd_audio_if.h		    USBD Audio-input interface header file  
  - Acoustic_BF/Inc/usbd_conf.h    			USB device driver Configuration file
  - Acoustic_BF/Inc/usbd_desc.h    			USB device AUDIO-input descriptor header file   	


@par Hardware and Software environment

  - This example runs on STM32F401RE.
    
  - This example has been tested with STMicroelectronics STM32F4xx-Nucleo RevB 
    boards and can be easily tailored to any other supported device 
    and development board.
    

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
