#include "lib.h"

void
umain(void)
{
    char i = 49, last = 49;
    int round = 0;
    while(1)
    {
        //syscall_write_dev(&i, 0x10000000, 1);
        while(i == last){
            round++;
            syscall_read_dev(&i, 0x10000000, 1);
        }
        last = i;
        writef("Input: %d\n", i);
        round = 0;
    }
}