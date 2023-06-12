// libTinyFS interface file. This file will access libDisk for disk emulator functionality.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libTinyFS.h"
#include "libDisk.h"
#include "ERRORNO.h"

// disk mounted or unmounted status
#define DISK_MOUNTED 1
#define DISK_UNMOUNTED -1

fileDescriptor cur_disk;
fileDescriptor mountDisk = DISK_MOUNTED;

/* Makes an empty TinyFS file system of size nBytes on the file specified by ‘filename’. This function should use the emulated disk library to open the specified file, and upon success, format the file to be mountable. This includes initializing all data to 0x00, setting magic numbers, initializing and writing the superblock and other metadata, etc. Must return a specified success/error code. */
int tfs_mkfs(char *filename, int nBytes)
{
    int fd = openDisk(filename, nBytes);

    // check if file is valid
    if (fd < 0)
    {
        return FILEO_ERR;
    }

    // initialize superblock
    uint8_t superblock[BLOCKSIZE];
    int i;
    for (i = 0; i < BLOCKSIZE; i++)
    {
        superblock[i] = 0x00;
    }

    // set magic number
    superblock[0] = MAGICNUMBER;

    // initialize and write superblock and other metadata
    if (writeBlock(fd, 0, superblock) < 0)
    {
        return FILEW_ERR;
    }

    // write other blocks
    int num_blocks = nBytes / BLOCKSIZE;
    int i;

    /* Todo: inode, data, free block */
    for (i = 1; i < num_blocks; i++)
    {
        uint8_t block[BLOCKSIZE];

        // write block
        if (writeBlock(fd, i, block) < 0)
        {
            return FILEW_ERR;
        }
    }

    return SUCCESS;
}

/* tfs_mount(char *filename) “mounts” a TinyFS file system located within ‘filename’. tfs_unmount(void) “unmounts” the currently mounted file system. As part of the mount operation, tfs_mount should verify the file system is the correct type. Only one file system may be mounted at a time. Use tfs_unmount to cleanly unmount the currently mounted file system. Must return a specified success/error code. */
int tfs_mount(char *filename)
{
    // check if there is already a disk mounted
    if (mountDisk != DISK_UNMOUNTED)
    {
        return ALREADY_MOUNTED_ERR;
    }

    // open disk
    cur_disk = openDisk(filename, 0);
    if (cur_disk < 0)
    {
        return FILEO_ERR;
    }

    // read superblock
    uint8_t block[BLOCKSIZE];
    int flag = readBlock(cur_disk, 0, block);
    if (flag < 0)
    {
        return FILER_ERR;
    }

    // check magic number
    if (block[0] != MAGICNUMBER)
    {
        return MAGICNUM_INVALID_ERR;
    }

    mountDisk = DISK_MOUNTED;
    return SUCCESS;
}

int tfs_unmount(void)
{
    if (mountDisk == DISK_UNMOUNTED)
    {
        return NO_MOUNTED_ERR;
    }
    closeDisk(cur_disk);
    // set mountDisk to -1
    mountDisk = DISK_UNMOUNTED;
    return SUCCESS;
}

/* Opens a file for reading and writing on the currently mounted file system. Creates a dynamic resource table entry for the file (the structure that tracks open files, the internal file pointer, etc.), and returns a file descriptor (integer) that can be used to reference this file while the filesystem is mounted. */
fileDescriptor tfs_openFile(char *name)
{
    return 0;
}

/* Closes the file and removes dynamic resource table entry */
int tfs_close(fileDescriptor FD)
{
    return 0;
}

/* Writes buffer ‘buffer’ of size ‘size’, which represents an entire file’s contents, to the file described by ‘FD’. Sets the file pointer to 0 (the start of file) when done. Returns success/error codes. */
int tfs_write(fileDescriptor FD, char *buffer, int size)
{
    return 0;
}

/* deletes a file and marks its blocks as free on disk. */
int tfs_delete(fileDescriptor FD)
{
    return 0;
}

/* reads one byte from the file and copies it to ‘buffer’, using the current file pointer location and incrementing it by one upon success. If the file pointer is already at the end of the file then tfs_readByte() should return an error and not increment the file pointer. */
int tfs_readByte(fileDescriptor FD, char *buffer)
{
    return 0;
}

/* change the file pointer location to offset (absolute). Returns success/error codes.*/
int tfs_seek(fileDescriptor FD, int offset)
{
    return 0;
}

int tfs_rename(fileDescriptor FD, char *newName)
{
    return 0;
}