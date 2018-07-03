#include "lib.h"

char buf[8192];

void
cp(int f, char *s)
{
        long n;
        int r;
    int t;
    if((r = create(s, FTYPE_REG)) < 0){
        fwritef(1, "File %s Already Exists!\n", s);
        return;
    }
    if((t = open(s, O_RDWR)) < 0)
    {
        fwritef(1, "Create %s Failed!\n", s);
        return;
    }
        while((n=read(f, buf, (long)sizeof buf))>0)
    {
                if((r=write(t, buf, n))!=n)
                        user_panic("write error copying %s: %e", s, r);
    }
    if(n < 0)
                user_panic("error reading %s: %e", s, n);
    close(t);
}

void
umain(int argc, char **argv)
{
        int f, i;
    char *t;
    char curpath[MAXPATHLEN];
    char path[MAXPATHLEN];
        if(argc <= 2)
                writef(1, "No Specific Files!");
        else if(argc >= 3)
    {
        t = argv[1];
        syscall_curpath(curpath, 0);
        if(t[0] == '/')
        {
            strcpy(curpath, t);
            // Do Nothing
        }
        else
        {
            if(curpath[strlen(curpath) - 1] != '/')
                strcat(curpath, "/");
            strcat(curpath, t);
        }
            
            for(i=2; i<argc; i++){
                f = open(curpath, O_RDONLY);
                if(f < 0)
                    user_panic("can't open %s: %d", argv[i], f);
                t = argv[i];
                syscall_curpath(path, 0);
                if(t[0] == '/')
                {
                    strcpy(path, t);
                    // Do Nothing
                }
                else
                {
                    if(path[strlen(path) - 1] != '/')
                        strcat(path, "/");
                    strcat(path, t);
                }

                fwritef(1, "From %s Copying to %s...\n", curpath, path);
                
                            if(strcmp(path, curpath) != 0)
                    cp(f, path);
                        close(f);
                }
        }
}