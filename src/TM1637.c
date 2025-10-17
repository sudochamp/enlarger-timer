/**
 **********************************************************************************
 * @file   TM1637.c
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

/* Includes ---------------------------------------------------------------------*/
#include "TM1637.h"

/* Private Constants ------------------------------------------------------------*/
/**
 * @brief  Command description
 */
#define DataCommandSetting      0x40 // 0b01000000
#define DisplayControl          0x80 // 0b10000000
#define AddressCommandSetting   0xC0 // 0b11000000

/**
 * @brief  Data Command Setting
 */
#define WriteDataToDisplayRegister    0x00  // 0b00000000
#define ReadKeyScanData               0x02  // 0b00000010
#define AutomaticAddressAdd           0x00  // 0b00000000
#define FixedAddress                  0x04  // 0b00000100
#define NormalMode                    0x00  // 0b00000000
#define TestMode                      0x08  // 0b00001000

/**
 * @brief  Display ControlInstruction Set
 */
#define ShowTurnOff   0x00  // 0b00000000
#define ShowTurnOn    0x08  // 0b00001000

/**
 * @brief  Communication delay
 */
#define CommunicationDelayUs   5

/**
 * @brief  Maximum number of digits
 */
#define MaxNumOfDigits 6


/* Private variables ------------------------------------------------------------*/
/**
 * @brief  Convert HEX number to Seven-Segment code
 */
const uint8_t HexTo7Seg[16] =
{
  0x3F, // 0
  0x06, // 1
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F, // 9
  0x77, // A
  0x7c, // b
  0x39, // C
  0x5E, // d
  0x79, // E
  0x71  // F
};


/**
 ==================================================================================
                           ##### Private Functions #####                           
 ==================================================================================
 */

static inline void
TM1637_StartCommunication(TM1637_Handler_t *Handler)
{
  Handler->DioConfigOut();
  Handler->DioWrite(1);
  Handler->ClkWrite(1);
  Handler->DelayUs(CommunicationDelayUs);
  Handler->DioWrite(0);
}

static inline void
TM1637_StopCommunication(TM1637_Handler_t *Handler)
{
  Handler->DioConfigOut();

  Handler->ClkWrite(0);
  Handler->DioWrite(0);
  Handler->DelayUs(CommunicationDelayUs);

  Handler->ClkWrite(1);
  Handler->DelayUs(CommunicationDelayUs);

  Handler->DioWrite(1);
}

static int8_t
TM1637_WriteBytes(TM1637_Handler_t *Handler,
                  const uint8_t *Data, uint8_t NumOfBytes)
{
  int8_t Result = 0;
  uint8_t Buff = 0;
  uint8_t ack = 0;

  for (uint8_t j = 0; j < NumOfBytes; j++)
  {
    Handler->DioConfigOut();

    Buff = Data[j];
    for (uint8_t i = 0; i < 8; ++i)
    {
      Handler->ClkWrite(0); // (i)th clock falling edge
      Handler->DioWrite(Buff & 0x01);
      Handler->DelayUs(CommunicationDelayUs);
      Handler->ClkWrite(1); // (i+1)th clock rising edge
      Handler->DelayUs(CommunicationDelayUs);

      Buff >>= 1;
    }

    Handler->ClkWrite(0);  // 8th clock falling edge
    Handler->DioConfigIn();
    Handler->DioWrite(1);
    Handler->DelayUs(CommunicationDelayUs);
    ack = Handler->DioRead(); // Read ACK
    
    // 9th clock to finish the ACK
    Handler->ClkWrite(1);
    Handler->DelayUs(CommunicationDelayUs);
    Handler->ClkWrite(0);

    if (ack) // If ACK is HIGH, then the chip has not received the data
    {
      Result = -1;
      break;
    }
    Result = 0;
  }

  Handler->DelayUs(CommunicationDelayUs);
  return Result;
}

static int8_t
TM1637_SetMultipleDisplayRegister(TM1637_Handler_t *Handler,
                                  const uint8_t *DigitData,
                                  uint8_t StartAddr, uint8_t Count)
{
  int8_t Result = 0;
  uint8_t Data = DataCommandSetting | WriteDataToDisplayRegister |
                 AutomaticAddressAdd | NormalMode;

  TM1637_StartCommunication(Handler);
  Result = TM1637_WriteBytes(Handler, &Data, 1);
  TM1637_StopCommunication(Handler);
  if (Result < 0)
    return -1;

  if (StartAddr >= MaxNumOfDigits)
    return -1;

  if (StartAddr + Count > MaxNumOfDigits)
    Count = 6 - StartAddr;

  Data = AddressCommandSetting | StartAddr;

  Result = 0;
  TM1637_StartCommunication(Handler);
  Result |= TM1637_WriteBytes(Handler, &Data, 1);
  Result |= TM1637_WriteBytes(Handler, DigitData, Count);
  TM1637_StopCommunication(Handler);
  if (Result < 0)
    return -2;

  return 0;
}



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
TM1637_Init(TM1637_Handler_t *Handler)
{
  Handler->PlatformInit();
  return TM1637_OK;
}

