#include "d_output_ev3.h"
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>


typedef struct {
  SLONG moveDegrees;
  SBYTE speed; // this value is unusable
  SLONG absoluteDegrees;
} one_motor;

typedef struct {
  one_motor motors[4];
} tacho_kmem;

typedef struct {
  UBYTE pad_start[5164];
  UBYTE OutputType[4];
  UBYTE OutputConnection[4];
  UBYTE pad_end[2];
} analog_kmem;


#define CMD_PROGRAM_START 0x03
typedef struct __attribute__((__packed__)) {
  UBYTE cmd;
} pwm_req_program_start;

#define CMD_PROGRAM_STOP 0x02
typedef struct __attribute__((__packed__)) {
  UBYTE cmd;
} pwm_req_program_stop;

#define CMD_SET_MOTOR_TYPE 0xA1
#define MOTOR_CONN_TACHO  125
#define MOTOR_CONN_NONE   125
#define MOTOR_TYPE_NONE   126
#define MOTOR_TYPE_NXT    7
#define MOTOR_TYPE_LARGE  9
#define MOTOR_TYPE_MEDIUM 8
typedef struct __attribute__((__packed__)) {
  UBYTE cmd;
  UBYTE types[4];
} pwm_req_set_motor_type;

#define CMD_OUTPUT_START 0xA6
typedef struct __attribute__((__packed__)) {
  UBYTE cmd;
  UBYTE mask;
} pwm_req_output_start;

#define CMD_OUTPUT_STOP 0xA3
typedef struct __attribute__((__packed__)) {
  UBYTE cmd;
  UBYTE mask;
  UBYTE doBrake;
} pwm_req_output_stop;

#define CMD_SET_DUTY_CYCLE 0xA4
typedef struct __attribute__((__packed__)) {
  UBYTE cmd;
  UBYTE mask;
  UBYTE power;
} pwm_req_set_duty_cycle;

#define CMD_RESET_TACHO 0xB2
typedef struct __attribute__((__packed__)) {
  UBYTE cmd;
  UBYTE mask;
} pwm_req_reset_tacho;


#define CMD_SET_POLARITY 0xA7
#define POLARITY_FORWARD (+1)
#define POLARITY_REVERSE (-1)
#define POLARITY_TOGGLE  (+1)
typedef struct __attribute__((__packed__)) {
  UBYTE cmd;
  UBYTE mask;
  SBYTE multiplier;
} pwm_req_set_polarity;

typedef struct {
  int pwmFD;
  int tachoFD;
  int analogFD;
  volatile tacho_kmem *kernelTacho;
  volatile analog_kmem *kernelAnalog;
  SLONG lastTacho[4];
  SBYTE dutyCycle[4];
  ev3_motor_type types[4];
  ev3_stop_mode  stopModes[4];
  UBYTE isRunning[4];
} EV3_OUTPUT_DATA;


static SBYTE outputStart(ev3_motor_port which);
static SBYTE outputStop(ev3_motor_port which);
static SBYTE sendPwmRequest(void *buffer, size_t size);

static EV3_OUTPUT_DATA this = {
  .pwmFD        = -1,
  .tachoFD      = -1,
  .analogFD     = -1,
  .kernelTacho  = NULL,
  .kernelAnalog = NULL,
  .lastTacho    = { 0,              0,              0,              0              },
  .dutyCycle    = { 0,              0,              0,              0              },
  .types        = { ev3_motor_none, ev3_motor_none, ev3_motor_none, ev3_motor_none },
  .stopModes    = { ev3_stop_coast, ev3_stop_coast, ev3_stop_coast, ev3_stop_coast },
  .isRunning    = { 0,              0,              0,              0              }
};


SBYTE sendPwmRequest(void *buffer, size_t size) {
  if (this.pwmFD < 0)
    return -1;

  int bytes = write(this.pwmFD, buffer, size);

  if (bytes < 0) {
    perror("EV3 OUTPUT: cannot send pwm request to kernel");
    return -1;
  } else {
    return 0;
  }
}

