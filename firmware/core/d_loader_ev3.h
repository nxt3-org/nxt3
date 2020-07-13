#ifndef EV3_FILESYSTEM_H
#define EV3_FILESYSTEM_H

#include "stdconst.h"
#include "modules.h"
#include <sys/types.h>
#include <dirent.h>

#ifndef FILENAME_SIZE
#define FILENAME_SIZE (FILENAME_LENGTH + 1)
#endif

typedef enum {
  OM_READ,
  OM_WRITE,
  OM_APPEND
} open_mode;

typedef enum {
  SK_FROM_START,
  SK_FROM_CURRENT,
  SK_FROM_END
} seek_mode;

typedef struct {
  char  name[FILENAME_SIZE];
  int   linuxFD;
  UBYTE *memCopy;
  ULONG fullLength;
  ULONG readPointer;
  ULONG writePointer;
} EV3_FS_CB;

#define EV3_FS_CB_INIT ((EV3_FS_CB) { .name = {'\0'}, .linuxFD = -1, .memCopy = NULL, .fullLength = 0, .readPointer = 0, .writePointer = 0 })

#define RETURN_OK   0
#define RETURN_ERR -1
#define RETURN_EOF  1

extern SBYTE ev3FsInit(const char *dataPath, const char *metaPath);
extern SBYTE ev3FsDeinit(void);

extern SBYTE ev3FsLoadMeta(EV3_FS_CB *this, const char *name);
extern SBYTE ev3FsSaveMeta(EV3_FS_CB *this);

extern SBYTE ev3FsStartBrowse(DIR **stream);

extern SBYTE ev3FsCreateWrite(EV3_FS_CB *this, const char *name, ULONG fileSize);
extern SBYTE ev3FsOpenRead(EV3_FS_CB *this, const char *name);
extern SBYTE ev3FsOpenAppend(EV3_FS_CB *this, const char *name);

extern SBYTE ev3FsMmap(EV3_FS_CB *this, UBYTE **memory);
extern SBYTE ev3FsRead(EV3_FS_CB *this, UBYTE *buffer, ULONG *pLength);
extern SBYTE ev3FsWrite(EV3_FS_CB *this, const UBYTE *buffer, ULONG *pLength);
extern SBYTE ev3FsSeekRead(EV3_FS_CB *this, SLONG offset, seek_mode mode);
extern SBYTE ev3FsTellRead(EV3_FS_CB *this, ULONG *filePosition);

extern SBYTE ev3FsRemove(EV3_FS_CB *this);
extern SBYTE ev3FsMove(EV3_FS_CB *this, const char *name);
extern SBYTE ev3FsShrink(EV3_FS_CB *this);

extern SBYTE ev3FsClose(EV3_FS_CB *this);

extern SBYTE ev3FsGetFreeBytes(ULONG *pAmount);

#endif