/**
 * @brief  De-Initialize TM1637.
 * @param  Handler: Pointer to handler
 * @retval TM1637_Result_t
 *         - TM1637_OK: Operation was successful.
 */
TM1637_Result_t
TM1637_DeInit(TM1637_Handler_t *Handler)
{
  Handler->PlatformDeInit();
  return TM1637_OK;
}

/**
 ==================================================================================
                        ##### Public Display Functions #####                       
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
                     uint8_t Brightness, uint8_t DisplayState)
{
  uint8_t Data = DisplayControl;
  
  Data |= Brightness & 0x07;
  Data |= (DisplayState) ? (ShowTurnOn) : (ShowTurnOff);

  TM1637_StartCommunication(Handler);
  TM1637_WriteBytes(Handler, &Data, 1);
  TM1637_StopCommunication(Handler);

  return TM1637_OK;
}

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
                      uint8_t DigitData, uint8_t DigitPos)
{
  if (TM1637_SetMultipleDisplayRegister(Handler, &DigitData, DigitPos, 1) < 0)
    return TM1637_FAIL;

  return TM1637_OK;
}

// /**
//  * @brief  Set data to multiple digits in 7-segment format
//  * @param  Handler: Pointer to handler
//  * @param  DigitData: Array to Digits data
//  * @param  StartAddr: First digit position
//  *         - 0: Seg1
//  *         - 1: Seg2
//  *         - .
//  *         - .
//  *         - .
//  * 
//  * @param  Count: Number of segments to write data
//  * @retval TM1637_Result_t
//  *         - TM1637_OK: Operation was successful
//  */
TM1637_Result_t
TM1637_SetMultipleDigit(TM1637_Handler_t *Handler, const uint8_t *DigitData,
                        uint8_t StartAddr, uint8_t Count)
{
  if (TM1637_SetMultipleDisplayRegister(Handler, DigitData, StartAddr, Count) < 0)
    return TM1637_FAIL;

  return TM1637_OK;
}

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
                          uint8_t DigitData, uint8_t DigitPos)
{
  uint8_t DigitDataHEX = 0;
  uint8_t DecimalPoint = DigitData & 0x80;

  DigitData &= 0x7F;

  if (DigitData <= 15)
  {
    DigitDataHEX = HexTo7Seg[DigitData] | DecimalPoint;
  }
  else
  {
    switch (DigitData)
    {
    case 'A':
    case 'a':
      DigitDataHEX = HexTo7Seg[0x0A] | DecimalPoint;
      break;

    case 'B':
    case 'b':
      DigitDataHEX = HexTo7Seg[0x0B] | DecimalPoint;
      break;

    case 'C':
    case 'c':
      DigitDataHEX = HexTo7Seg[0x0C] | DecimalPoint;
      break;

    case 'D':
    case 'd':
      DigitDataHEX = HexTo7Seg[0x0D] | DecimalPoint;
      break;

    case 'E':
    case 'e':
      DigitDataHEX = HexTo7Seg[0x0E] | DecimalPoint;
      break;

    case 'F':
    case 'f':
      DigitDataHEX = HexTo7Seg[0x0F] | DecimalPoint;
      break;

    default:
      DigitDataHEX = 0;
      break;
    }
  }

  return TM1637_SetSingleDigit(Handler, DigitDataHEX, DigitPos);
}


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
//                             uint8_t StartAddr, uint8_t Count)
// {
//   uint8_t DigitDataHEX[MaxNumOfDigits];
//   uint8_t DecimalPoint = 0;

//   for (uint8_t i = 0; i < Count && i < MaxNumOfDigits; i++)
//   {
//     DecimalPoint = DigitData[i] & 0x80;

//     if ((DigitData[i] & 0x7F) >= 0 && (DigitData[i] & 0x7F) <= 15)
//     {
//       DigitDataHEX[i] = HexTo7Seg[DigitData[i] & 0x7F] | DecimalPoint;
//     }
//     else
//     {
//       switch (DigitData[i] & 0x7F)
//       {
//       case 'A':
//       case 'a':
//         DigitDataHEX[i] = HexTo7Seg[0x0A] | DecimalPoint;
//         break;

//       case 'B':
//       case 'b':
//         DigitDataHEX[i] = HexTo7Seg[0x0B] | DecimalPoint;
//         break;

//       case 'C':
//       case 'c':
//         DigitDataHEX[i] = HexTo7Seg[0x0C] | DecimalPoint;
//         break;

//       case 'D':
//       case 'd':
//         DigitDataHEX[i] = HexTo7Seg[0x0D] | DecimalPoint;
//         break;

//       case 'E':
//       case 'e':
//         DigitDataHEX[i] = HexTo7Seg[0x0E] | DecimalPoint;
//         break;

//       case 'F':
//       case 'f':
//         DigitDataHEX[i] = HexTo7Seg[0x0F] | DecimalPoint;
//         break;

//       default:
//         DigitDataHEX[i] = 0;
//         break;
//       }
//     }
//   }

//   return TM1637_SetMultipleDigit(Handler,
//                                  (const uint8_t *)DigitDataHEX, StartAddr, Count);
// }