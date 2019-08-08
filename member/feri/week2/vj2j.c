#include <stdio.h>
int main()
{
    int i,j,cha,chb,first;
    int n;
    int most=0;
    scanf("%d",&n);
    while(n)
    {
    int time[n][2];
    for(i=0;i<n;i++)
        scanf("%d %d",&time[i][0],&time[i][1]);
    for(i=0;i<n-1;i++)
    {
        for(j=0;j<n-1-i;j++)
        {
            if(time[j][1]>time[j+1][1])
            {
                cha=time[j][1];
                time[j][1]=time[j+1][1];
                time[j+1][1]=cha;
                chb=time[j][0];
                time[j][0]=time[j+1][0];
                time[j+1][0]=chb;
            }
        }
    }
    first=time[0][1];
    most=1;
    for(i=0;i<n;i++)
    {
        if(i+1==n-1)
        {
            if(time[i+1][0]>=first)
            {
            most++;
            first=time[i+1][1];
            }
            break;
        }
        else if(i+1<n-1)
        {
            if(time[i+1][0]>=first)
            {
                most++;
                first=time[i+1][1];
                continue;
            }
            else if(time[i+1][0]<first)
            {
                continue;
            }
        }
    }
    printf("%d\n",most);
    scanf("%d",&n);
    }
}
