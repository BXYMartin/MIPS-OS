/*
 * operations on IDE disk.
 */

#include "fs.h"
#include "lib.h"
#include <mmu.h>


// Overview:
//      read data from IDE disk. First issue a read request through
//      disk register and then copy data from disk buffer
//      (512 bytes, a sector) to destination array.
//
// Parameters:
//      diskno: disk number.
//      secno: start sector number.
//      dst: destination for data read from IDE disk.
//      nsecs: the number of sectors to read.
//
// Post-Condition:
//      If error occurred during read the IDE disk, panic. 
// 
// Hint: use syscalls to access device registers and buffers
void
ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs)
{
        // 0x200: the size of a sector: 512 bytes.
        int offset_begin = secno * 0x200;
        int offset_end = offset_begin + nsecs * 0x200;
        int offset = 0;
    int r = 0;
    int offset_disk = 0;
    const int read_disk = 0;
    syscall_write_dev(&diskno, D_ADDR+D_ID, 4);
        while (offset_begin + offset < offset_end) {
        // Your code here
        offset_disk = offset_begin + offset;
        syscall_write_dev(&offset_disk, D_ADDR+D_OFF, 4);
        syscall_write_dev(&read_disk, D_ADDR+D_RW, 4);
        syscall_read_dev(&r, D_ADDR+D_S, 1);
        if(r == 0)
            user_panic("Disk I/O Error");
        syscall_read_dev(dst+offset, D_ADDR+D_BIN, 512);
        offset+=0x200;
        /*
        if(read_sector(diskno, offset_begin + offset))
        {
            syscall_read_dev(dst + offset, 0x13004000, 0x200);
            //user_bcopy(0x93004000, dst + offset, 0x200);
            offset += 0x200;
        }
        else
        {
            user_panic("Disk I/O Error");
        }
        */
        // error occurred, then panic.
        }
}


// Overview:
//      write data to IDE disk.
//
// Parameters:
//      diskno: disk number.
//      secno: start sector number.
//      src: the source data to write into IDE disk.
//      nsecs: the number of sectors to write.
//
// Post-Condition:
//      If error occurred during read the IDE disk, panic.
//
// Hint: use syscalls to access device registers and buffers
void
ide_write(u_int diskno, u_int secno, void *src, u_int nsecs)
{
        // Your code here
        int offset_begin = secno * 0x200;
        int offset_end = offset_begin + nsecs * 0x200;
        int offset = 0;
    int offset_disk = 0;
    int r;
    const int write_call = 1;
        //writef("diskno: %d\n", diskno);
    syscall_write_dev(&diskno, D_ADDR+D_ID, 4);
        while (offset_begin + offset < offset_end) {
            // copy data from source array to disk buffer.
        //writef("@@@@@@@@@ Write IDE: offset_begin: %x\toffset: %x\tsrc: %x\n", offset_begin, offset, src);
        offset_disk = offset_begin + offset;
        syscall_write_dev(&offset_disk, D_ADDR+D_OFF, 4);
        syscall_write_dev(src+offset, D_ADDR+D_BIN, 512);
        syscall_write_dev(&write_call, D_ADDR+D_RW, 4);
        syscall_read_dev(&r, D_ADDR+D_S, 1);
        if(r == 0)
            user_panic("Disk I/O Error");
        offset+=0x200;
        //user_bcopy(src + offset, 0x93004000, 0x200);
        /*
        syscall_write_dev(src + offset, 0x13004000, 0x200);
        if(write_sector(diskno, offset_begin + offset))
        {
            offset += 0x200;
        }
        else
        {
            user_panic("Disk I/O Error");
        }
        */
        // if error occur, then panic.
        }
} 