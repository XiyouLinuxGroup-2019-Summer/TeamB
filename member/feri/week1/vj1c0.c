#include<stdio.h>
#include<stdlib.h>
int comp(const void*a,const void*b)
{
    return *(int*)a-*(int*)b;
}
int main()
{
    int a[1000000];
    int i,n;
    int temp=1;
    int fl=0;
    scanf("%d",&n);
    for(i=0;i<n;i++)
        scanf("%d",&a[i]);
    qsort(a,n,sizeof(int),comp);
    for(i=0;i<n;i++)
    {
        if(a[i]>=temp)
        {
            temp++;
            fl++;
        }
    }
    printf("%d\n",fl);
}
