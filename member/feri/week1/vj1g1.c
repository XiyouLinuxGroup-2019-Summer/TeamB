#include <stdio.h>
int main()
{
    int i;
    int n,k,v;
    scanf("%d %d",&n,&k);
    int low=1,high=n;
    int mid,sum,ok=1;
    while(low<=high&&ok==1)
    {
        if(low==high)
            ok=0;
        mid=(low+high)/2;
        sum=mid;
        for(i=k;mid/i>0;i*=k)
        {
            sum+=mid/i;
        }
        if(sum<n)
            low=mid+1;
        if(sum>=n)
        {
            high=mid-1;
            v=mid;
        }
    }
    printf("%d\n",v);
    return 0;
}