SBYTE ev3OutputInit(void) {
  this.pwmFD    = open("/dev/lms_pwm", O_RDWR);
  if (this.pwmFD < 0) {
    perror("EV3 OUTPUT: cannot open /dev/lms_pwm");
    return -1;
  }

  this.tachoFD  = open("/dev/lms_motor", O_RDWR);
  if (this.tachoFD < 0) {
    perror("EV3 OUTPUT: cannot open /dev/lms_motor");
    return -1;
  }

  this.analogFD = open("/dev/lms_analog", O_RDWR);
  if (this.analogFD < 0) {
    perror("EV3 OUTPUT: cannot open /dev/lms_analog");
    return -1;
  }

  this.kernelTacho = mmap(NULL, sizeof(tacho_kmem), PROT_READ, MAP_SHARED | MAP_LOCKED, this.tachoFD, 0);
  if (this.kernelTacho == MAP_FAILED) {
    this.kernelTacho = NULL;
    perror("EV3 OUTPUT: cannot mmap /dev/lms_motor");
    return -1;
  }

  this.kernelAnalog = mmap(NULL, sizeof(analog_kmem), PROT_READ, MAP_SHARED | MAP_LOCKED, this.analogFD, 0);
  if (this.kernelAnalog == MAP_FAILED) {
    this.kernelAnalog = NULL;
    perror("EV3 OUTPUT: cannot mmap /dev/lms_analog");
    return -1;
  }

  pwm_req_program_start startReq = {
    .cmd = CMD_PROGRAM_START
  };

  if (sendPwmRequest(&startReq, sizeof(startReq) < 0))
    return -1;

  pwm_req_set_polarity polarityReq = {
    .cmd        = CMD_SET_POLARITY,
    .mask       = 0x0F,
    .multiplier = POLARITY_FORWARD
  };

  if (sendPwmRequest(&polarityReq, sizeof(polarityReq)) < 0)
    return -1;

  return ev3OutputCtrl();
}

SBYTE ev3OutputExit(void) {
  pwm_req_program_stop exitReq = {
    .cmd = CMD_PROGRAM_STOP
  };

  if (sendPwmRequest(&exitReq, sizeof(exitReq)) < 0)
    return -1;

  if (this.kernelAnalog && munmap((void*) this.kernelAnalog, sizeof(analog_kmem)) < 0)
    perror("EV3 OUTPUT: cannot munmap /dev/lms_analog");
  if (this.kernelTacho  && munmap((void*) this.kernelTacho, sizeof(tacho_kmem)) < 0)
    perror("EV3 OUTPUT: cannot munmap /dev/lms_motor");
  if (this.analogFD >= 0 && close(this.analogFD) < 0)
    perror("EV3 OUTPUT: cannot close /dev/lms_analog");
  if (this.pwmFD    >= 0 && close(this.pwmFD)    < 0)
    perror("EV3 OUTPUT: cannot close /dev/lms_pwm");
  if (this.tachoFD  >= 0 && close(this.tachoFD)  < 0)
    perror("EV3 OUTPUT: cannot close /dev/lms_motor");

  this.kernelAnalog = NULL;
  this.kernelTacho  = NULL;
  this.analogFD = -1;
  this.tachoFD  = -1;
  this.pwmFD    = -1;

  return 0;
}

