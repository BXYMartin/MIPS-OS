#include <env.h>
#include <pmap.h>
#include <printf.h>


/* Overview:
 *  Check whether current environment has used up its time. If yes, then 
 *  check whether the list env_sched_list[pos] is empty. If yes, change the
 *  pointer to point the other list. Next, if current list is not empty, 
 *  pop the first one and allocate running time to it.
 * Hints:
 *  The variable which is for counting should be defined as 'static'.
 */
void sched_yield_d(void)
{
    static int pos = -1;
    while(1){
        pos = (pos+1)%NENV;
        //if(envs[pos].env_status==ENV_NOT_RUNNABLE)
          //  printf("%d, %d\n", envs[pos].env_id,envs[pos].env_status);
        if(envs[pos].env_status==ENV_RUNNABLE){
            //printf("%d\n", envs[pos].env_id);
            env_run(&envs[pos]);
        }
    }
}

void sched_yield_c(void)
{
    static int pos = -1;
    
    if(curenv && curenv->env_status == ENV_RUNNABLE)
    {
            env_run(curenv);
    }    
    else
        while(1){
           pos = (pos+1)%NENV; 
           if(envs[pos].env_status==ENV_RUNNABLE){
               env_run(&envs[pos]); 
           }
        }
}


void sched_yield_l(void)
{
    int i;
    static int pos = 0;
    static int times = 0;
    static struct Env *e;
    struct Env *temp;
    e = curenv;
   
    if(--times<=0 || e->env_status != ENV_RUNNABLE)
    {
        if(e && e->env_status == ENV_RUNNABLE)
        {
        temp = LIST_FIRST(&env_sched_list[1-pos]);
        if(temp)
        {
            /*
            while(LIST_NEXT(temp, env_sched_link) != NULL)
            {
                temp = LIST_NEXT(temp, env_sched_link);
            }
            LIST_INSERT_AFTER(temp, e, env_sched_link);
            */
            LIST_INSERT_HEAD(&env_sched_list[1-pos], e, env_sched_link);
        }
        else
        {
            LIST_INSERT_HEAD(&env_sched_list[1-pos], e, env_sched_link);
        }
        }
        else
        {
        LIST_INIT(&env_sched_list[0]);
        LIST_INIT(&env_sched_list[1]);
            // for(i=0;i<NENV-1;i++)

        for(i=NENV-1;i>=0;i--)
        {
            if(envs[i].env_status == ENV_RUNNABLE)
            {
                LIST_INSERT_HEAD(&env_sched_list[1-pos], &envs[i], env_sched_link);
            }
        }
        }


        if(LIST_EMPTY(&env_sched_list[pos]))
        {
            pos = 1 - pos;
        }
        
        if(!(LIST_EMPTY(&env_sched_list[pos])))
        {
            e = LIST_FIRST(&env_sched_list[pos]);
            LIST_REMOVE(e, env_sched_link);
            times = e->env_pri;
        }
        
    }
    env_run(e);
    return;
}


void sched_yield_t(void)
{
    int i;
    static int pos = 0;
    static int times = 0;
    static struct Env *e;
    static int posid = 0;
    struct Env *tmp;
    int count;
    e = curenv;
   
    if(--times<=0) // || (e && e->env_status != ENV_RUNNABLE)
    {
        if(e)
        {

            
            LIST_INSERT_HEAD(&env_sched_list[1-pos], e, env_sched_link);
        }


        if(LIST_EMPTY(&env_sched_list[pos]))
        {
            pos = 1 - pos;
        }
        
        if(!(LIST_EMPTY(&env_sched_list[pos])))
        {
            e = LIST_FIRST(&env_sched_list[pos]);
            LIST_REMOVE(e, env_sched_link);
            times = e->env_pri;
        }
        else
        {
            //while(1){
                //printf("!");
            //}
            //env_run(NULL);
        }
    }
    if(posid != e->env_id){
        //printf("Preparing to run ENV: %d\n", e->env_id);
        posid = e->env_id;
    }
    
    count=0;
    for(i=0;i<NENV;i++){
        if(envs[i].env_status==ENV_RUNNABLE){
            count++;
        }
    }
    if(count>1){
        //printf("HaHa!");
    }
    
    env_run(e);
    return;
}


void sched_yield(void)
{
    int i;
    static int pos = 0;
    static int times = 0;
    static struct Env *e;
    if(--times<=0 || curenv == NULL || curenv->env_status!=ENV_RUNNABLE)
    {
        if(LIST_EMPTY(&env_sched_list[pos]))
        {
            pos = 1 - pos;
        }
        e = LIST_FIRST(&env_sched_list[pos]);
        if(e == NULL)
            while(1);
        LIST_REMOVE(e, env_sched_link);
        LIST_INSERT_HEAD(&env_sched_list[1-pos], e, env_sched_link);

        times = e->env_pri;
    }
    env_run(e);
}