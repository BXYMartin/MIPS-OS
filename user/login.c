#include "lib.h"

char buf[512];

void cons_in(int ret, char *in)
{
   int i = 0;
   char c;
   syscall_read_dev(&c, 0x10000000, 4);
   while(c != 13)
   {
       if(c == 8){
           if(i > 0){
            i--;
            in[i] = '\0';
               if(ret)
                   writef("\b \b");
           }
       }
       else if(c == ' '){
            in[i] = '\0';
            return;
       }
       else if(c == 27)
       {
           do{
            syscall_read_dev(&c, 0x10000000, 1);
           }while(c == 0);
           do{
            syscall_read_dev(&c, 0x10000000, 1);
           }while(c == 0);
       }
       else if(c != 0){
            in[i++] = c;
            if(ret)
                writef("%c", c);
       }
    syscall_read_dev(&c, 0x10000000, 1);
   }
   in[i] = '\0';
   return;
}

int
readuser(char *s, int clr)
{
        long n;
        int r, p;
    static int i = 0;
    if(clr)
    {
        i = 0;
        return 1;
    }
    p = i;
    while(buf[p] != '\n' && buf[p] != '\0')
    {
        s[p++-i] = buf[p];
    }
    if(buf[p] == '\0')
    {
        s[p-i] = 0;
        p = -1;
        i = 0;
        return 0;
    }
    s[p-i] = 0;
    i = ++p;
    return 1;
}

void
getitem(char *in, int p, char *out)
{
    int i, j = 0;
    for(i = 1;i<p;i++)
    {
        while(in[j] != ':' && in[j] != '\n' && in[j] != '\0')
        {
            j++;
        }
        j++;
    }
    i = 0;
    while(in[j] != ':' && in[j] != '\n' && in[j] != '\0')
        out[i++] = in[j++];
    out[i] = '\0';
    return;
}

void
umain(int argc, char **argv)
{
    int i, r, f, try = 0;
    char *args[3], line[1024], item[1024];        
    char UserName[1024], Passwd[1024];
    char in[MAXPATHLEN];
    close(0);
    if ((r = opencons()) < 0)
        user_panic("opencons: %e", r);
    if (r != 0)
        user_panic("first opencons used fd %d", r);
    if ((r = dup(0, 1)) < 0)
        user_panic("dup: %d", r);
    while(1){
    if((f = open("/passwd", O_RDONLY)) >= 0)
    {
        read(f, buf, (long)sizeof buf);
        
        writef("\n"leftup);
        for(r = 0;r < 10;r++)
            writef(hori);
        writef(rightup"\n");
        writef("- Users    -\n", item);
        readuser("", 1);
        while(readuser(line, 0))
        {
            getitem(line, 1, item);
            writef(vert" %8s "vert"\n", item);
        }
        writef(leftdown);
        for(r = 0;r < 10;r++)
            writef(hori);
        writef(rightdown"\n");
        //readuser(line, 0);
        while(1)
        {
            writef(PURPLE(\nUser Name:));
            cons_in(1, UserName);
            writef(BLUE(\nPassword:));
            cons_in(0, Passwd);
            r = 0;
            readuser("", 1);
            while(readuser(line, 0))
            {
                getitem(line, 1, item);
                if(strcmp(item, UserName) == 0)
                {
                    r = 1;
                    break;
                }
            }
            if(r == 1)
            {
                getitem(line, 2, item);
Again:
                if(strcmp(Passwd, item) == 0)
                    break;
                else
                {
                    writef(RED(\nPassword Incorrect:));
                    cons_in(0, Passwd);
                    try++;
                    if(try < 3)
                        goto Again;
                    else
                        try = 0;
                }
            }
            readuser("", 1);
            writef(RED(\nWrong User Name or Password!));
        }
    }
    getitem(line, 4, item);
    syscall_perm(1, item[0]-'0', "");
    getitem(line, 7, item);
    writef("\nRunning Shell from %s\n", item);
    args[0] = item;
    args[1] = UserName;
    args[2] = 0;
    close(f);
    r = spawnl(args[0], args[0], args[1], args[2]);
    wait(r);
    }
}