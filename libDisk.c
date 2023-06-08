#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "ERRORNO.h"
#include "libDisk.h"

/*
libDisk interface functions
The emulator is a library of four functions that operate on a regular UNIX file.
The necessary functions are: openDisk(), readBlock(), writeBlock(), and closeDisk().
*/

#define BLOCKSIZE 256

/*
All I/O done to the emulated disk must be block aligned to BLOCKSIZE,
meaning that the disk assumes the buffers passed in readBlock() and writeBlock()
are exactly BLOCKSIZE bytes large. If they are not, the behavior is undefined.
*/

int openDisk(char *filename, int nBytes)
/*
    # This function opens a regular UNIX file and designates the first nBytes of it as space for the emulated disk.
    # nBytes should be a number that is evenly divisible by the block size.
    # If nBytes > 0 and there is already a file by the given filename, that disk is resized to nBytes, and that file’s contents may be overwritten.
    # If nBytes is 0, an existing disk is opened, and should not be overwritten. There is no requirement to maintain integrity of any content beyond nBytes.
    # Errors must be returned for any other failures, as defined by your own error code system.

    # args
        # (char *) filename, (int) nBytes
    # returns (int)
        # On access: fd
        # Error: FILE_ERR
*/
{
    char *space;
    int fd;
    if (nBytes == 0)
    {
        return open(filename, O_RDWR);
    }
    else
    {
        if ((fd = open(filename, O_CREAT | O_RDWR, 0777)) >= 0)
        {
            if (nBytes % BLOCKSIZE != 0)
            {
                nBytes = (nBytes / BLOCKSIZE + 1) * BLOCKSIZE;
            }
            space = (char *)malloc(nBytes * sizeof(char));
            int i;
            for (i = 0; i < nBytes; i++)
            {
                space[i] = '\0';
            }
            if (write(fd, space, nBytes) < nBytes)
            {
                free(space);
                return FILEO_ERR;
            }
            free(space);
        }
    }
    return fd;
}

int readBlock(int disk, int bNum, void *block)
/*
# reads an entire block of BLOCKSIZE bytes from the open disk (identified by ‘disk’)
    # and copies the result into a local buffer (must be at least of BLOCKSIZE bytes).
    # The bNum is a logical block number, which must be translated into a byte offset within the disk.
    # The translation from logical to physical block is straightforward:
    # bNum=0 is the very first byte of the file.
    # bNum=1 is BLOCKSIZE bytes into the disk
    # bNum=n is n*BLOCKSIZE bytes into the disk.
    #
    # args
        # (int) disk #, (int) bNum, block
    # returns (int)
        # On access: 0
        # Error: FILER_ERR
*/
{
    int byteOffset = bNum * BLOCKSIZE;
    if (lseek(disk, byteOffset, SEEK_SET) == -1)
        return FILER_ERR;
    if (read(disk, block, BLOCKSIZE) < -1)
        return FILER_ERR;
    return 0;
}

int writeBlock(int disk, int bNum, void *block)
/*
    # writes the content of the buffer ‘block’ to that location.
    # BLOCKSIZE bytes will be written from ‘block’ regardless of its actual size.
    # The disk must be open, and must translate the logical block bNum to the correct byte position in the file.
    # args
        # (int) disk #, (int) bNum, block
    # returns (int)
        # On access: 0
        # Error: FILEW_ERR
*/
{
    int offset = bNum * BLOCKSIZE;
    if (lseek(disk, offset, SEEK_SET) == -1)
        return FILEW_ERR;
    if ((write(disk, block, BLOCKSIZE)) <= 0)
        return FILEW_ERR;
    return 0;
}

void closeDisk(int disk)
/*
    # makes the disk closed to further I/O
    # args
        # (int) disk #
    # returns (int)
        # On access: 0
        # Error: FILWC_ERR
*/
{
    close(disk);
}
