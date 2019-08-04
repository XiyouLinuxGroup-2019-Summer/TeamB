#include <stdio.h>
#include <unistd.h>

#include "threadpool.h"

void sum(void *args)
{
        int i, count = 0;
        for (i = 0; i <= 99; i++){
                count++;
        }
        printf("%d\n", count);
}
int main()
{
        Threadpool *pool = Threadpoolinit(4);
        Threadpoolcreate(pool);
        Threadpoolpushtask(pool, sum, NULL);
        Threadpoolpushtask(pool, sum, NULL);
        Threadpoolpushtask(pool, sum, NULL);
        Threadpoolpushtask(pool, sum, NULL);
        Threadpoolpushtask(pool, sum, NULL);
        Threadpoolpushtask(pool, sum, NULL);
        sleep(2);
        Threadpoolstop(pool);
        Threadpooldestory(pool);
        return 0;
}

