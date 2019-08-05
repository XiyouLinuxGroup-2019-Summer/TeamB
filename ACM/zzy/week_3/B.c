#include<stdio.h>
#include<string.h>
#include<math.h>
int book[10][10];
int count;
void dfs(int n,int lie){
    if (lie == n){
        count++;
        return;
    }
    for(int i = 0;i < n;i++){
        if(book[i][lie] == -1){
            book[i][lie] = lie;
            /* printf("\nlie = %d\n",lie); */
            for(int a = 0; a < n;a++){
                for(int b = 0;b < n;b++){
                    if(book[a][b] == -1 && (a == i || b == lie || abs(a-i) == abs(b - lie))){
                        book[a][b] = lie;
                    }
                }
            }
        
            dfs(n,lie+1);
            for(int a = 0; a < n;a++){
                for(int b = 0;b < n;b++){
                    if(book[a][b] ==  lie){
                        book[a][b] = -1;
                    }
                }
            }
        }
    }
}
int main(){
    int n;
    while(1){  
        scanf("%d",&n);
        if(n == 0)
            break;
        
        memset(book,-1,sizeof(book));
        count = 0;

        dfs(n,0);
        
        printf("%d\n",count);
    }
    return 0;
}
