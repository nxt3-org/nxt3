#include "stdconst.h"
#include "modules.h"
#include "d_display.h"
#include "d_timer.h"
#include "d_display_ev3.h"
#include "c_display.iom.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define EV3_LCD_WIDTH   178
#define EV3_LCD_HEIGHT  128

#define EV3_STRIDE ((EV3_LCD_WIDTH + 2) / 3)
#define EV3_BUFFER (EV3_STRIDE * EV3_LCD_HEIGHT)

#define NXT_LCD_WIDTH   100
#define NXT_LCD_HEIGHT  64

#define NXT_SKIP_INITIAL_UPLOADS 2
#define NXT_SINGLE_UPLOAD_ROWS   8
#define NXT_UPLOAD_COUNT         8

#define EV3_X0 ((EV3_LCD_WIDTH  - NXT_LCD_WIDTH)  / 2)
#define EV3_Y0 ((EV3_LCD_HEIGHT - NXT_LCD_HEIGHT) / 2)
#define NXT_X0 0
#define NXT_Y0 0

#define NXT_CROP_X0 ((NXT_LCD_WIDTH * 2 - EV3_LCD_WIDTH) / 4)

static void  ev3DisplayInit(void *pHeaders);
static void  ev3DisplayPower(UBYTE On, UBYTE Contrast);
static UBYTE ev3DisplayUpdate(UWORD Height,UWORD Width,UBYTE *pImage,UBYTE Scaling);
static void  ev3DisplayExit(void);


static void  ev3DisplayUpdateCentered(int block, UBYTE *nxtBuffer);
static void  ev3DisplayUpdateStretched(int block, UBYTE *nxtBuffer);
static void  ev3DisplayUpdateCropped(int block, UBYTE *nxtBuffer);

typedef struct {
  int   fd;
  UBYTE *memory;
  UBYTE enabled;
  UBYTE counter;
  UBYTE oldScaling;
} EV3_DISPLAY_DATA;



DISPLAY_OUTPUT DisplayEV3 = {
  .Name     = "EV3 Framebuffer",
  .Init     = ev3DisplayInit,
  .Exit     = ev3DisplayExit,
  .SetPower = ev3DisplayPower,
  .Update   = ev3DisplayUpdate
};

DISPLAY_OUTPUT *pDisplay = &DisplayEV3;

static EV3_DISPLAY_DATA this = {
  .fd      = -1,
  .memory  = NULL,
  .enabled = FALSE,
  .counter = 0
};

void ev3DisplayInit(void *pHeaders) {
  if (this.fd < 0) {
    // open framebuffer kernel file
    this.fd = open("/dev/fb0", O_RDWR);
    if (this.fd < 0) {
      perror("EV3 FB: cannot open /dev/fb0");
      return;
    }

    this.memory = mmap(NULL, EV3_BUFFER, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED, this.fd, 0);
    if (this.memory == MAP_FAILED) {
      this.memory = NULL;
      perror("EV3 FB: cannot mmap /dev/fb0");

      close(this.fd);
      this.fd = -1;
      return;
    }

    this.enabled = FALSE;
    this.counter = 0;
  }
}

void ev3DisplayPower(UBYTE On, UBYTE Contrast) {
  if (!this.memory) return;

  UBYTE was = this.enabled;
  UBYTE is  = On;

  this.enabled = On;

  // clear everything
  if (was != is) {
    memset(this.memory, 0, EV3_BUFFER);
  }
}

UBYTE ev3DisplayUpdate(UWORD Height,UWORD Width,UBYTE *pImage,UBYTE Scaling) {

  if (!this.memory)       { return 0; }
  if (!this.enabled)      { return 0; }
  if (Width  != NXT_LCD_WIDTH)  { return 0; }
  if (Height != NXT_LCD_HEIGHT) { return 0; }

  if (Scaling != this.oldScaling) {
    memset(this.memory, 0, EV3_BUFFER);
    this.oldScaling = Scaling;
  }



  if (this.counter >= NXT_SKIP_INITIAL_UPLOADS) {
    int blocksDone = this.counter - NXT_SKIP_INITIAL_UPLOADS;

    time_point now = dTimerGetNow();
    switch (Scaling) {
      case SCALING_OFF:     ev3DisplayUpdateCentered(blocksDone, pImage);  break;
      case SCALING_STRETCH: ev3DisplayUpdateStretched(blocksDone, pImage); break;
      case SCALING_CROP:    ev3DisplayUpdateCropped(blocksDone, pImage);   break;
    }
    time_delta dt = dTimerNowGetDelta(&now);

    printf("DT: %9ld us\n", dt.tv_nsec / 1000);
  }

  this.counter++;

  if (this.counter == (NXT_UPLOAD_COUNT + NXT_SKIP_INITIAL_UPLOADS))
    this.counter = 0;

  return this.counter;
}

