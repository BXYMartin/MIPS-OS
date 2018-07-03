#include "lib.h"
#include <args.h>

#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()"
char buf[1024] = {'\0'};

int debug = 0;

int
_gettoken(char *s, char **p1, char **p2)
{
        int t;

        if (s == 0) {
                //if (debug > 1) writef("GETTOKEN NULL\n");
                return 0;
        }

        //if (debug > 1) writef("GETTOKEN: %s\n", s);

        *p1 = 0;
        *p2 = 0;

        while(strchr(WHITESPACE, *s))
                *s++ = 0;
        if(*s == 0) {
        //      if (debug > 1) writef("EOL\n");
                return 0;
        }
        if(strchr(SYMBOLS, *s)){
                t = *s;
                *p1 = s;
                *s++ = 0;
                *p2 = s;
//              if (debug > 1) writef("TOK %c\n", t);
                return t;
        }
        *p1 = s;
        while(*s && !strchr(WHITESPACE SYMBOLS, *s))
                s++;
        *p2 = s;
        if (debug > 1) {
                t = **p2;
                **p2 = 0;
//              writef("WORD: %s\n", *p1);
                **p2 = t;
        }
        return 'w';
}

int
gettoken(char *s, char **p1)
{
        static int c, nc;
        static char *np1, *np2;

        if (s) {
                nc = _gettoken(s, &np1, &np2);
                return 0;
        }
        c = nc;
        *p1 = np1;
        nc = _gettoken(np2, &np1, &np2);
        return c;
}

#define MAXARGS 16
void
runcmd(char *s)
{ 
        char *argv[MAXARGS], *t;
    char curpath[MAXPATHLEN];
        int argc, c, i, r, p[2], fd, rightpipe;
        int fdnum;
        rightpipe = 0;
        gettoken(s, 0);
again:
        argc = 0;
        for(;;){
                c = gettoken(0, &t);
                switch(c){
                case 0:
                        goto runit;
                case 'w':
                        if(argc == MAXARGS){
                                fwritef(1, "too many arguments\n");
                                exit();
                        }
                        argv[argc++] = t;
                        break;
                case '<':
                        if(gettoken(0, &t) != 'w'){
                                fwritef(1, "syntax error: < not followed by word\n");
                                exit();
                        }
                        // Your code here -- open t for reading,

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
            fdnum = open(curpath, O_RDONLY);
            dup(fdnum, 0);
            close(fdnum);
            goto runit;
                        // dup it onto fd 0, and then close the fd you got.
                        //user_panic("< redirection not implemented");
                        break;
                case '>':
                        // Your code here -- open t for writing,
            c = gettoken(0, &t);
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
            fdnum = open(curpath, O_RDWR);
            dup(fdnum, 1);
            close(fdnum);
            goto runit;
                        // dup it onto fd 1, and then close the fd you got.
                        //user_panic("> redirection not implemented");
                        break;
                case '|':
                        // Your code here.
                        //      First, allocate a pipe.
                        pipe(p);
            //  Then fork.
            r = fork();
            if(r < 0)
            {
                fwritef(1, "Fork Error in Shell!\n");
                break;
            }
                        //      the child runs the right side of the pipe:
            else if(r == 0)
            {
                        //              dup the read end of the pipe onto 0
                dup(p[0], 0);
                        //              close the read end of the pipe
                close(p[0]);
                        //              close the write end of the pipe
                close(p[1]);
                        //              goto again, to parse the rest of the command line
                goto again;
            }
                        //      the parent runs the left side of the pipe:
            else
            {
                        //              dup the write end of the pipe onto 1
                dup(p[1], 1);
                        //              close the write end of the pipe
                close(p[1]);
                        //              close the read end of the pipe
                close(p[0]);
                        //              set "rightpipe" to the child envid
                rightpipe = r;
                        //              goto runit, to execute this piece of the pipeline
                goto runit;

            }
                        //                      and then wait for the right side to finish
            //user_panic("| not implemented");
                        break;
                }
        }

runit:
        if(argc == 0) {
                if (debug) fwritef(1, "Empty Command!");
                return;
        }
        argv[argc] = 0;
   // /*
        if (1) {
                //writef("[%08x] SPAWN:", env->env_id);
                fwritef(1, "\nExecute");
        for (i=0; argv[i]; i++)
                        fwritef(1, " %s", argv[i]);
                fwritef(1, "\n");
        }
   // */
        if ((r = spawn(argv[0], argv)) < 0)
    {
        fwritef(1, "Spawn Failed!\n");
                //writef("spawn %s: %e\n", argv[0], r);
    }
    close_all();
        if (r >= 0) {
                //if (debug) writef("[%08x] WAIT %s %08x\n", env->env_id, argv[0], r);
                wait(r);
        }
        if (rightpipe) {
                //if (debug) writef("[%08x] WAIT right-pipe %08x\n", env->env_id, rightpipe);
                wait(rightpipe);
        }

        exit();
}



void
get(int f)
{
        long n;
    char *cur, s[1024], temp[1024], buf[1024];
        int r;
    int i, j;
    i = 0;
    temp[0] = 0;
    buf[0] = 0;
    // Bug!!!
        while((i == 0 && (n = read(f, buf, (long)sizeof buf) > 0)) || buf[i++] != '\0')
    {
        strcat(temp, buf);
        strcpy(buf, temp);
        cur = &buf[i];
        j = i;
        for(;buf[i] != '\n' && buf[i] != '\0';i++)
            s[i - j] = buf[i];
        s[i - j] = '\0';
        if(buf[i] == '\n')
        {
            temp[0] = 0;
            if((r = fork()) == 0)
            {
                runcmd(s);
                exit();
                return;
            }
            else
            {
                wait(r);
            }
            continue;
        }
        else
        {
            buf[0] = '\0';
            i = 0;
            strcpy(temp, s);
            continue;
        }
    }
    if(n < 0)
                user_panic("error reading %s: %e", s, n);
    fwritef(1, "\n\nReached End-Of-File!");
}

void
umain(int argc, char **argv)
{
        int f, i;
    char *t;
    char curpath[MAXPATHLEN];
        if(argc == 1)
                fwritef(1, "Too Few Arguments!");
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
                        user_panic("can't open %s: %d", argv[i], f);
                else{
                        get(f);
                        close(f);
                }
        }
}