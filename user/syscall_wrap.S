#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>


LEAF(msyscall)
    // TODO: execute a `syscall` instruction and return from msyscall
    
    sw a0, 0(sp)
    sw a1, 4(sp)
    sw a2, 8(sp)
    sw a3, 12(sp)
    move v0, a0
    
    syscall

    jr ra
    nop    

END(msyscall)
