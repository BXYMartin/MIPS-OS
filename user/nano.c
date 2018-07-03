#include "lib.h"
#define MAXPAGE 10

char buf[8192];
char file[MAXPAGE][8192] = {""};

void
flush(char *buf, int offset)
{
    int i;
    for(i = 0;i<offset;i++)
    {
        fwritef(1, "%c%c%c", 27, 91, 67);
    }
    for(i = 0;i<strlen(buf);i++)
    {
        fwritef(1, "\b \b");
    }
 }

void
readfile(int f, char *s)
{
    long n;
    int r;
    int i = 0, j;
    while((n = read(f, buf, (long)sizeof buf))>0)
    {
        strcpy(file[i++], buf);
    }
    //disp(0, s);
    if(n < 0)
        user_panic("error reading %s: %e", s, n);
}

void
savefile(int f, char *s)
{
    fwritef(f, "%s", s);
}

int
inputline(char *buf)
{
    int i, r, j;
    char temp[1024];
    char c = 0, d = 0;
    int offset = 0;
    r = 0;
    fwritef(1, "%s", buf);
        for(i=strlen(buf);c != '`'; i++){
                if((r = read(0, &c, 1)) != 1){
                        if(r < 0)
                                fwritef(1, "read error: %e", r);
                        exit();
                }
                if(c == 127 || c == '\b'){
                        if(i - offset > 0)
            {
                buf[i] = 0;
                flush(buf, offset);
                buf[i - offset - 1] = 0;
                buf = strcat(buf, &buf[i - offset]);
                fwritef(1, "%s", buf);
                for(j = 0;j<offset;j++)
                {
                    fwritef(1, "%c%c%c", 27, 91, 68);
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


        if(c == '\r'){
            buf[i] = 0;
            offset = 0;
            c = 0;
            flush(buf, offset);
            return 0;
        }
        

        switch(c){
            case 27:
                
                c = 0;
                buf[i] = 0;
                flush(buf, offset);
                i = strlen(buf) - 1;
                offset = 0;
                return -1;
                break;
            case 28:

                c = 0;
                buf[i] = 0;
                flush(buf, offset);
                i = strlen(buf) - 1;
                offset = 0;
                return 1;
                break;
            case 30:

                c = 0;
                if(i - offset > 0)
                {
                    fwritef(1, "%c%c%c", 27, 91, 68);
                    offset++;
                }
                i--;
                break;
            case 29:
                c = 0;
                if(offset > 0)
                {
                    offset--;
                    fwritef(1, "%c%c%c", 27, 91, 67);
                }
                i--;
                break;
        }

        if(c != 0)
        {
            flush(buf, offset);
            buf[i] = 0;
            strcpy(temp, &buf[i - offset]);
            buf[i - offset] = 0;
            strcat(buf, &c);
            strcat(buf, temp);
            fwritef(1, "%s", buf);

            for(j = 0;j<offset;j++)
            {
                fwritef(1, "%c%c%c", 27, 91, 68);
            }
        }

        }
        buf[i] = 0;
        return -2;
}

void getline(char *buf, int l, char *ans)
{
    int i, j, s, t = l;
    s = 0;
    if(l < 0)
        return;
    for(i = 0;i<strlen(buf);i++)
    {
        if(buf[i] == '\n' || buf[i] == '\r')
        {
            l--;
        
            if(l < 0)
            {
                for(j = s;j<i;j++)
                    ans[++l] = buf[j];
                ans[++l] = 0;
                return;
            }
            s = i + 1;
        }
        
    }
    svline(buf, "", t);
    strcpy(ans, &buf[strlen(buf) - 1]);
    ans[0] = 0;
    return;
}

void svline(char *buf, char *in, int l)
{
    int i, j, s;
    char ans[8192];
    s = 0;
    for(i = 0;i<strlen(buf);i++)
    {
        if(buf[i] == '\n' || buf[i] == '\r')
        {
            l--;
        
            if(l < 0)
            {
                strcpy(ans, buf);
                ans[s] = 0;
                strcat(ans, in);
                strcat(ans, "\n");
                strcat(ans, &buf[i+1]);
                strcpy(buf, ans);
                return;
            }
            s = i + 1;
        }
        
    }
    strcat(buf, "\n");
    strcat(buf, in);
    return;
}

void
active(int curpage, char *s)
{
    int curline = 0, i, edit = 0;
    char buf[8192];
    fwritef(1, LIGHT_RED(============== %s =============\n), s);
    fwritef(1, "%s\n", file[curpage]);
    getline(file[curpage], curline, buf);
    while(buf!=NULL)
    {
        edit = i;
        if(edit == 0)
        {
            fwritef(1, GREEN(\rEditing Line %d: ), curline);
            edit = 1;
        }
        else
        {
            fwritef(1, RED(\rEditing Line %d: ), curline);
        }
        if((i = inputline(buf)) < -1)
        {
            break;
        }
        //for();
        svline(file[curpage], buf, curline);
        curline = (curline + i < 0)?0:(curline + i);
        getline(file[curpage], curline, buf);
    }
} 

void
umain(int argc, char **argv)
{ 
    int f, i;
    char *t;
    char curpath[MAXPATHLEN];
        if(argc == 1)
                fwritef(1, "Too few arguments!");
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
                f = open(curpath, O_RDWR);
                if(f < 0)
                        fwritef(1, "Can't open %s: %d\n", argv[i], f);
                 else{
                        readfile(f, argv[i]);
                        active(0, argv[i]);
                        ftruncate(f, 0);
                        if(1)
                            seek(f, 0);
                        savefile(f, file[0]);
                        close(f);
                }
         }
}