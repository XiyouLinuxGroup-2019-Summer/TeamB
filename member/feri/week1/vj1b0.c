#include <stdio.h>
int main()
{
    int m,j=0;
    int i,ma=0,mb=0;
    int k;
    scanf("%d",&m);
    scanf("%d",&k);
    while(j!=m)
    {
    int a[k];
    int max=0;
    for(i=0;i<k;i++)
    {
        scanf("%d",&a[i]);
    }
    for(i=0;i<k;i++)
    {
        if(a[i]%3==0)
            max++;
        if(a[i]%3==1)
            ma++;
        if(a[i]%3==2)
            mb++;
    }
    if(mb>ma)
    {
        max=max+ma;
    }
    else if(ma>mb)
    {
        max=max+mb;
        max=max+(ma-mb)/3;
    }
    else if(ma==mb)
    {
        max=max+ma;
    }
    printf("%d\n",max);
    j++;
}
    return 0;
}
