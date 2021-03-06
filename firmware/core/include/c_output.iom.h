//
// Date init       14.12.2004
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: c_output.iom                                  $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/c_outp $
//
// Platform        C
//

#ifndef   COUTPUT_IOM
#define   COUTPUT_IOM

#define   NO_OF_OUTPUTS     3
#define   pMapOutPut        ((IOMAPOUTPUT*)(pHeaders[ENTRY_OUTPUT]->pIOMap))

// Constants reffering to mode
enum
{
  MOTORON    = 0x01,
  BRAKE      = 0x02,
  REGULATED  = 0x04,
  REG_METHOD = 0xF0   /* Regulation methods - to be designed! */
};

#define OPTION_HOLDATLIMIT     0x10
#define OPTION_RAMPDOWNTOLIMIT 0x20

// Constants related to Flags
enum
{
  UPDATE_MODE		          = 0x01,
  UPDATE_SPEED		          = 0x02,
  UPDATE_TACHO_LIMIT          = 0x04,
  UPDATE_RESET_COUNT          = 0x08,
  UPDATE_PID_VALUES	          = 0x10,
  UPDATE_RESET_BLOCK_COUNT    = 0x20,
  UPDATE_RESET_ROTATION_COUNT = 0x40,
  PENDING_UPDATES             = 0x80  
};

// Constant related to RunState
#define MOTOR_RUN_STATE_IDLE      0x00
#define MOTOR_RUN_STATE_RAMPUP    0x10
#define MOTOR_RUN_STATE_RUNNING   0x20
#define MOTOR_RUN_STATE_RAMPDOWN  0x40

// Constant related to RegMode
enum
{
  REGULATION_MODE_IDLE = 0,
  REGULATION_MODE_MOTOR_SPEED = 1,
  REGULATION_MODE_MOTOR_SYNC = 2,
  REGULATION_MODE_MOTOR_POS = 4,
};
											
typedef   struct
{
  SLONG   TachoCnt;						/* R  - Holds current number of counts, since last reset, updated every 1 mS */
  SLONG   BlockTachoCount;				/* R  - Holds current number of counts for the current output block */
  SLONG   RotationCount;			    /* R  - Holds current number of counts for the rotation counter to the output */
  ULONG   TachoLimit;					/* RW - Holds number of counts to travel, 0 => Run forever */
  SWORD   MotorRPM;						/* !! Is not updated, will be removed later !! */
  UBYTE   Flags;						/* RW - Holds flags for which data should be updated */
  UBYTE   Mode;							/* RW - Holds motor mode: Run, Break, regulated, ... */
  SBYTE   Speed;						/* RW - Holds the wanted speed */
  SBYTE   ActualSpeed;					/* R  - Holds the current motor speed */
  UBYTE   RegPParameter;				/* RW - Holds the P-constant use din the regulation, Is set to a default value at init => Setting this value is optional for the user */
  UBYTE   RegIParameter;				/* RW - Holds the I-constant use din the regulation, Is set to a default value at init => Setting this value is optional for the user */
  UBYTE   RegDParameter;				/* RW - Holds the D-constant use din the regulation, Is set to a default value at init => Setting this value is optional for the user */
  UBYTE	  RunState;					    /* RW - Holds the current RunState in the output module */
  UBYTE   RegMode;						/* RW - Tells which regulation mode should be used */
  UBYTE   Overloaded;					/* R  - True if the motor has been overloaded within speed control regulation */ 
  SBYTE   SyncTurnParameter;			/* RW - Holds the turning parameter need within MoveBlock */   
  UBYTE   Options;
  SBYTE   MaxSpeed;						/* RW - Maximum speed for absolute regulation, or 0 for no limit */
  SBYTE   MaxAcceleration;				/* RW - Maximum acceleration for absolute regulation, or 0 for no limit */
}OUTPUT;


typedef   struct
{
  OUTPUT  Outputs[NO_OF_OUTPUTS];
  UBYTE   RegulationTime;               /* RW - Interval between regulation computations */
  UBYTE   RegulationOptions;            /* RW - Options for regulation, see REGOPTION_* */
}IOMAPOUTPUT;



#endif



