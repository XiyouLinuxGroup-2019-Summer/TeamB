#include <stdio.h>
int lst=100;
int xlst;
long long m;
void judge(long long n,int st)
{
    int i;
    int div[3]={2,3,5};
    if(n==m&&n%2!=0&&n%3!=0&&n%5!=0)
    {
        printf("-1\n");
        return ;
    }
     if(n==1)          
     {              
         if(st<lst) 
             lst=st;
         return ;   
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
        if(i==2&&n!=m&&(n%2==0||n%3==0||n%4==0))
        {
            return ;
        }
        else if(i==2&&n%2!=0&&n%3!=0&&n%5!=0&&n!=m)
        {
            xlst=1;
            return ;
        }
    }
    if(xlst==1)
    {
        printf("-1\n");
        return ;
    }
    else
    {
        printf("%d\n",lst);
    }
    return;
}
int main()
{
    long long int n;
    int steps=0;
    scanf("%lld",&n);
    m=n;
    judge(n,steps);
    return 0;
}
