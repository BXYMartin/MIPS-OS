#include "../user/lib.h"
#include <unistd.h>
#include <asm/asm.h>
#include <pmap.h>
#include <env.h>
#include <printf.h>
#include <kclock.h>
#include <trap.h>

extern char aoutcode[];
extern char boutcode[];

#define L1 1000
#define L2 100
#define L3 1

void prout(int per, char *stage)
{
    int i;
    static curp = 0;
    if(curp != per)
    {
    printf(RED(\r%3d%%)" "LIGHT_GREEN(|), per);
    for(i = 0;i < per/5;i++)
    {
        printf(BAR);
    }
    while(++i < 20)
    {
        printf(" ");
    }
    printf(LIGHT_GREEN(|)" "GREEN(%s), stage);
    printf("      ");
    }
    curp = per;
}

void pause(int a, int b, char *stage)
{
    int i, j, k;
    for(i = a;i < b+1;i++)
    {
        prout(i, stage);

        for(j = 0;j < L2;j++)
        {
            for(k = 0;k < L1;k++)
            {
                ;
            }
        }
        
    }
}

void progress(int per, char *stage)
{
    int i;
    static int curper = 0;
    pause(per, per, stage);
    curper = per;
}

void mips_init()
{
        //printf("init.c:\tmips_init() is called\n");
        printf(GREEN(Operating System is booting...\n));

    progress(0, "Initializing...");
    mips_detect_memory();
        progress(10, "Detecting Memory...");
        mips_vm_init();
        progress(20, "Initializing Memory...");
        page_init();
        progress(30, "Initializing Pages...");

        env_init();
        progress(40, "Initializing Process...");
    //env_check();
//      ENV_CREATE(user_fktest);
//      ENV_CREATE(user_pingpong);
//      ENV_CREATE_PRIORITY(user_pingpong, 2);
//  ENV_CREATE(user_idle);
//  ENV_CREATE(user_ipc_p0);
//  ENV_CREATE(user_ipc_p1);
//  ENV_CREATE(user_checkdev);
//  ENV_CREATE(user_fstest);
//  ENV_CREATE(fs_serv);
//  ENV_CREATE(user_testpipe);
//  ENV_CREATE(user_testpiperace);
//  ENV_CREATE(user_checkdev);
    ENV_CREATE(user_icode);
        progress(60, "Setting runtime loader...");
    ENV_CREATE(fs_serv);
        progress(80, "Running File Service...");
//    ENV_CREATE(user_testpipe);
//    ENV_CREATE(user_testpiperace);
//    ENV_CREATE(fs_serv);
    trap_init();
        progress(90, "Initializing Trap...");
        kclock_init();
        progress(100, "Clock starting...");


        while(1);
        panic("init.c:\tend of mips_init() reached!");
}

void bcopy(const void *src, void *dst, size_t len)
{
        void *max;

        max = dst + len;
        // copy machine words while possible
        while (dst + 3 < max)
        {
                *(int *)dst = *(int *)src;
                dst+=4;
                src+=4;
        }
        // finish remaining 0-3 bytes
        while (dst < max)
        {
                *(char *)dst = *(char *)src;
                dst+=1;
                src+=1;
        }
}

void bzero(void *b, size_t len)
{
        void *max;

        max = b + len;

        //printf("init.c:\tzero from %x to %x\n",(int)b,(int)max);

        // zero machine words while possible

        while (b + 3 < max)
        {
                *(int *)b = 0;
                b+=4;
        }

        // finish remaining 0-3 bytes
        while (b < max)
        {
                *(char *)b++ = 0;
        }

}