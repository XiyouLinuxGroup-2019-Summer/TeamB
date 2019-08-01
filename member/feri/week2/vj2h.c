#include <stdio.h>
#include <string.h>
int main()
{
    int i,j,com,coma,minfei=0,max=0,zon=0;
    int N,T;
    int count=0;
    scanf("%d",&T);
    while(T--)
    {
        minfei=0;
        max=0;
        zon=0;
        count=0;
    scanf("%d",&N);
    int stu[N][N];
    for(i=0;i<N;i++)
    {
        scanf("%d",&stu[i][0]);
    }
    int maxdata=stu[0][0];
    for(i=0;i<N;i++)
    {
        if(stu[i][0]>maxdata)
            maxdata=stu[i][0];
    }
    int a[maxdata+1];
    for(i=0;i<maxdata+1;i++)
        a[i]=0;
    for(j=0;j<N;j++)
    {
        scanf("%d",&stu[j][1]);
        zon+=stu[j][1];
    }
    for(i=0;i<N-1;i++)
    {
        for(j=0;j<N-1-i;j++)
        {
            if(stu[j][1]<stu[j+1][1])
            {
                com=stu[j][1];
                stu[j][1]=stu[j+1][1];
                stu[j+1][1]=com;
                coma=stu[j][0];
                stu[j][0]=stu[j+1][0];
                stu[j+1][0]=coma;
            }
        }
    }
    j=0;
    while(j<N)
    {
        for(i=0;i<maxdata+1;i++)
        {
            if(stu[j][0]==i&&a[i]==0)
            {
                a[i]=1;
                max+=stu[j][1];
                j++;
                break;
            }
            else if(stu[j][0]==i&&a[i]==1)
            {
                while(i>0)
                {
                    if(a[i]==0)
                    {
                        a[i]=1;
                        max+=stu[j][1];
                        break;
                    }
                    else
                    {
                        i--;
                    }
                }
                j++;
                break;
            }
        }
    }
    minfei=zon-max;
    printf("%d\n",minfei);
}
    return 0;
}
