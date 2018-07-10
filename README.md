# MIPS-OS
A Custom Operating System with a Shell for MIPS R3000

## Customed readELF Version
This project includes a customed readelf source code in order to load icode into memory properly.
![image](https://github.com/BXYMartin/MIPS-OS/blob/master/imgs/Icode-Loader.png)

## Memory Management
``` c++
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
```
 Within `mm/pmap.c`, a self-mapping page table is implemented. Size per page is set to 4KB for the convenience of self-mapping. The map above is the key to the whole architecture.

 Just like linux, the memory space is divided into 4 segments. For those above `0x80000000`, they are accessible only in kernel mode. The kseg0 memory space is a direct mapping for physical memory. The corresponding relationship of addresses is shown below for reference.  
 
![image](https://github.com/BXYMartin/MIPS-OS/blob/master/imgs/Addr-Translation.png)

 The MIPS architecture also treat different part of the memory differently, using cache or TLB mechanism.
 
 ![image](https://github.com/BXYMartin/MIPS-OS/blob/master/imgs/MIPS-Architecture.png)
 
 To get a better understanding of `pmap.c`, I print the call-stack of the main function.
 
 ![image](https://github.com/BXYMartin/MIPS-OS/blob/master/imgs/Pmap-Callstack.png)

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
In this Operating System, I implemented a double queue job scheduling. For those **runnable** processes, the time interrupt will constantly call `sched_yield()` in `lib/sched.c`, which uses the double queue scheduling algorithm. There are also backups such as Round-Robin Algorithms which is also functional.

### Spawn Processes using `Fork`
 ![image](https://github.com/BXYMartin/MIPS-OS/blob/master/imgs/Fork.png)
 
 After fork, child process return `0` while father process return non-zero to distinguish processes being forked.
 
 Before fork, the page fault handler for father process must be set properly. IF NOT, when spawning the child process, things might go wrong following the picture below.
 
 ![image](https://github.com/BXYMartin/MIPS-OS/blob/master/imgs/Crucial-Order.png)
 
## Interrupts and Exceptions
The interrupts and exceptions can be classified into 4 cases:
``` c++
set_except_vector(0, handle_int);
set_except_vector(1, handle_mod);
set_except_vector(2, handle_tlb);
set_except_vector(3, handle_tlb);
set_except_vector(8, handle_sys);
```

## Syscall
Some instructions with high privileges should run in kernel mode. Yet syscall is designed to be the only way to switch current running mode of CPU. According to the syscall table, this OS implemented following syscalls for different purposes.

``` c++
sys_call_table:                         // Syscall Table
.align 2
    .word sys_putchar
    .word sys_getenvid
    .word sys_yield
    .word sys_env_destroy
    .word sys_set_pgfault_handler
    .word sys_mem_alloc
    .word sys_mem_map
    .word sys_mem_unmap
    .word sys_env_alloc
    .word sys_set_env_status
    .word sys_set_trapframe
    .word sys_panic
    .word sys_ipc_can_send
    .word sys_ipc_recv
    .word sys_cgetc
    .word sys_write_dev
    .word sys_read_dev
    .word sys_curpath
    .word sys_perm
```

## File System
In `fs/fsformat.c`, it generates a 64MB disk image in custom format.
### File Struct
``` c++
struct File {
        u_char f_name[MAXNAMELEN];      // filename
        u_char f_perm[64];
        u_int f_size;                   // file size in bytes
        u_int f_type;                   // file type
        u_int f_direct[NDIRECT];
        u_int f_indirect;
        struct File *f_dir;             // the pointer to the dir where this file is in, valid only in memory.
        u_char f_pad[BY2FILE - MAXNAMELEN - 64 - 4 - 4 - NDIRECT * 4 - 4 - 4];
};
```
 The union member f_pad is used to align the size of the struct to size `BY2FILE`. Inherited from the original code, I added `u_char f_perm` to indicate the file's permission for different user groups.

### Address Space
 In this OS, file service is a separate process that is blocked until a request arrives. Therefore the address space through different processes is a little tricky.
 
 ![image](https://github.com/BXYMartin/MIPS-OS/blob/master/imgs/Address-Mapping.png)

## Pipe
 Apart from IPC System Call, processes can also communicate through pipe. The pipe is actually a shared memory slice through different env page tables.
 
 ![image](https://github.com/BXYMartin/MIPS-OS/blob/master/imgs/Pipe.png)
 
 The pipe is marked using `struct fd` - File Descriptor. 
 
 Using the `fd` struct, the pipe can handle read and write simutaneously. It can be used to assist the communication between serve processes and normal user processes.
 
 ![image](https://github.com/BXYMartin/MIPS-OS/blob/master/imgs/Addr-Space.png)
 
 To determine the correct status of the pipe, the OS uses the ref count of the corresponding page to check whether this pipe needs closing. The ref count for the file descriptor is always no greater than that of the pipe, when there equality is satisfied, this pipe is need to be closed.
 
 ![image](https://github.com/BXYMartin/MIPS-OS/blob/master/imgs/Serve-Step.png)
 
## Shell
The shell provides a interface for user to interact with OS. This OS uses `icode.b` as the launcher to launch `sh.b` and all other periperal user management processes.

Also, some works on mapping of certain function keys such as `left` and `right` direction keys, `TAB` auto-fill and history recordings are also implemented to simulate the linux terminal.

Apart from normal input, ANSI color mode is also supported using macro definitions below.
``` c++
/* Define color modes */
#define RED(str) "\033[0;32;31m" # str "\033[m"
#define LIGHT_RED(str) "\033[1;31m" # str "\033[m"
#define GREEN(str) "\033[0;32;32m" # str "\033[m"
#define LIGHT_GREEN(str) "\033[1;32m" # str "\033[m"
#define BLUE(str) "\033[0;32;34m" # str "\033[m"
#define LIGHT_BLUE(str) "\033[1;34m" # str "\033[m"
#define DARK_GRAY(str) "\033[1;30m" # str "\033[m"
#define CYAN(str) "\033[0;36m" # str "\033[m"
#define LIGHT_CYAN(str) "\033[1;36m" # str "\033[m"
#define PURPLE(str) "\033[0;35m" # str "\033[m"
#define LIGHT_PURPLE(str) "\033[1;35m" # str "\033[m"
#define BROWN(str) "\033[0;33m" # str "\033[m"
#define YELLOW(str) "\033[1;33m" # str "\033[m"
#define LIGHT_GRAY(str) "\033[0;37m" # str "\033[m"
#define WHITE(str) "\033[1;37m" # str "\033[m"
```

## User and Permission (Partly Implemented)
Use linux's solution for reference, add a file named `fs/passwd` to store the necessary information for Users. Also, in struct file, there exist a 9-bit permission bit like "rwxr-xr--" to show the permission for 3 user groups.
