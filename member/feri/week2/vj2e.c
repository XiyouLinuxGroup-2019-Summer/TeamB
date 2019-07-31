#include <stdio.h>
int main()
{
    int earn=0;
    int i,j,swap;
    int n,m;
    scanf("%d %d",&n,&m);
    int saleprice[n];
    for(i=0;i<n;i++)
    {
        scanf("%d",&saleprice[i]);
    }
    for(i=0;i<n-1;i++)
    {
        for(j=0;j<n-i-1;j++)
        {
            if(saleprice[j]>saleprice[j+1])
            {
                swap=saleprice[j];
                saleprice[j]=saleprice[j+1];
                saleprice[j+1]=swap;
            }
        }
    }
    for(i=0;i<m;i++)
    {
        if(saleprice[i]<=0)
        {
            earn+=saleprice[i];
        }
    }
    earn=-1*earn;
    printf("%d\n",earn);
    return 0;
}

