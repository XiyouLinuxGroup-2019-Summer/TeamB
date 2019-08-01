#include <stdio.h>
int main()
{
    int n,m;//n家，m斤；
    float min,a,b;
    scanf("%d %d",&n,&m);
    scanf("%f %f",&a,&b);
    min=(a/b)*m;
    n--;
    while(n!=0)
    {
        scanf("%f %f",&a,&b);
        if(((a/b)*m)<min)
            min=(a/b)*m;
        n--;
    }
    printf("%.8f",min);
    return 0;
}

