#include <stdio.h>
int main()
{
    int m,j=0;
    int i,ma=0,mb=0;
    int k;
    scanf("%d",&m);
    int max[m];
    for(i=0;i<m;i++)
    {
        max[i]=0;
    }
    scanf("%d",&k);
    while(j!=m)
    {
        int a[k];
        for(i=0;i<k;i++)
        {
            scanf("%d",&a[i]);
        }
        for(i=0;i<k;i++)
        {
            if(a[i]%3==0)
                max[j]++;
            if(a[i]%3==1)
                ma++;
            if(a[i]%3==2)
                mb++;
        }
        /*if(max[j]==0)
        {
            if(ma>mb)
            {
            max[j]=mb+(ma-mb)/3;
            }
            else if(ma<mb)
            max[j]=ma+(mb-ma)/3;
            else if(ma==mb)
            max[j]=ma;
        }*/
            if(mb>ma)
            {
                max[j]=max[j]+ma;
                max[j]=max[j]+(mb-ma)/3;
            }
            else if(ma>mb)
            {
                max[j]=max[j]+mb;
                max[j]=max[j]+(ma-mb)/3;
            }
             else if(ma==mb)
            {
                max[j]=max[j]+ma;
            }
        j++;
}
for(i=0;i<m;i++)
{
    printf("%d\n",max[i]);
}
return 0;
}
