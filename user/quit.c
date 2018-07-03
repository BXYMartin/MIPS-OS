#include "lib.h"

int flag[256];

void
umain(int argc, char **argv)
{
    int args = 0;
    writef(RED(Goodbye My Shell!\n));
    syscall_write_dev(&args, 0x10000010, 4);
}