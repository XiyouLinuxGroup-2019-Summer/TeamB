#include<stdio.h>
#include<string.h>
int main()
{
    int i,j;
    char x[202],b[202];
 
    while(fgets(x,202,stdin))
    {
        if(strcmp(x,"START")==0)
            continue;
        if(strcmp(x,"END")==0)
            continue;
        if(strcmp(x,"ENDOFINPUT")==0)
            break;        
                            
        j=strlen(x);  
        for(i=0;i<j;++i)
        {
            if(x[i]>='A'&&x[i]<='Z')
            {
                if(x[i]+21>'Z')
                    b[i]=(x[i]-5);
                else b[i]=(x[i]+21);                
            }
            else
                b[i]=x[i];
        }
     
        for(i=0;i<j;i++)
            printf("%c",b[i]);
        printf("\n");        
    }
return 0;         
}
