#include <stdio.h>
int main()
{
    int i;
    int n,d;
    int c=0,wins=0,k=0;
    char days[101];
    scanf("%d %d",&n,&d);
    while(d--)
    {
        scanf("%s",days);
        c=0;
        for(i=0;i<n;i++)
        {
            if(days[i]=='1')
                c++;
        }
        if(c<n)
            k++;
        if(wins<k)
            wins=k;
        if(c==n)
            k=0;
    }
    printf("%d\n",wins);
    return 0;
}
