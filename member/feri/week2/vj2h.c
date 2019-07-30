#include <stdio.h>
#include <string.h>
int main()
{
    int i,j,com;
    int N,T;
    int count=0;
    scanf("%d",&T);
    while(T--)
    {
        count=0;
    scanf("%d",&N);
    int stu[N][N];
    int a[N];
    memset(stu,0,N);
    memset(a,0,N);
    for(i=0;i<N;i++)
    {
        scanf("%d",&stu[i][0]);
        a[i]=stu[i][0];
    }
    for(j=0;j<N;j++)
        scanf("%d",&stu[j][1]);
    for(i=0;i<N-1;i++)
    {
        for(j=0;j<N-1-i;j++)
        {
            if(stu[j][1]<stu[j+1][1])
            {
                com=stu[j][1];
                stu[j][1]=stu[j+1][1];
                stu[j+1][1]=com;
            }
        }
    }
    j=0;
    for(i=0;i<N;i++)
    {
        if(stu[j][0]==a[i])
        {
            a[i]=1;
            j++;
        }
    }
    for(i=0;i<N;i++)
    {
        if(a[i]==1)
            count++;
    }
    printf("%d\n",count);
}
    return 0;
}
