#include<stdio.h>
int main(){
    int n;
    scanf("%d",&n);

    int i = 0,k = 1;
    /*while(n > 0){
        n -= k;
        k *= 2;
        i++;
    }
    */

    //效率更高
    while(n>0){
        n -= k;
        k = k << 1;
         i++; 
    }

    printf("%d\n",i);
}



