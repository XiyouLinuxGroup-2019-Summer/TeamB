#include <stdio.h>
int lst=100;
int judge(long long n,int st)
{
    int i;
    int div[3]={2,3,5};
    if(n==1)
    {
        printf("~~~~%d\n",st);
        if(st<lst)
            lst=st;
        return 0;
    }
    //printf("rrrrr%lldrrrrr\n",n);
    for(i=0;i<3;i++)
    {
    printf("rrrrr%lldrrrrr\n",n);
        if(n%div[i]==0)
        {
            printf("@\n");
            switch(div[i])
            {
            case 2:
               printf("^\n");
                n=n/2;
                printf("%lldooooo",n);
                //printf("%d\n",st);
                st++;
                printf("%d\n",st);
                judge(n,st);
                n=2*n;
                break;
            case 3:
            printf("*\n");
                n=(2*n)/3;
                printf("%lld",n);
                st++;
                judge(n,st);
                n=(3*n)/2;
                break;
            case 5:
                printf("!\n");
                n=(n*4)/5;
                printf("%lld",n);
                st++;
                judge(n,st);
                n=(5*n)/4;
                break;
            }
        }
    }
    printf("ggggggg%lldgggg\n",n);
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
      //  printf("-1\n");
   // return 0;
    a=judge(n,steps);
        printf("(%d)\n",a);
    return 0;
}
