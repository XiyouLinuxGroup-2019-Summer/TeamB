#include <stdio.h>
int main()
{
    int n,k,v,sk,m;
    scanf("%d %d",&n,&k);
    m=0;
    sk=1;
    for(v=1;v>=sk;v++)
    {
        while(v>=sk)
        {
            m=m+v/sk;
            sk=sk*k;
        }
        if(m>=n)
            break;
        else 
        {
            sk=1;
            m=0;
        }
    }
    printf("%d\n",v);
    return 0;
}

