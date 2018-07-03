#include "lib.h"
#include <args.h>

int debug = 0;

//
// get the next token from string s
// set *p1 to the beginning of the token and
// *p2 just past the token.
// return:
//      0 for end-of-string
//      > for >
//      | for |
//      w for a word
//
// eventually (once we parse the space where the nul will go),
// words get nul-terminated.
#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()"

#define MAXHIS 10
char buf[1024];
char his[MAXHIS][1024] = {"\0"};
char curUser[1024] = {'\0'};
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

void builtin_cmd(char *str, int quit)
{
    if(strcmp(str, "logout") == 0)
    {
        if(quit)
        {
            writef("Logging Out...\n");
        }
        syscall_perm(1, 0, "");
        exit();
    }
    return;
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
                                writef("too many arguments\n");
                                exit();
                        }
                        argv[argc++] = t;
                        break;
                case '<':
                        if(gettoken(0, &t) != 'w'){
                                writef("syntax error: < not followed by word\n");
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
                writef("Fork Error in Shell!\n");
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
                if (debug) writef(RED(Empty Command!));
                return;
        }
        argv[argc] = 0;
   // /*
        if (1) {
                //writef("[%08x] SPAWN:", env->env_id);
                /*
        writef("Execute");
        for (i=0; argv[i]; i++)
                        writef(" %s", argv[i]);
                */
        writef("\n");
        }
   // */
    builtin_cmd(argv[0], 0);
        if ((r = spawn(argv[0], argv)) < 0)
    {
        writef(RED(Spawn Failed!\n));
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
handlecmd(char *s)
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
                                writef("too many arguments\n");
                                exit();
                        }
                        argv[argc++] = t;
                        break;
        }
    }
runit:
    if(argc == 0)
        return;
    builtin_cmd(argv[0], 1);
}

void
flush(char *buf, int offset)
{
    int i;
    for(i = 0;i<offset;i++)
    {
        writef("%c%c%c", 27, 91, 67);
    }
    for(i = 0;i<strlen(buf);i++)
    {
        writef("\b \b");
    }
 }

int
lsfile(char *name, int type, int offset, char *ret)
{
    int r, i, pos;
    char path[MAXPATHLEN], temp[MAXNAMELEN];
    char list[MAXHIS][MAXNAMELEN] = {""};
        struct Stat st;
        int fd, n;
        struct File f;
    syscall_curpath(path, 0);
        if ((r = stat(path, &st)) < 0)
    {
        for(i = 0;i < offset;i++)
        {
            writef("%c%c%c", 27, 91, 67);
        }
        writef("Stat File Failed!");
        return 0;
    }


        if ((fd = open(path, O_RDONLY)) < 0)
    {
        for(i = 0;i < offset;i++)
        {
            writef("%c%c%c", 27, 91, 67);
        }
        writef("Open Directory Failed!");
        return 0;
    }

    pos = 0;
    while ((n = readn(fd, &f, sizeof f)) == sizeof f)
    {
                if (f.f_name[0] && pos < MAXHIS)
                {
            strcpy(temp, f.f_name);
            temp[strlen(name)] = '\0';
            if(strcmp(temp, name) == 0)
            {
                strcpy(list[pos++], f.f_name);
            }
        }
    }
    close(fd);
    switch(pos)
    {
        case 0:

            return 1;
        case 1:
            strcpy(ret, &list[0][strlen(name)]);
            return 2;
        default:
            
            for(i = 0;i < offset;i++)
            {
                writef("%c%c%c", 27, 91, 68);
            }
        
            for(i = 0;i<pos;i++)
            {
                if(i%4 == 0)
                    writef("\n");
                writef("%8s  ", list[i]);
            }
            return 3;
    }

}

void
skip_space(char *p)
{
    while(*p == ' ')
        p++;
    return;
}

int
autofill(char *buf, int *length, int *off)
{
    int i, len, offset;
    int r;
    char temp[1024], *pos, ret[1024], curpath[MAXPATHLEN];
    len = *length;
    offset = *off;
    strcpy(temp, buf);
    temp[len-offset] = '\0';
    for(i = len-offset-1;i >= 0 && temp[i] != ' ';i--);
    pos = &temp[i+1];
    skip_space(temp);
    if((r = lsfile(pos, (pos == temp)?0:1, offset, ret)) == 0)
    {
        buf[0] = '#';
        return 1;
    }
    else if(r == 1)
    {
        return 0;
    }
    else if(r == 2)
    {
        flush(buf, offset);
        i = *length;
        buf[i] = 0;
        strcpy(temp, &buf[i - offset]);
        buf[i - offset] = 0;
        strcat(buf, ret);
        strcat(buf, temp);
        writef("%s", buf);
        *length = *length + strlen(ret);
        for(i=0;i<offset;i++)
            writef("%c%c%c", 27, 91, 68);
        return 0;

    }
    else
    {
        syscall_curpath(curpath, 0);
        fwritef(1, DARK_GRAY(\nbuaa_mips_%s@linux:)PURPLE(%s)"$ %s", curUser, curpath, buf);
        for(i=0;i<offset;i++)
            writef("%c%c%c", 27, 91, 68);
        
        return 0;
    }
} 

