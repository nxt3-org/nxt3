#include "d_loader_ev3.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

static const char *DataRootPath  = ".";
static int         DataRootFD    = -1;
static const char *MetaRootPath  = ".";
static int         MetaRootFD    = -1;

typedef struct {
  ULONG Version;
  ULONG DataSize;
} FILE_META;

#define FILE_META_VERSION_1 0x00000001
#define FILE_META_VERSION   FILE_META_VERSION_1

static SBYTE ev3FstatAt(int parentFD, const char *path, struct stat *block);
static SBYTE ev3InitMeta(EV3_FS_CB *this, struct stat *fileStat);

SBYTE ev3FsInit(const char *dataPath, const char *metaPath) {
  DataRootPath = dataPath;
  MetaRootPath = metaPath;

  // do not care about these errors
  mkdir(DataRootPath, 00755);
  mkdir(MetaRootPath, 00755);

  DataRootFD = open(DataRootPath, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
  if (DataRootFD < 0) {
    int _errno = errno;
    perror("EV3 FS: cannot open root data directory");
    errno = _errno;
    return -1;
  }

  MetaRootFD = open(MetaRootPath, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
  if (DataRootFD < 0) {
    int _errno = errno;
    perror("EV3 FS: cannot open root meta directory");
    close(DataRootFD);
    errno = _errno;
    return -1;
  }

  return 0;
}

SBYTE ev3FsDeinit(void) {
  if (MetaRootFD >= 0) {
    if (close(MetaRootFD) < 0) {
      perror("EV3 FS: cannot close root meta directory");
    }
    MetaRootFD = -1;
  }

  if (DataRootFD >= 0) {
    if (close(DataRootFD) < 0) {
      perror("EV3 FS: cannot close root data directory");
    }
    DataRootFD = -1;
  }

  return 0;
}

SBYTE ev3FsStartBrowse(DIR **pStream) {
  *pStream = NULL;

  DIR *stream = opendir(DataRootPath);
  if (stream == NULL) {
    int _errno = errno;
    perror("EV3 FS: cannot (re)open root directory");
    errno = _errno;
    return -1;
  }

  *pStream = stream;
  return 0;
}

SBYTE ev3FsLoadMeta(EV3_FS_CB *this, const char *name) {
  *this = (EV3_FS_CB) {
    .name         = {0},
    .linuxFD      = -1,
    .memCopy      = NULL,
    .fullLength   = 0,
    .readPointer  = 0,
    .writePointer = 0
  };
  strncpy(this->name, name, FILENAME_LENGTH);

  struct stat fsInfo = {0};
  if (ev3FstatAt(DataRootFD, this->name, &fsInfo) < 0) {
    return -1;
  }

  if (ev3InitMeta(this, &fsInfo) < 0) {
    return -1;
  }

  return 0;
}

SBYTE ev3FstatAt(int parentFD, const char *path, struct stat *block) {
  int err = fstatat(parentFD, path, block, 0);
  if (err < 0) {
    int _errno = errno;
    perror("EV3 FS: cannot stat data file");
    errno = _errno;
    return -1;
  }

  return 0;
}

SBYTE ev3InitMeta(EV3_FS_CB *this, struct stat *fileStat) {
  int fd = openat(MetaRootFD, this->name, O_CREAT | O_RDWR, 00644);
  if (fd < 0) {
    int _errno = errno;
    perror("EV3 FS: cannot open/create meta file");
    errno = _errno;
    return -1;
  }

  struct stat metaStat = {0};
  int err = fstat(fd, &metaStat);
  if (err < 0) {
    int _errno = errno;
    perror("EV3 FS: cannot stat meta file");
    errno = _errno;
    return -1;
  }

  UBYTE metaIsOlder = metaStat.st_mtim.tv_sec  >= fileStat->st_mtim.tv_sec &&
                      metaStat.st_mtim.tv_nsec >  fileStat->st_mtim.tv_nsec;

  FILE_META info = {
    .Version  = FILE_META_VERSION,
    .DataSize = fileStat->st_size
  };

  if (metaIsOlder) {
    FILE_META loaded = {0};
    int bytes = pread(fd, &loaded, sizeof(loaded), 0);

    if (bytes < 0) {
      int _errno = errno;
      perror("EV3 FS: cannot read meta block");
      close(fd);
      errno = _errno;
      return -1;
    }

    if (bytes >= 4) {
      if (loaded.Version == FILE_META_VERSION_1 && bytes >= 8) {
        if (info.DataSize > loaded.DataSize) {
          info.DataSize = loaded.DataSize;
        }
      }
    }
  }

  err = pwrite(fd, &info, sizeof(info), 0);
  if (err < 0) {
    int _errno = errno;
    perror("EV3 FS: cannot write meta block");
    close(fd);
    errno = _errno;
    return -1;
  }

  err = close(fd);
  if (err < 0) {
    int _errno = errno;
    perror("EV3 FS: cannot close meta file");
    errno = _errno;
    return -1;
  }

  this->fullLength   = fileStat->st_size;
  this->readPointer  = 0;
  this->writePointer = info.DataSize;
  return 0;
}

SBYTE ev3FsSaveMeta(EV3_FS_CB *this) {
  int fd = openat(MetaRootFD, this->name, O_CREAT | O_WRONLY, 00644);
  if (fd < 0) {
    int _errno = errno;
    perror("EV3 FS: cannot open/create meta file");
    errno = _errno;
    return -1;
  }

  FILE_META info = {
    .Version  = FILE_META_VERSION,
    .DataSize = this->writePointer
  };

  int err = pwrite(fd, &info, sizeof(info), 0);
  if (err < 0) {
    int _errno = errno;
    perror("EV3 FS: cannot write meta block");
    close(fd);
    errno = _errno;
    return -1;
  }

  err = close(fd);
  if (err < 0) {
    int _errno = errno;
    perror("EV3 FS: cannot close meta file");
    errno = _errno;
    return -1;
  }

  return 0;
}

SBYTE ev3FsCreateWrite(EV3_FS_CB *this, const char *name, ULONG fileSize) {
  *this = (EV3_FS_CB) {
    .name         = {0},
    .linuxFD      = -1,
    .memCopy      = NULL,
    .fullLength   = fileSize,
    .readPointer  = 0,
    .writePointer = 0
  };
  strncpy(this->name, name, FILENAME_LENGTH);


  this->linuxFD = openat(DataRootFD, this->name, O_CREAT | O_EXCL | O_WRONLY | O_CLOEXEC, 00644);
  if (this->linuxFD < 0) {
    int _errno = errno;
    perror("EV3 FS: cannot create file for writing");
    errno = _errno;
    return -1;
  }

  int err = ftruncate(this->linuxFD, fileSize);
  if (err < 0) {
    int _errno = errno;
    perror("EV3 FS: cannot set file size");
    close(this->linuxFD);
    this->linuxFD = -1;
    errno = _errno;
    return -1;
  }

  if (ev3FsSaveMeta(this) < 0) {
    close(this->linuxFD);
    this->linuxFD = -1;
    return -1;
  }

  return 0;
}

SBYTE ev3FsOpenRead(EV3_FS_CB *this, const char *name) {
  if (name != NULL) {
    if (ev3FsLoadMeta(this, name) < 0) {
      return -1;
    }
  }

  this->linuxFD = openat(DataRootFD, this->name, O_RDONLY | O_CLOEXEC);
  if (this->linuxFD < 0) {
    int _errno = errno;
    perror("EV3 FS: cannot create file for reading");
    errno = _errno;
    return -1;
  }

  return 0;
}

SBYTE ev3FsOpenAppend(EV3_FS_CB *this, const char *name) {
  if (name != NULL) {
    if (ev3FsLoadMeta(this, name) < 0) {
      return -1;
    }
  }

  this->linuxFD = openat(DataRootFD, this->name, O_WRONLY | O_CLOEXEC);
  if (this->linuxFD < 0) {
    int _errno = errno;
    perror("EV3 FS: cannot create file for appending");
    errno = _errno;
    return -1;
  }

  return 0;
}

SBYTE ev3FsMmap(EV3_FS_CB *this, UBYTE **pMemory) {
  if (this->linuxFD < 0) {
    errno = EBADF;
    perror("EV3 FS: passed in bad file to memmap");
    errno = EBADF;
    return -1;
  }

  if (this->memCopy) {
    *pMemory = this->memCopy;
    return 0;
  } else {
    *pMemory = NULL;
  }

  this->memCopy = malloc(this->fullLength);
  if (!this->memCopy) {
    int _errno = errno;
    perror("EV3 FS: cannot allocate memory for memmap");
    errno = _errno;
    return -1;
  }

  ULONG read = 0;
  while (1) {
    SLONG now = pread(this->linuxFD, this->memCopy, this->fullLength - read, 0);

    if (now < 0) {
      if (errno == EINTR) {
        continue;
      } else {
        int _errno = errno;
        perror("EV3 FS: cannot read file to memory");
        free(this->memCopy);
        this->memCopy = NULL;
        errno = _errno;
        return -1;
      }

    } else if (now == 0) {
      break;

    } else {
      read += now;
      if (read == this->fullLength)
        break;
    }
  }

  if (read != this->fullLength) {
    errno = EFBIG;
    perror("EV3 FS: memmap read invalid number of bytes");
    free(this->memCopy);
    this->memCopy = NULL;
    errno = EFBIG;
    return -1;

  } else {
    *pMemory = this->memCopy;
    return 0;
  }
}

SBYTE ev3FsRead(EV3_FS_CB *this, UBYTE *buffer, ULONG *pLength) {
  if (this->linuxFD < 0) {
    errno = EBADF;
    perror("EV3 FS: passed in bad file to read");
    errno = EBADF;
    return RETURN_ERR;
  }

  ULONG available = this->writePointer - this->readPointer;
  ULONG request   = *pLength;
  ULONG fulfill   = request < available ? request : available;

  *pLength = 0;
  memset(buffer, 0, request);

  if (fulfill == 0) {
    if (request == 0) {
      return RETURN_OK;
    } else {
      return RETURN_EOF;
    }
  }

  if (this->memCopy) {
    memcpy(buffer, this->memCopy + this->readPointer, fulfill);

  } else {
    ULONG done = 0;
    while (1) {
      SLONG now = pread(this->linuxFD, buffer + done, fulfill - done, this->readPointer + done);

      if (now < 0) {
        if (errno == EINTR) {
          continue;
        } else {
          int _errno = errno;
          perror("EV3 FS: read failed");
          errno = _errno;
          return RETURN_ERR;
        }

      } else if (now == 0) {
        break;

      } else {
        done += now;
        if (done == fulfill)
          break;
      }
    }

    if (done != fulfill) {
      errno = EIO;
      perror("EV3 FS: cannot read full file");
      errno = EIO;
      return RETURN_ERR;
    }
  }

  this->readPointer += fulfill;
  *pLength = fulfill;

  if (this->readPointer == this->writePointer) {
    return RETURN_EOF;
  } else {
    return RETURN_OK;
  }
}

SBYTE ev3FsWrite(EV3_FS_CB *this, const UBYTE *buffer, ULONG *pLength) {
  if (this->linuxFD < 0) {
    errno = EBADF;
    perror("EV3 FS: passed in bad file to write");
    errno = EBADF;
    return RETURN_ERR;
  }

  ULONG available = this->fullLength - this->writePointer;
  ULONG request   = *pLength;
  ULONG fulfill   = request < available ? request : available;

  *pLength = 0;

  if (fulfill == 0) {
    if (request == 0) {
      return RETURN_OK;
    } else {
      return RETURN_EOF;
    }
  }

  ULONG done = 0;
  while (1) {
    SLONG now = pwrite(this->linuxFD, buffer + done, fulfill - done, this->writePointer + done);

    if (now < 0) {
      if (errno == EINTR) {
        continue;
      } else {
        int _errno = errno;
        perror("EV3 FS: write failed");
        errno = _errno;
        return RETURN_ERR;
      }

    } else if (now == 0) {
      break;

    } else {
      done += now;
      if (done == fulfill)
        break;
    }
  }

  if (done != fulfill) {
    errno = EIO;
    perror("EV3 FS: cannot write full file");
    errno = EIO;
    return RETURN_ERR;
  }

  this->writePointer += fulfill;
  *pLength = fulfill;

  if (this->writePointer == this->fullLength) {
    return RETURN_EOF;
  } else {
    return RETURN_OK;
  }
}

SBYTE ev3FsSeekRead(EV3_FS_CB *this, SLONG offset, seek_mode mode) {
  ULONG newPointer = 0;

  if (mode == SK_FROM_START) {
    newPointer = offset;

  } else if (mode == SK_FROM_CURRENT) {
    newPointer = this->readPointer + offset;

  } else if (mode == SK_FROM_END) {
    newPointer = this->writePointer + offset;

  } else {
    errno = EINVAL;
    perror("EV3 FS: invalid seek mode passed in");
    errno = EINVAL;
    return -1;
  }

  if (newPointer > this->writePointer) {
    errno = EINVAL;
    perror("EV3 FS: invalid seek target");
    errno = EINVAL;
    return -1;
  }

  return 0;
}

SBYTE ev3FsTellRead(EV3_FS_CB *this, ULONG *filePosition) {
  *filePosition = this->readPointer;
  return 0;
}

SBYTE ev3FsRemove(EV3_FS_CB *this) {
  int err = 0;
  int retval = 0;
  int _errno = 0;

  if (this->linuxFD >= 0) {
    err = ev3FsClose(this);
    if (err < 0) {
      _errno = errno;
      perror("EV3 FS: cannot close file for removal");
      retval = -1;
    }
  }

  err = unlinkat(DataRootFD, this->name, 0);
  if (err < 0) {
    _errno = errno;
    perror("EV3 FS: cannot remove data file");
    retval = -1;
  }

  err = unlinkat(MetaRootFD, this->name, 0);
  if (err < 0) {
    _errno = errno;
    perror("EV3 FS: cannot remove meta file");
    retval = -1;
  }

  errno = _errno;
  return retval;
}

SBYTE ev3FsMove(EV3_FS_CB *this, const char *name) {
  int err = 0;
  int _errno = 0;

  err = linkat(DataRootFD, this->name, DataRootFD, name, 0);
  if (err < 0) {
    _errno = errno;
    perror("EV3 FS: cannot make hardlink to data file");
    errno = _errno;
    return -1;
  }

  err = linkat(MetaRootFD, this->name, MetaRootFD, name, 0);
  if (err < 0) {
    _errno = errno;
    perror("EV3 FS: cannot make hardlink to meta file");
    errno = _errno;
    return -1;
  }

  err = unlinkat(DataRootFD, this->name, 0);
  if (err < 0) {
    perror("EV3 FS: warning: cannot remove data file after moving");
  }

  err = unlinkat(MetaRootFD, this->name, 0);
  if (err < 0) {
    perror("EV3 FS: warning: cannot remove meta file after moving");
  }

  strncpy(this->name, name, FILENAME_LENGTH);
  return 0;
}

SBYTE ev3FsShrink(EV3_FS_CB *this) {
  int _errno = 0;
  int retval = 0;
  this->fullLength = this->writePointer;

  int err = ftruncate(this->linuxFD, this->fullLength);
  if (err < 0) {
    _errno = errno;
    perror("EV3 FS: cannot set file size");
    retval = -1;
  }

  if (ev3FsClose(this) < 0) {
    _errno = errno;
    perror("EV3 FS: cannot close file after shrinking");
    retval = -1;
  }

  errno = _errno;
  return retval;
}

SBYTE ev3FsClose(EV3_FS_CB *this) {
  int err = 0;
  int retval = 0;
  int _errno = 0;
  UBYTE wasOpen = FALSE;

  if (this->linuxFD >= 0) {
    wasOpen = TRUE;
    err = close(this->linuxFD);
    if (err < 0) {
      _errno = errno;
      perror("EV3 FS: cannot close data file descriptor");
      retval = -1;
    }
  }
  this->linuxFD = -1;

  if (this->memCopy) {
    free(this->memCopy);
  }
  this->memCopy = NULL;

  if (retval == 0 && wasOpen) {
    if (ev3FsSaveMeta(this) < 0) {
      _errno = errno;
      perror("EV3 FS: cannot write file metadata");
      retval = -1;
    }
  }

  errno = _errno;
  return retval;
}

SBYTE ev3FsGetFreeBytes(ULONG *pAmount) {
  *pAmount = 0;

  struct statvfs info;
  int err = fstatvfs(DataRootFD, &info);

  if (err < 0) {
    int _errno = errno;
    perror("EV3 FS: cannot do statvfs");
    errno = _errno;
    return -1;
  }

  *pAmount = info.f_bsize * info.f_bavail; // even though we are root
  return 0;
}
