#include <stdio.h>
int main()
{
    int step=0;
    long long n;
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
            printf("-1\n");
            n=1;
            return 0;
        }
    }
    printf("%d\n",step);
    return 0;
}
