#include "lib.h"

char buf[8192];

void
cat(int f, char *s)
{
        long n;
        int r;

        while((n=read(f, buf, (long)sizeof buf))>0)
    {
                if((r=write(1, buf, n))!=n)
                        user_panic("write error copying %s: %e", s, r);
        if(strcmp(buf, "`") == 0)
        {
            writef("\nExit with Code 0");
            return;
        }
    }
    if(n < 0)
                user_panic("error reading %s: %e", s, n);
}

void
umain(int argc, char **argv)
{
        int f, i;
    char *t;
    char curpath[MAXPATHLEN];
        if(argc == 1)
                cat(0, "<stdin>");
        else for(i=1; i<argc; i++){
        t = argv[i];
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
                f = open(curpath, O_RDONLY);
                if(f < 0)
        {
                        return;
        }
        else{
                        cat(f, argv[i]);
                        close(f);
                }
        }
}