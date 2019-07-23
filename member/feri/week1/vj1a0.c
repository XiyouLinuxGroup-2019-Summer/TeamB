#include <stdio.h>
int lst=100;
int judge(long long n,int st)
{
    int i;
    int div[3]={2,3,5};
    if(n==1)
    {
        if(st<lst)
            lst=st;
        return 0;
    }
    for(i=0;i<3;i++)
    {
        if(n%div[i]==0)
        {
            switch(div[i])
            {
            case 2:
                n=n/2;
                st++;
                judge(n,st);
                n=2*n;
                break;
            case 3:
                n=(2*n)/3;
                st++;
                judge(n,st);
                n=(3*n)/2;
                break;
            case 5:
                n=(n*4)/5;
                st++;
                judge(n,st);
                n=(5*n)/4;
                break;
            }
        }
    }
    if(n!=1&&n%2!=0&&n%3!=0&&n%5!=0)
    {
        printf("-1\n");
        return -1;
    }
    return lst;
}
int main()
{
    int a;
    long long int n;
    int steps=0;
    scanf("%lld",&n);
    if(n%7==0||n%11==0||n%13==0)
    {
        printf("-1\n");
        return 0;
    }
    a=judge(n,steps);
        printf("%d\n",a);
    return 0;
}