void ev3DisplayUpdateCentered(int blocksDone, UBYTE *nxtBuffer) {
  UBYTE *nxtBlock = &nxtBuffer[NXT_LCD_WIDTH * blocksDone];

  UBYTE *ev3Column = &this.memory[(EV3_Y0 + NXT_SINGLE_UPLOAD_ROWS * blocksDone) * EV3_STRIDE + EV3_X0 / 3];
  UBYTE ev3Mask = 0b00000011 << (3 * (2 - EV3_X0 % 3));

  for (int dx = 0; dx < NXT_LCD_WIDTH; dx++) {

    UBYTE rows = nxtBlock[dx];
    UBYTE *ev3Pix = ev3Column;
    for (int i = 0; i < 8; i++) {
      UBYTE on = rows & 0x01;

      if (on) {
        *ev3Pix |=  ev3Mask;
      } else {
        *ev3Pix &= ~ev3Mask;
      }

      ev3Pix += EV3_STRIDE;
      rows >>= 1;
    }


    if (ev3Mask == 0b11000000) {
      ev3Mask = 0b00011000;
    } else if (ev3Mask == 0b00011000) {
      ev3Mask = 0b00000011;
    } else if (ev3Mask == 0b00000011) {
      ev3Mask = 0b11000000;
      ev3Column += 1;
    }
  }
}

void ev3DisplayUpdateStretched(int blocksDone, UBYTE *nxtBuffer) {
  UBYTE *nxtBlock = &nxtBuffer[NXT_LCD_WIDTH * blocksDone];

  UBYTE *ev3Column = &this.memory[NXT_SINGLE_UPLOAD_ROWS * blocksDone * EV3_STRIDE * 2];
  UBYTE ev3Mask = 0b11000000;

  for (int dstX = 0; dstX < EV3_LCD_WIDTH; dstX++) {

    UBYTE rows = nxtBlock[dstX * NXT_LCD_WIDTH / EV3_LCD_WIDTH];
    UBYTE *ev3Pix = ev3Column;
    for (int i = 0; i < 8; i++) {
      UBYTE on = rows & 0x01;

      for (int j = 0; j < 2; j++) {
        if (on) {
          *ev3Pix |=  ev3Mask;
        } else {
          *ev3Pix &= ~ev3Mask;
        }

        ev3Pix += EV3_STRIDE;
      }
      rows >>= 1;
    }


    if (ev3Mask == 0b11000000) {
      ev3Mask = 0b00011000;
    } else if (ev3Mask == 0b00011000) {
      ev3Mask = 0b00000011;
    } else if (ev3Mask == 0b00000011) {
      ev3Mask = 0b11000000;
      ev3Column += 1;
    }
  }
}

void ev3DisplayUpdateCropped(int blocksDone, UBYTE *nxtBuffer) {
  UBYTE *nxtBlock = &nxtBuffer[NXT_LCD_WIDTH * blocksDone];

  UBYTE *ev3Column = &this.memory[NXT_SINGLE_UPLOAD_ROWS * blocksDone * EV3_STRIDE * 2];
  UBYTE ev3Mask = 0b11000000;

  for (int dstX = 0; dstX < EV3_LCD_WIDTH; dstX++) {

    UBYTE rows = nxtBlock[dstX / 2 + NXT_CROP_X0];
    UBYTE *ev3Pix = ev3Column;
    for (int i = 0; i < 8; i++) {
      UBYTE on = rows & 0x01;

      for (int j = 0; j < 2; j++) {
        if (on) {
          *ev3Pix |=  ev3Mask;
        } else {
          *ev3Pix &= ~ev3Mask;
        }

        ev3Pix += EV3_STRIDE;
      }
      rows >>= 1;
    }


    if (ev3Mask == 0b11000000) {
      ev3Mask = 0b00011000;
    } else if (ev3Mask == 0b00011000) {
      ev3Mask = 0b00000011;
    } else if (ev3Mask == 0b00000011) {
      ev3Mask = 0b11000000;
      ev3Column += 1;
    }
  }
}

void ev3DisplayExit(void) {
  if (this.memory) {
    if (munmap(this.memory, EV3_BUFFER) < 0) {
      perror("EV3 FB: cannot unmap /dev/fb0");
    }
    this.memory = NULL;
  }
  if (this.fd >= 0) {
    if (close(this.fd) < 0) {
      perror("EV3 FB: cannot close /dev/fb0");
    }
    this.fd = -1;
  }
}
