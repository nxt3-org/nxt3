//
// Date init       14.12.2004
//
// Revision date   $Date:: 24-09-08 15:23                                    $
//
// Filename        $Workfile:: c_input.iom                                   $
//
// Version         $Revision:: 16                                            $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/c_inpu $
//
// Platform        C
//

#ifndef   CINPUT_IOM
#define   CINPUT_IOM

#include "hal_adc_defs.h"

typedef   struct
{
  UWORD   CustomZeroOffset;    /* Set the offset of the custom sensor  */
  UWORD   ADRaw;
  UWORD   SensorRaw;
  SWORD   SensorValue;

  UBYTE   SensorType;
  UBYTE   SensorMode;
  UBYTE   SensorBoolean;

  UBYTE   DigiPinsDir;         /* Direction of the Digital pins 1 is output 0 is input         */
  UBYTE   DigiPinsIn;          /* Contains the status of the digital pins                      */
  UBYTE   DigiPinsOut;         /* Sets the output level of the digital pins                    */
  UBYTE   CustomPctFullScale;  /* Sets the Pct full scale of the custom sensor                 */
  UBYTE   CustomActiveStatus;  /* Sets the active or inactive state of the custom sensor       */

  UBYTE   InvalidData;         /* Indicates wether data is invalid (1) or valid (0)            */

  UBYTE   Spare1;
  UBYTE   Spare2;
  UBYTE   Spare3;

}INPUTSTRUCT;

typedef   struct
{
  CALDATA ColorCal;
  UWORD   ADRaw[NO_OF_COLORS];
  UWORD   SensorRaw[NO_OF_COLORS];
  SWORD   SensorValue[NO_OF_COLORS];
  UBYTE   Boolean[NO_OF_COLORS];
  UBYTE   CalibrationState;
  UBYTE   Free1;
  UBYTE   Free2;
  UBYTE   Free3;
}COLORSTRUCT;

typedef   struct
{
  INPUTSTRUCT   Inputs[NO_OF_INPUTS];
  COLORSTRUCT   Colors[NO_OF_INPUTS];
  UBYTE   (*pFunc)(UBYTE, UBYTE, UBYTE, UBYTE *);
}IOMAPINPUT;

#endif



