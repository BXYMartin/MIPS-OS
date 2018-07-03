#include "lib.h"
#include <mmu.h>
#include <env.h>

void
exit(void)
{
        //close_all();
        syscall_env_destroy(0);
}

void
exit_all(void)
{
        //close_all();
    writef("Destroying %08x", envs[ENVX(syscall_getenvid())].env_parent_id);
    syscall_env_destroy(envs[ENVX(syscall_getenvid())].env_parent_id);
        syscall_env_destroy(0);
}

struct Env *env;

void
libmain(int argc, char **argv)
{
        // set env to point at our env structure in envs[].
        env = 0;        // Your code here.
        //writef("xxxxxxxxx %x  %x  xxxxxxxxx\n",argc,(int)argv);
        int envid;
        envid = syscall_getenvid();
        envid = ENVX(envid);
        env = &envs[envid];
        // call user main routine
        umain(argc, argv);
        // exit gracefully
        exit();
        //syscall_env_destroy(0);
}