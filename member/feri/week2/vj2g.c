#include <stdio.h>
int main()
{
    int i,j;
    int m,n;
    double maxend=0,maxdivia=0,maxdivib=0,cha,chb;
    scanf("%d %d",&m,&n);
    double room[n][2];
    while(m!=-1&&n!=-1)
    {
    maxend=0;
    maxdivia=0;
    maxdivib=0;
    for(i=0;i<n;i++)
    {
        scanf("%lf %lf",&room[i][0],&room[i][1]);
    }
    for(i=0;i<n-1;i++)
    {
        for(j=0;j<n-i-1;j++)
        {
            maxdivia=room[j][0]/room[j][1];
            maxdivib=room[j+1][0]/room[j+1][1];
            if(maxdivia<maxdivib)
            {
                cha=room[j][0];
                room[j][0]=room[j+1][0];
                room[j+1][0]=cha;
                chb=room[j][1];
                room[j][1]=room[j+1][1];
                room[j+1][1]=chb;
            }
        }
    }
    for(i=0;i<n;i++)
    {
        if(m==0)
            break;
        else if(m<=room[i][1])
        {
            maxend+=m*(room[i][0]/room[i][1]);
            m=0;
            continue;
        }
        else
        {
            maxend+=room[i][0];
            m=m-room[i][1];
        }
    }
    printf("%.3f\n",maxend);
    scanf("%d %d",&m,&n);
    }
}
