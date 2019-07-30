#include <stdio.h>
#include <string.h>
int main ()
{
    int n, i, days, years, sum;
    char month[10];
    char *s1[19 = {"pop", "no", "zip", "zotz", "tzec", "xul", "yoxkin", "mol", "chen", "yax", "zac", "ceh", "mac", "kankin", "muan", "pax", "koyab", "cumhu", "uayet"};
    char *s2[20] = {"imix", "ik", "akbal", "kan", "chicchan", "cimi", "manik", "lamat", "muluk", "ok", "chuen", "eb", "ben", "ix", "mem", "cib", "caban", "eznab", "canac", "ahau"};   
    scanf("%d", &n);
    printf("%d\n", n); 
    while ( n-- ) 
    {
        scanf("%d. %s %d", &days, month, &years);
        for ( i = 0; i < 19; i++ ) 
        {
            if ( strcmp(*(s1+i),month) == 0 )
                break;
        }
        sum = years*365+i*20+days+1;
        years=sum/260;
        days= sum%13;
        if ( day == 0 )
            day = 13;
        i = sum%20;
        if ( i == 0 )
            i = 20;
        if ( sum%260 == 0 ) 
        {
            printf("%d %s %d\n", days, *(s2+i-1), years-1);
            continue;
        }
        printf("%d %s %d\n", days, *(s2+i-1), years);
    }

    return 0;
}
