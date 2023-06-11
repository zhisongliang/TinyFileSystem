#ifndef LIBTINYFS_H
#define LIBTINYFS_H

// disk block may be any of these types
/* superblock
 * must contain magic number,
 * poitner to root directory,
 * and free block list implementation
 * number possible: 1
 * size: 256 bytes
 */

/* inode
 * the file size and a data block indexing implementation
 * number possible: many
 * size: 256 bytes
 */

/* data
 * contains actual data of a file
 * number possible: many
 * size: 256 bytes
 */

/* free
 * An unallocated block
 * number possible: many
 * size: 256 bytes
 */

/* The default size of the disk and file system block */
#define BLOCKSIZE 256
/* Your program should use a 10240 byte disk size giving you 40 blocks total. This is the default size. You must be able to support different possible values, or report an error if it exceeds the limits of your implementation. */
#define DEFUALT_DISK_SIZE 10240
/* use this name for a default disk file name */
#define DEFUALT_DISK_NAME "tinyFSDisk"
typedef int fileDescriptor;

/* Makes an empty TinyFS file system of size nBytes on the file specified by ‘filename’. This function should use the emulated disk library to open the specified file, and upon success, format the file to be mountable. This includes initializing all data to 0x00, setting magic numbers, initializing and writing the superblock and other metadata, etc. Must return a specified success/error code. */
int tfs_mkfs(char *filename, int nBytes);

/* tfs_mount(char *filename) “mounts” a TinyFS file system located within ‘filename’. tfs_unmount(void) “unmounts” the currently mounted file system. As part of the mount operation, tfs_mount should verify the file system is the correct type. Only one file system may be mounted at a time. Use tfs_unmount to cleanly unmount the currently mounted file system. Must return a specified success/error code. */
int tfs_mount(char *filename);
int tfs_unmount(void);

/* Opens a file for reading and writing on the currently mounted file system. Creates a dynamic resource table entry for the file (the structure that tracks open files, the internal file pointer, etc.), and returns a file descriptor (integer) that can be used to reference this file while the filesystem is mounted. */
fileDescriptor tfs_openFile(char *name);

/* Closes the file and removes dynamic resource table entry */
int tfs_close(fileDescriptor FD);

/* Writes buffer ‘buffer’ of size ‘size’, which represents an entire file’s contents, to the file described by ‘FD’. Sets the file pointer to 0 (the start of file) when done. Returns success/error codes. */
int tfs_write(fileDescriptor FD, char *buffer, int size);

/* deletes a file and marks its blocks as free on disk. */
int tfs_delete(fileDescriptor FD);

/* reads one byte from the file and copies it to ‘buffer’, using the current file pointer location and incrementing it by one upon success. If the file pointer is already at the end of the file then tfs_readByte() should return an error and not increment the file pointer. */
int tfs_readByte(fileDescriptor FD, char *buffer);

/* change the file pointer location to offset (absolute). Returns success/error codes.*/
int tfs_seek(fileDescriptor FD, int offset);

/* renames a file.  New name should be passed in. */
int tfs_rename(fileDescriptor FD, char *newName);

/* lists all the files and directories on the disk */
void tfs_readdir();

#endif