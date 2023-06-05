#!/usr/bin/env python3
import os

# libDisk interface functions
# The emulator is a library of four functions that operate on a regular UNIX file.
# The necessary functions are: openDisk(), readBlock(), writeBlock(), and closeDisk(). 

BLOCKSIZE = 256

### 
# All I/O done to the emulated disk must be block aligned to BLOCKSIZE, 
# meaning that the disk assumes the buffers passed in readBlock() and writeBlock()
# are exactly BLOCKSIZE bytes large. If they are not, the behavior is undefined. 
###

Block_Device = []
Disk = 0

class Disk:
    
    def __init__(self, filename, filestream):
        self.filename = filename
        self.filestream = filestream
        self.open = 1
        

class libDisk:

    def __init__(self):
        self.BLOCKSIZE = 256
        self.Block_Device = []
        self.DiskNumber = 0

    ###
    # This function opens a regular UNIX file and designates the first nBytes of it as space for the emulated disk. 
    # nBytes should be a number that is evenly divisible by the block size. 
    # If nBytes > 0 and there is already a file by the given filename, that disk is resized to nBytes, and that file’s contents may be overwritten. 
    # If nBytes is 0, an existing disk is opened, and should not be overwritten. There is no requirement to maintain integrity of any content beyond nBytes. 
    # Errors must be returned for any other failures, as defined by your own error code system.

    # args
        # (string) filename, (int) nBytes
    # returns (int)
        # On access: disk #
        # Error: -1
    ###
    def openDisk(self, filename, nBytes):
        if nBytes > 0:
            if os.path.exists(filename):
                # file already exist, allow overwrite
                fd = open(filename, "w+")
                for i in range(len(self.Block_Device)):
                    if self.Block_Device[i].filename == filename:
                        self.Block_Device[i].open = 1
                        self.Block_Device[i].filestream = fd
                        return i
            else:
                # create new disk
                fd = open(filename, "w+")
                newDisk = Disk(filename, fd)
                self.Block_Device.append(newDisk)
                self.DiskNumber += 1
                return self.DiskNumber
        else:
            if os.path.exists(filename):
                # file already exist but nBytes == 0, read only
                fd = open(filename, "r+")
                for i in range(len(self.Block_Device)):
                    if self.Block_Device[i].filename == filename:
                        self.Block_Device[i].open = 1
                        self.Block_Device[i].filestream = fd
                        return i
            else:
                return -1
    
    ###
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
        # Error: -2
    ###
    
    def readBlock(self, disk, bNum, block):
        if disk >= self.DiskNumber:
            return -2
        fd = self.Block_Device[disk].filestream
        fd.seek(bNum*self.BLOCKSIZE)
        block.contents = fd.read(self.BLOCKSIZE)
        return 0
    
    ###
    # writes the content of the buffer ‘block’ to that location. 
    # BLOCKSIZE bytes will be written from ‘block’ regardless of its actual size.
    # The disk must be open, and must translate the logical block bNum to the correct byte position in the file.
    # args
        # (int) disk #, (int) bNum, block
    # returns (int)
        # On access: 0
        # Error: -2
    ###

    def writeBlock(self, disk, bNum, block):
        if disk >= self.DiskNumber:
            return -2
        curr = self.Block_Device[disk].filestream
        curr.seek(bNum*self.BLOCKSIZE)
        w = block.contents[:self.BLOCKSIZE]
        curr.write(w)
        return 0
        
    ###
    # makes the disk closed to further I/O
    # args
        # (int) disk #
    # returns (int)
        # On access: 0
        # Error: -2
    ###
    
    def closeDisk(self, disk):
        if disk >= self.DiskNumber:
            return -2
        self.Block_Device[disk].filestream.close()
        self.Block_Device[disk].open = 0
        return 0