void
readline(char *buf, u_int n)
{
        int i, r, j;
    char temp[1024];
    char c = 0, d = 0;
    static int curpos = 0;
    int offset = 0;
        r = 0;
    buf[0] = 0;
        for(i=0; i<n; i++){
                if((r = read(0, &c, 1)) != 1){
                        if(r < 0)
                                writef("read error: %e", r);
                        exit();
                }
                if(c == 127 || c == '\b'){
                        if(i - offset > 0)
            {
                buf[i] = 0;
                flush(buf, offset);
                buf[i - offset - 1] = 0;
                buf = strcat(buf, &buf[i - offset]);
                writef("%s", buf);
                for(j = 0;j<offset;j++)
                {
                    writef("%c%c%c", 27, 91, 68);
                }
                c = 0;
                                i -= 2;
            }
                        else
            {
                c = 0;
                buf[i] = 0;
                                i -= 1;
            }
                }

        if(c == '\t')
        {
            if(autofill(buf, &i, &offset) == 0)
                c = 0;
            else
                c = '\r';
            i -= 1;
        }

                if(c == '\r'){
            buf[i] = 0;
            offset = 0;
            c = 0;
            strcpy(his[(curpos++)%MAXHIS], buf);
                        return;
                }
        

        switch(c){
            case 27:
                
                c = 0;
                buf[i] = 0;
                flush(buf, offset);
                strcpy(his[(curpos)%MAXHIS], buf);
                strcpy(buf, his[(--curpos)%MAXHIS]);
                writef("%s", buf);
                i = strlen(buf) - 1;
                offset = 0;
                
                break;
            case 28:

                c = 0;
                buf[i] = 0;
                flush(buf, offset);
                strcpy(buf, his[(++curpos)%MAXHIS]);
                i = strlen(buf) - 1;
                writef("%s", buf);
                offset = 0;
                break;
            case 30:

                c = 0;
                if(i - offset > 0)
                {
                    writef("%c%c%c", 27, 91, 68);
                    offset++;
                }
                i--;
                break;
            case 29:
                c = 0;
                if(offset > 0)
                {
                    offset--;
                    writef("%c%c%c", 27, 91, 67);
                }
                i--;
                break;
        }

        if(c != 0)
        {
            flush(buf, offset);
            buf[i] = 0;
            strcpy(temp, &buf[i - offset]);
            buf[i - offset] = c;
            buf[i - offset + 1] = 0;
            //strcat(buf, &c);
            strcat(buf, temp);
            writef("%s", buf);

            for(j = 0;j<offset;j++)
            {
                writef("%c%c%c", 27, 91, 68);
            }
        }

        }
        writef(RED(line too long\n));
        while((r = read(0, buf, 1)) == 1 && buf[0] != '\n')
                ;
        buf[0] = 0;
}


void
usage(void)
{
        writef("usage: sh [-dix] [command-file]\n");
        exit();
}

void
umain(int argc, char **argv)
 {
        int r, interactive, echocmds;
    char curpath[MAXPATHLEN];
        interactive = '?';
        echocmds = 0;
    /*
        writef("\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
        writef("::                                                         ::\n");
        writef("::              Super Shell  V1.0.1_1                      ::\n");
        writef("::                                                         ::\n");
        writef(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
        */
    ARGBEGIN{
        case 'd':
                debug++;
                break;
        case 'i':
                interactive = 1;
                break;
        case 'x':
                echocmds = 1;
                break;
        default:
                usage();
        }ARGEND
        if(argc > 1)
                usage();
        if(argc == 1){
        strcpy(curUser, argv[0]);
                /*
        close(0);
                if ((r = open(argv[1], O_RDONLY)) < 0)
                        user_panic("open %s: %e", r);
                user_assert(r==0);
        */
        }
        if(interactive == '?')
                interactive = iscons(0);
        for(;;){
                if (interactive)
        {
            syscall_curpath(curpath, 0);
                        fwritef(1, DARK_GRAY(\nbuaa_mips_%s@linux:)PURPLE(%s)"$ ", curUser, curpath);
        }
        readline(buf, 1024);

                if (buf[0] == '#')
                        continue;
                if (echocmds)
                        fwritef(1, "# %s\n", buf);
                if ((r = fork()) < 0)
                        user_panic("fork: %e", r);
                if (r == 0) {
                        runcmd(buf);
                        exit();
                        return;
                } else
                        wait(r);
            handlecmd(buf);
   