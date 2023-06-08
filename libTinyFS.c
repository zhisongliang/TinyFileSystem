// libTinyFS interface file. This file will access libDisk for disk emulator functionality.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libTinyFS.h"
#include "libDisk.h"
#include "ERRORNO.h"

int tfs_mkfs(char *filename, int nBytes);
int tfs_mount(char *filename);
int tfs_unmount(void);
fileDescriptor tfs_openFile(char *name);
int tfs_close(fileDescriptor FD);
int tfs_write(fileDescriptor FD, char *buffer, int size);
int tfs_delete(fileDescriptor FD);
int tfs_readByte(fileDescriptor FD, char *buffer);
int tfs_seek(fileDescriptor FD, int offset);