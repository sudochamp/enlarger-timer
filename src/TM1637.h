/**
 **********************************************************************************
 * @file   TM1637.h
 * @author Hossein.M (https://github.com/Hossein-M98)
 * @brief  TM1637 chip driver
 *         Functionalities of the this file:
 *          + Display config and control functions
 **********************************************************************************
 *
 * Copyright (c) 2023 Mahda Embedded System (MIT License)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **********************************************************************************
 */
  
/* Define to prevent recursive inclusion ----------------------------------------*/
#ifndef _TM1637_H_
#define _TM1637_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ---------------------------------------------------------------------*/
#include <stdint.h>


/* Exported Constants -----------------------------------------------------------*/
#define TM1637DisplayStateOFF 0
#define TM1637DisplayStateON  1

#define TM1637DecimalPoint    0x80

  
/* Exported Data Types ----------------------------------------------------------*/
/**
 * @brief  Handler data type
 * @note   User must initialize this this functions before using library:
 *         - PlatformInit
 *         - PlatformDeInit
 *         - DioConfigOut
 *         - DioConfigIn
 *         - DioWrite
 *         - DioRead
 *         - ClkWrite
 *         - StbWrite
 *         - DelayUs
 */
typedef struct TM1637_Handler_s
{
  // Initialize the platform-dependent layer
  void (*PlatformInit)(void);
  // Uninitialize the platform-dependent layer
  void (*PlatformDeInit)(void);

  // Config the GPIO that connected to DIO PIN of SHT1x as output
  void (*DioConfigOut)(void);
  // Config the GPIO that connected to DIO PIN of SHT1x as input
  void (*DioConfigIn)(void);
  // Set level of the GPIO that connected to DIO PIN of SHT1x
  void (*DioWrite)(uint8_t);
  // Read the GPIO that connected to DIO PIN of SHT1x
  uint8_t (*DioRead)(void);

  // Set level of the GPIO that connected to CLK PIN of SHT1x
  void (*ClkWrite)(uint8_t);

  // Delay (us)
  void (*DelayUs)(uint8_t);
} TM1637_Handler_t;


/**
 * @brief  Data type of library functions result
 */
typedef enum TM1637_Result_e
{
  TM1637_OK      = 0,
  TM1637_FAIL    = -1
} TM1637_Result_t;



/**
 ==================================================================================
                           ##### Common Functions #####                           
 ==================================================================================
 */

/**
 * @brief  Initialize TM1637.
 * @param  Handler: Pointer to handler      
 * @retval TM1637_Result_t
 *         - TM1637_OK: Operation was successful.
 */
TM1637_Result_t
TM1637_Init(TM1637_Handler_t *Handler);


/**
 * @brief  De-Initialize TM1637.
 * @param  Handler: Pointer to handler
 * @retval TM1637_Result_t
 *         - TM1637_OK: Operation was successful.
 */
TM1637_Result_t
TM1637_DeInit(TM1637_Handler_t *Handler);



/**
 ==================================================================================
                           ##### Display Functions #####                           
 ==================================================================================
 */

/**
 * @brief  Config display parameters
 * @param  Handler: Pointer to handler
 * @param  Brightness: Set brightness level
 *         - 0: Display pulse width is set as 1/16
 *         - 1: Display pulse width is set as 2/16
 *         - 2: Display pulse width is set as 4/16
 *         - 3: Display pulse width is set as 10/16
 *         - 4: Display pulse width is set as 11/16
 *         - 5: Display pulse width is set as 12/16
 *         - 6: Display pulse width is set as 13/16
 *         - 7: Display pulse width is set as 14/16
 * 
 * @param  DisplayState: Set display ON or OFF
 *         - TM1637DisplayStateOFF: Set display state OFF
 *         - TM1637DisplayStateON: Set display state ON
 * 
 * @retval TM1637_Result_t
 *         - TM1637_OK: Operation was successful
 */
TM1637_Result_t
TM1637_ConfigDisplay(TM1637_Handler_t *Handler,
                     uint8_t Brightness, uint8_t DisplayState);


/**
 * @brief  Set data to single digit in 7-segment format
 * @param  Handler: Pointer to handler
 * @param  DigitData: Digit data
 * @param  DigitPos: Digit position
 *         - 0: Seg1
 *         - 1: Seg2
 *         - .
 *         - .
 *         - .
 * 
 * @retval TM1637_Result_t
 *         - TM1637_OK: Operation was successful
 */
TM1637_Result_t
TM1637_SetSingleDigit(TM1637_Handler_t *Handler,
                      uint8_t DigitData, uint8_t DigitPos);


/**
 * @brief  Set data to multiple digits in 7-segment format
 * @param  Handler: Pointer to handler
 * @param  DigitData: Array to Digits data
 * @param  StartAddr: First digit position
 *         - 0: Seg1
 *         - 1: Seg2
 *         - .
 *         - .
 *         - .
 * 
 * @param  Count: Number of segments to write data
 * @retval TM1637_Result_t
 *         - TM1637_OK: Operation was successful
 */
// TM1637_Result_t
// TM1637_SetMultipleDigit(TM1637_Handler_t *Handler, const uint8_t *DigitData,
//                         uint8_t StartAddr, uint8_t Count);


/**
 * @brief  Set data to multiple digits in 7-segment format
 * @param  Handler: Pointer to handler
 * @param  DigitData: Digit data (0, 1, ... , 15, a, A, b, B, ... , f, F) 
 * @param  DigitPos: Digit position
 *         - 0: Seg1
 *         - 1: Seg2
 *         - .
 *         - .
 *         - .
 * 
 * @retval TM1637_Result_t
 *         - TM1637_OK: Operation was successful
 */
TM1637_Result_t
TM1637_SetSingleDigit_HEX(TM1637_Handler_t *Handler,
                          uint8_t DigitData, uint8_t DigitPos);


/**
 * @brief  Set data to multiple digits in hexadecimal format
 * @param  Handler: Pointer to handler
 * @param  DigitData: Array to Digits data. 
 *                    (0, 1, ... , 15, a, A, b, B, ... , f, F)
 * 
 * @param  StartAddr: First digit position
 *         - 0: Seg1
 *         - 1: Seg2
 *         - .
 *         - .
 *         - .
 * 
 * @param  Count: Number of segments to write data
 * @retval TM1637_Result_t
 *         - TM1637_OK: Operation was successful
 */
// TM1637_Result_t
// TM1637_SetMultipleDigit_HEX(TM1637_Handler_t *Handler, const uint8_t *DigitData,
//                             uint8_t StartAddr, uint8_t Count);



#ifdef __cplusplus
}
#endif

#endif //! _TM1638_H_