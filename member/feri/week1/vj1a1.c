#include <stdio.h>
int main()
{
    int k,m=1;
    int step=0;
    long long n;
    scanf("%d",&k);
    while(m<=k)
    {
        scanf("%lld",&n);
        while(n!=1)
        {
            if(n%2==0)
            {
                n=n/2;
                step++;
            }
            else if(n%3==0)
            {
                n=(2*n)/3;
                step++;
            }
            else if(n%5==0)
            {
                n=(4*n)/5;
                step++;
            }
            else
            {
                step=-1;
                n=1;
            }
        }
        printf("%d\n",step);
        step=0;
        m++;
     }
    return 0;
}
































