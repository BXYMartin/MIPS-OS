#include "lib.h"

int flag[256] = {0};

void
ps(char *env, char *prefix)
{
    int pos = 0;
    fwritef(1, "\tPID\t%16s\tPRI\tRUNS\n", "NAME");
    while(pos < NENV)
    {
        if(envs[pos].env_status==ENV_RUNNABLE)
        {
            fwritef(1, BROWN(\t%d)"\t%16s\t%d"CYAN(\t%d\n), envs[pos].env_id, envs[pos].env_name, envs[pos].env_pri, envs[pos].env_runs);
        }
        pos++;
    }
}

void
usage(void)
{
        fwritef(1, "usage: ps [-dFl] [file...]\n");
        exit();
}

void
umain(int argc, char **argv)
{
    int i;
    for(i = 0;i < 256;i++)
    {
        flag[i] = 0;
    }
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
                ps("", "");
        }
        else {
                for (i=0; i<argc; i++)
                        ps(argv[i], argv[i]);
        }
}