SBYTE ev3OutputCtrl(void) {
  UBYTE uploadType[4] = { MOTOR_TYPE_NONE, MOTOR_TYPE_NONE, MOTOR_TYPE_NONE, MOTOR_TYPE_NONE };
  UBYTE uploadFlag = 0;

  for (int motor = 0; motor < 4; motor++) {
    UBYTE newConn = this.kernelAnalog->OutputConnection[motor];
    UBYTE newType = this.kernelAnalog->OutputType[motor];
    ev3_motor_type newCombo = ev3_motor_none;

    if (newConn == MOTOR_CONN_TACHO) {
      if (newType == MOTOR_TYPE_NXT) {
        newCombo = ev3_motor_nxt;

      } else if (newType == MOTOR_TYPE_MEDIUM) {
        newCombo = ev3_motor_medium;

      } else if (newType == MOTOR_TYPE_LARGE) {
        newCombo = ev3_motor_large;
      }
    }

    if (newCombo != ev3_motor_none) {
      uploadType[motor] = newType;
    }

    if (newCombo != this.types[motor]) {
      uploadFlag |= 1 << motor;
      this.types[motor] = newCombo;
    }
  }

  if (!uploadFlag)
    return 0;

  pwm_req_set_motor_type req = {
    .cmd   = CMD_SET_MOTOR_TYPE,
    .types = { uploadType[0], uploadType[1], uploadType[2], uploadType[3] }
  };

  if (sendPwmRequest(&req, sizeof(req)) < 0)
    return -1;

  for (int motor = 0; motor < 4; motor++) {
    if ((uploadFlag & (1 << motor)) == 0)
      continue;

    this.stopModes[motor] = ev3_stop_coast;
    this.isRunning[motor] = FALSE;
    SLONG tmp = 0;
    ev3OutputCalcTachoDelta(motor, &tmp);
    ev3OutputSetPwmDuty(motor, this.dutyCycle[motor]);
  }

  return 0;
}

SBYTE outputStart(ev3_motor_port which) {
  if (which >= 4)
    return -1;

  pwm_req_output_start req = {
    .cmd     = CMD_OUTPUT_START,
    .mask    = 1 << which,
  };

  return sendPwmRequest(&req, sizeof(req));
}

SBYTE outputStop(ev3_motor_port which) {
  if (which >= 4)
    return -1;

  this.dutyCycle[which] = 0;

  pwm_req_output_stop req = {
    .cmd     = CMD_OUTPUT_STOP,
    .mask    = 1 << which,
    .doBrake = this.stopModes[which] == ev3_stop_brake
  };

  return sendPwmRequest(&req, sizeof(req));
}

SBYTE ev3OutputSetStopMode(ev3_motor_port which, ev3_stop_mode mode) {
  if (which >= 4)
    return -1;

  this.stopModes[which] = mode;
  return 0;
}

SBYTE ev3OutputSetPwmDuty(ev3_motor_port which, SBYTE percent) {
  if (which >= 4)
    return -1;

  this.dutyCycle[which] = percent;

  if (percent == 0) {
    if (this.isRunning[which]) {
      if (outputStop(which) < 0) {
        return -1;
      }
      this.isRunning[which] = FALSE;
    }
  } else {
    if (!this.isRunning[which]) {
      if (outputStart(which) < 0) {
        return -1;
      }
      this.isRunning[which] = TRUE;
    }
  }

  pwm_req_set_duty_cycle req = {
    .cmd   = CMD_SET_DUTY_CYCLE,
    .mask  = 1 << which,
    .power = percent
  };

  return sendPwmRequest(&req, sizeof(req));
}

SBYTE ev3OutputGetTacho(ev3_motor_port which, SLONG *pDegrees) {
  if (which >= 4)
    return -1;

  *pDegrees = this.kernelTacho->motors[which].absoluteDegrees;
  return 0;
}

SBYTE ev3OutputCalcTachoDelta(ev3_motor_port which, SLONG *pDegrees) {
  SLONG now = 0;
  if (ev3OutputGetTacho(which, &now) < 0)
    return -1;

  *pDegrees = now - this.lastTacho[which];
  this.lastTacho[which] = now;
  return 0;
}

SBYTE ev3OutputResetTacho(ev3_motor_port which) {
  if (which >= 4)
    return -1;

  this.lastTacho[which] = 0;

  pwm_req_reset_tacho req = {
    .cmd   = CMD_RESET_TACHO,
    .mask  = 1 << which,
  };

  return sendPwmRequest(&req, sizeof(req));
}

SBYTE ev3OutputDetectType(ev3_motor_port which, ev3_motor_type *pPluggedIn) {
  if (which >= 4)
    return -1;

  *pPluggedIn = this.types[which];
  return 0;
}
