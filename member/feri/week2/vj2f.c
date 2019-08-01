#include <stdio.h>
int main()
{
    int k,i;
    int any,divi;
    scanf("%d",&k);
    if(k>36)
    {
        printf("-1\n");
    }
    else
    {
        divi=k/2;
        any=k%2;
    for(i=0;i<any;i++)
    {
        printf("4");
    }
    for(i=1;i<=divi;i++)
    {
        printf("8");
    }
    }
    return 0;
}

