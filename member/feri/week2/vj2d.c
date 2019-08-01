#include <stdio.h>
int main()
{
    int i;
    int income=0,sum=0;
    int N,A,B,C,T;
    scanf("%d %d %d %d %d",&N,&A,&B,&C,&T);
    int mess[N+1];
    for ( i = 1 ; i <= N ; i++ )
    {
        scanf("%d",&mess[i]);
        sum+=T-mess[i];
    }
    if((C-B)>0)
    {
        income=N*A+sum*(C-B);
    }
    if((C-B)<=0)
    {
        income=N*A;
    }
    printf("%d\n",income);
    return 0;
}

