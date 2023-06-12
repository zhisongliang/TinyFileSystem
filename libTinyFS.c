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

char *cur_fs = "";
fileDescriptor cur_disk;
fileDescriptor mountDisk = DISK_MOUNTED;

File resourceTable[MAX_FILE_NUM];

// ******* Helper functions prototypes ********
int fd_seek(fileDescriptor FD);
void initSuperBlock(Block_FS *superblock, unsigned char first_free, int nBytes);
void initRootInode(Block_FS *root_inode);
void initInode(Block_FS *inode, int file_size, unsigned char data_block_idx);
void serialize_Name_Inode_Pair(const Name_Inode_Pair *pair, uint8_t *buffer);
void deserialize_Name_Inode_Pair(const uint8_t *buffer, Name_Inode_Pair *pair);

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
    Block_FS superblock;
    initSuperBlock(&superblock, 0, nBytes);

    // initialize and write superblock and other metadata
    if (writeBlock(fd, 0, superblock.block) < 0)
    {
        return FILEW_ERR;
    }

    int num_blocks = nBytes / BLOCKSIZE;
    int i;

    // initialize root directory inode
    Block_FS root_inode;
    initRootInode(&root_inode);

    // write free blocks to the rest of the disk
    for (i = 2; i < num_blocks; i++)
    {
        Block_FS free_block[BLOCKSIZE];
        free_block->block[0] = FREE_BLOCK_CODE;

        for (i = 1; i < BLOCKSIZE; i++)
        {
            root_inode.block[i] = 0x00;
        }

        // write block
        if (writeBlock(fd, i, free_block) < 0)
        {
            return FILEW_ERR;
        }
    }

    return SUCCESS;
}

void initSuperBlock(Block_FS *superblock, unsigned char first_free, int nBytes)
{
    int i;
    for (i = 0; i < BLOCKSIZE; i++)
    {
        superblock->block[i] = 0x00;
    }

    // set superblock code
    superblock->block[0] = SUPERBLOCK_CODE;
    // set magic number
    superblock->block[1] = MAGICNUMBER;
    // set pointer to root inode
    superblock->block[2] = ROOT_INODE_IDX;
    // set pointer to free block table
    superblock->block[3] = 2;
}

void initRootInode(Block_FS *root_inode)
{
    int i;
    for (i = 0; i < BLOCKSIZE; i++)
    {
        root_inode->block[i] = 0x00;
    }

    // root inode cannot handle that much files
    if (MAX_FILE_NUM * sizeof(Name_Inode_Pair) > BLOCKSIZE)
    {
        return REACH_FILE_LIMIT_ERR;
    }

    root_inode->block[0] = INODE_CODE;
    Name_Inode_Pair name_inode_pairs[MAX_FILE_NUM];
    for (i = 0; i < MAX_FILE_NUM; i++)
    {
        name_inode_pairs[i].inode_idx = -1;
        strcpy(name_inode_pairs[i].filename, "");
    }

    // serialize name-inode pairs into root inode
    // skip first byte
    for (i = 1; i < BLOCKSIZE; i += sizeof(Name_Inode_Pair))
    {
        serialize_Name_Inode_Pair(&name_inode_pairs[i / sizeof(Name_Inode_Pair)], root_inode->block + i);
    }
}

void initInode(Block_FS *inode, int file_size, unsigned char data_block_idx)
{
    int i;
    for (i = 0; i < BLOCKSIZE; i++)
    {
        inode->block[i] = 0x00;
    }
}

void serialize_Name_Inode_Pair(const Name_Inode_Pair *pair, uint8_t *buffer)
{
    memcpy(buffer, pair, sizeof(Name_Inode_Pair));
}

void deserialize_Name_Inode_Pair(const uint8_t *buffer, Name_Inode_Pair *pair)
{
    memcpy(pair, buffer, sizeof(Name_Inode_Pair));
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

/* return index of fd in resourceTable
if not found, return -1 */
int fd_seek(fileDescriptor FD)
{
    int i;
    for (i = 0; i < MAX_FILE_NUM; i++)
    {
        if (resourceTable[i].fd = FD)
            return i;
    }
    // not found
    return -1;
}

/* change the file pointer location to offset (absolute). Returns success/error codes.*/
int tfs_seek(fileDescriptor FD, int offset)
{
    int i = fd_seek(FD);
    if (i < 0)
    {
        return FILE_NOT_FOUND_ERR;
    }
    resourceTable[i].file_ptr = offset;
    return SUCCESS;
}

/* Closes the file and removes dynamic resource table entry */
int tfs_close(fileDescriptor FD)
{
    int i = fd_seek(FD);
    if (i < 0)
    {
        return FILE_NOT_FOUND_ERR;
    }
    resourceTable[i].fd = -1;
    return SUCCESS;
}

/* Opens a file for reading and writing on the currently mounted file system. Creates a dynamic resource table entry for the file (the structure that tracks open files, the internal file pointer, etc.), and returns a file descriptor (integer) that can be used to reference this file while the filesystem is mounted. */
fileDescriptor tfs_openFile(char *name)
{
    // check if there is a disk mounted
    if (mountDisk == DISK_UNMOUNTED)
    {
        return NO_MOUNTED_ERR;
    }

    // check if the file is already opened

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

int tfs_rename(fileDescriptor FD, char *newName)
{
    return 0;
}