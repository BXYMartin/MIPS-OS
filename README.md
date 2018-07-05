# MIPS-OS
An Custom Operating System with a Shell for MIPS R3000

## Customed readELF Version
This project includes a customed readelf source code in order to load icode into memory properly.

## Memory Management
 o     4G ----------->  +----------------------------+------------0x100000000

 o                      |       ...                  |  kseg3

 o                      +----------------------------+------------0xe000 0000

 o                      |       ...                  |  kseg2

 o                      +----------------------------+------------0xc000 0000

 o                      |   Interrupts & Exception   |  kseg1

 o                      +----------------------------+------------0xa000 0000

 o                      |      Invalid memory        |   /|\

 o                      +----------------------------+----|-------Physics Memory Max

 o                      |       ...                  |  kseg0

 o  VPT,KSTACKTOP-----> +----------------------------+----|-------0x8040 0000-------end

 o                      |       Kernel Stack         |    | KSTKSIZE            /|\

 o                      +----------------------------+----|------                |

 o                      |       Kernel Text          |    |                    PDMAP

 o      KERNBASE -----> +----------------------------+----|-------0x8001 0000    |

 o                      |   Interrupts & Exception   |   \|/                    \|/

 o      ULIM     -----> +----------------------------+------------0x8000 0000-------

 o                      |         User VPT           |     PDMAP                /|\

 o      UVPT     -----> +----------------------------+------------0x7fc0 0000    |

 o                      |         PAGES              |     PDMAP                 |

 o      UPAGES   -----> +----------------------------+------------0x7f80 0000    |

 o                      |         ENVS               |     PDMAP                 |

 o  UTOP,UENVS   -----> +----------------------------+------------0x7f40 0000    |

 o  UXSTACKTOP -/       |     user exception stack   |     BY2PG                 |

 o                      +----------------------------+------------0x7f3f f000    |

 o                      |       Invalid memory       |     BY2PG                 |

 o      USTACKTOP ----> +----------------------------+------------0x7f3f e000    |

 o                      |     normal user stack      |     BY2PG                 |

 o                      +----------------------------+------------0x7f3f d000    |

 a                      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                           |

 a                      .                            .                         kuseg

 a                      |~~~~~~~~~~~~~~~~~~~~~~~~~~~~|                           |

 a                      |                            |                           |

 o       UTEXT   -----> +----------------------------+                           |

 o                      |                            |     2 * PDMAP            \|/

 a     - ------------>  +----------------------------+ -----------------------------
 
 Within `mm/pmap.c`, a self-mapping page table is implemented. Size per page is set to 4KB for the convenience of self-mapping. The map above is the key to the whole architecture.

## Process Management
### Processes
Each process is described in `envs`, a struct to store the necessary information.
``` c++
struct Env {
        struct Trapframe env_tf;        // Saved registers
        LIST_ENTRY(Env) env_link;       // Free list
        u_int env_id;                   // Unique environment identifier
        u_int env_parent_id;            // env_id of this env's parent
        u_int env_status;               // Status of the environment
        Pde  *env_pgdir;                // Kernel virtual address of page dir
        u_int env_cr3;
        LIST_ENTRY(Env) env_sched_link;
        u_int env_pri;
        u_int env_ipc_value;            // data value sent to us 
        u_int env_ipc_from;             // envid of the sender  
        u_int env_ipc_recving;          // env is blocked receiving
        u_int env_ipc_dstva;            // va at which to map received page
        u_int env_ipc_perm;             // perm of page mapping received
        u_int env_pgfault_handler;      // page fault state
        u_int env_xstacktop;            // top of exception stack
        u_int env_runs;                 // number of times been env_run'ed
        u_int env_nop;                  // align to avoid mul instruction
        char env_name[MAXENVLEN];
};
```
### Schedule Algorithm
In this Operating System, I implemented a double queue job scheduling. For those **runnable** processes, the time interrupt will constantly call `sched_yield()` in `lib/sched.c`, which uses the double queue scheduling algorithm. There are also backups such as Round-Robin Algorithms which is functional.

## Interrupts and Exceptions
The interrupts and exceptions can be classified into 4 cases:


## Syscall and Fork


## File System
In `fs/fsformat.c`, it generates a 64MB disk image in custom format.
### File Struct

## Pipe and Shell

## User and Permission (Partly Implemented)
Use linux's solution for reference, add a file named `fs/passwd` to store the necessary information for Users. Also, in struct file, there exist a 9-bit permission bit like "rwxr-xr--" to show the permission for 3 user groups.
