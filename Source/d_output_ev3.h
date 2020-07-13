#ifndef EV3_OUTPUT_H
#define EV3_OUTPUT_H

#include "stdconst.h"
#include "modules.h"

typedef enum {
  ev3_motor_a,
  ev3_motor_b,
  ev3_motor_c,
  ev3_motor_d
} ev3_motor_port;

typedef enum {
  ev3_stop_brake,
  ev3_stop_coast
} ev3_stop_mode;

typedef enum {
  ev3_motor_none,
  ev3_motor_nxt,
  ev3_motor_large,
  ev3_motor_medium
} ev3_motor_type;

extern SBYTE ev3OutputInit(void);
extern SBYTE ev3OutputExit(void);
extern SBYTE ev3OutputCtrl(void);

extern SBYTE ev3OutputSetStopMode(ev3_motor_port which, ev3_stop_mode mode);
extern SBYTE ev3OutputSetPwmDuty(ev3_motor_port which, SBYTE percent);
extern SBYTE ev3OutputGetTacho(ev3_motor_port which, SLONG *pDegrees);
extern SBYTE ev3OutputCalcTachoDelta(ev3_motor_port which, SLONG *pDegrees);
extern SBYTE ev3OutputResetTacho(ev3_motor_port which);

extern SBYTE ev3OutputDetectType(ev3_motor_port which, ev3_motor_type *pPluggedIn);

#endif
