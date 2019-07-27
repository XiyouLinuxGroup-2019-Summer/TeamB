#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
    int i,len;
    char cifers[205];
    char chart[26]={'V','W','X','Y','Z','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U'};
    char input[10];
    while(gets(input))
    {
        if(strcmp(input,"ENDOFINPUT")==0)
        {
            break;
        }
        else if(strcmp(input,"START")==0)
        {
            gets(cifers);
            len=strlen(cifers);
            for(i=0;i<len;i++)
            {
                if(cifers[i]>='A'&&cifers[i]<='Z')
                {
                    cifers[i]=chart[cifers[i]-'A'];
                }
            }
            printf("%s\n",cifers);
            memset(cifers,0,len);
        }
    }
    return 0;
}
