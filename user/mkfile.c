#include "lib.h"

int flag[256];


void
mkfile(char *path, char *prefix)
{ 
    int r, fd;
    char curpath[MAXPATHLEN] = {'\0'};
    curpath[0] = '/';
    curpath[1] = '\0';
    syscall_curpath(curpath, 0);
    if(path[0] == '/')
    {
        // Do Nothing
    }
    else
    {
        if(curpath[strlen(curpath) - 1] != '/')
            strcat(curpath, "/");
        strcat(curpath, path);
    }

    if((r = create(curpath, FTYPE_REG)) < 0){
        fwritef(1, "File %s Already Exists!\n", curpath);
        return;
    }
    fwritef(1, "Created File %s!", curpath);
}

void
usage(void)
{ 
        fwritef(1, "usage: mkfile [-dFl] [file...]\n");
        exit();
} 

void
umain(int argc, char **argv)
{  
    int i;
    ARGBEGIN{
    default:
            usage();
    case 'd':
    case 'F':
    case 'l':
            flag[(u_char)ARGC()]++;
            break;
    }ARGEND
    if (argc == 0){
        return;    
    }
    else {
        for (i=0; i<argc; i++)
            mkfile(argv[i], argv[i]);
    }
}