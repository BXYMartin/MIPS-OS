#include "lib.h"

int flag[256];

void
cd(char *path, char *prefix)
{ 
    int r, fd;
    struct Stat st;
    char curpath[MAXPATHLEN] = {'\0'};
    curpath[0] = '/';
    curpath[1] = '\0';
    syscall_curpath(curpath, 0);
    if(path[0] == '/')
    {
        syscall_curpath(path, 2);
    }
    else if(strcmp(path, "..") == 0)
    {
        syscall_curpath("", 3);
    }
    else
    {
        if(curpath[strlen(curpath) - 1] != '/')
            strcat(curpath, "/");
        strcat(curpath, path);
        syscall_curpath(curpath, 1);
    }
    syscall_curpath(path, 0);
    if ((r=stat(path, &st)) < 0)
    {
        fwritef(1, RED(Wrong Path %s!\n), path);
        syscall_curpath("", 3);
        return;
    }
    if (st.st_isdir)
    {
        return;
    }
    else
    {
        fwritef(1, RED(Not a Directory!\n));
        syscall_curpath("", 3);
    }
}

void
usage(void)
{
        fwritef(1, "usage: cd [-dFl] [file...]\n");
        exit();
}

void
umain(int argc, char **argv)
{ 
    int i;
    char curpath[MAXPATHLEN];
    ARGBEGIN{
    default:
            usage();
    case 'd':
    case 'F':
    case 'l':
            flag[(u_char)ARGC()]++;
            break;
    }ARGEND
    syscall_curpath(curpath, 0);
    if (argc == 0){
        return;    
    }
    else {
        for (i=0; i<argc; i++)
                    cd(argv[i], argv[i]);
    }
    syscall_curpath(curpath, 0);
    fwritef(1, GREEN(Enter Directory %s), curpath);
}