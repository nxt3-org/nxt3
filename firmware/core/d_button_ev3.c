#include "stdconst.h"
#include "m_sched.h"
#include "d_button.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


// enum definitions

typedef enum {
  EV3_UP,
  EV3_ENTER,
  EV3_DOWN,
  EV3_RIGHT,
  EV3_LEFT,
  EV3_BACK,
} ev3_button;

typedef enum {
  NXT_ESCAPE,
  NXT_RIGHT,
  NXT_LEFT,
  NXT_ENTER
} nxt_button;

typedef enum __attribute__ ((__packed__)) {
  EV3_LED_OFF = '0',
  EV3_LED_ON_GREEN,
  EV3_LED_ON_RED,
  EV3_LED_ON_ORANGE,
  EV3_LED_BLINK_GREEN,
  EV3_LED_BLINK_RED,
  EV3_LED_BLINK_ORANGE,
  EV3_LED_BEAT_GREEN,
  EV3_LED_BEAT_RED,
  EV3_LED_BEAT_ORANGE,
} ev3_led_mode;


// button mapping

static const ev3_button nxt_to_ev3[4] = {
  [NXT_ESCAPE] = EV3_BACK,
  [NXT_RIGHT]  = EV3_RIGHT,
  [NXT_LEFT]   = EV3_LEFT,
  [NXT_ENTER]  = EV3_ENTER,
};


// kernel device definitions

typedef struct {
  const char version[8];
} kernel_read;

typedef struct {
  ev3_led_mode mode;
  UBYTE padding;
} kernel_write;

typedef struct {
  UBYTE buttons[6];
} kernel_memory;


// module data

typedef struct {
  int fd;
  volatile kernel_memory *kmem;
} BUTTON_DATA;


static BUTTON_DATA this;


static void ev3ButtonInit(void *pHeaders, UBYTE Prescaler);
  static UBYTE ev3ButtonLowerInit(void);
  static void ev3ButtonSetGreen(void);
static void ev3ButtonExit(void);
static void ev3ButtonRead(UBYTE *pOutMask);
  static UBYTE ev3ButtonSample(void);

BUTTON_INPUT ButtonsEV3 = {
  .Name = "EV3 Buttons",
  .Init = ev3ButtonInit,
  .Exit = ev3ButtonExit,
  .Read = ev3ButtonRead
};

BUTTON_INPUT *pButton = &ButtonsEV3;

void ev3ButtonInit(void *pHeaders, UBYTE Prescaler) {
  if (!ev3ButtonLowerInit())
    return;
  ev3ButtonSetGreen();
}

UBYTE ev3ButtonLowerInit(void) {
  this.fd = open("/dev/lms_ui", O_RDWR);
  if (this.fd < 0) {
    perror("EV3 BUTTONS: cannot open /dev/lms_ui");
    return FALSE;
  }

  this.kmem = mmap(NULL, sizeof(kernel_memory), PROT_READ, MAP_SHARED | MAP_LOCKED, this.fd, 0);
  if (this.kmem == MAP_FAILED) {
    this.kmem = NULL;
    perror("EV3 BUTTONS: cannot mmap /dev/lms_ui");

    close(this.fd);
    this.fd = -1;
    return FALSE;
  }

  return TRUE;
}

void ev3ButtonSetGreen(void) {
  kernel_write leds = { .mode = EV3_LED_ON_GREEN };

  if (write(this.fd, &leds, sizeof(leds)) < 0) {
    perror("EV3 BUTTONS: cannot set leds to green");
  }
}

void ev3ButtonRead(UBYTE *pOutMask)
{
  *pOutMask = ev3ButtonSample();
}

UBYTE ev3ButtonSample(void) {
  UBYTE result = 0;
  kernel_memory copy = *this.kmem;

  for (int btn = 0; btn < NOS_OF_AVR_BTNS; btn++) {
    ev3_button index = nxt_to_ev3[btn];

    if (copy.buttons[index]) {
      result |= 1 << btn;
    }
  }

  return result;
}


void ev3ButtonExit(void) {
  if (this.kmem) {
    if (munmap((void*) this.kmem, sizeof(kernel_memory)) < 0) {
      perror("EV3 BUTTONS: cannot unmap /dev/lms_ui");
    }
    this.kmem = NULL;
  }

  if (this.fd >= 0) {
    kernel_write leds = { .mode = EV3_LED_OFF };

    if (write(this.fd, &leds, sizeof(leds)) < 0) {
      perror("EV3 BUTTONS: cannot turn off leds");
    }

    if (close(this.fd) < 0) {
      perror("EV3 BUTTONS: cannot close /dev/lms_ui");
    }
    this.fd = -1;
  }
}
