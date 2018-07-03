#include "lib.h"

int flag[256];

char *cons_in()
{
   int i = 0;
   char c;
   char in[MAXPATHLEN];
   syscall_read_dev(&c, 0x10000000, 4);
   while(c != 13)
   {
       if(c == 8){
           if(i > 0){
            i--;
            in[i] = '\0';
           }
           writef("------------------\n>%s", in);
       }
       else if(c == ' '){
            writef(" ");
            in[i] = '\0';
            return in;
       }
       else if(c != 0){
        in[i++] = c;
        writef("%c", c);
       }
    syscall_read_dev(&c, 0x10000000, 1);
   }
   writef("\n");
   in[i] = '\0';
   return in;
}

void
rm(char *path, char *prefix)
{ 
    int r, fd;
    struct Stat st;
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
        strcat(curpath, "/");
        strcat(curpath, path);
    }
    if ((r=stat(curpath, &st)) < 0)
    {
        fwritef(1, "File %s not Exist!\n", path);
        syscall_curpath("", 3);
        return;
    }
    
    fwritef(1, "Delete %s, Are you sure?\n>>", path);

    if(strcmp(cons_in(), "YES") == 0){
        remove(curpath);
    }
}

void
usage(void)
{
        fwritef(1, "usage: rm [-dFl] [file...]\n");
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
                    rm(argv[i], argv[i]);
    }
}