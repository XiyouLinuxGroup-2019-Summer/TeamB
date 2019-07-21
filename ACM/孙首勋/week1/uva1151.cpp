#include <bits/stdc++.h>
using namespace std;
const int maxn = 1000 + 5;
const int maxm = maxn*(maxn);
int T,n,m,rec,q,cnt,pa[maxn], x[maxn],y[maxn];
struct node {
    int n,c,a[maxn];
}p[9];

int findf(int x){
    if(x == pa[x])
        return x;
    else 
        return pa[x] = findf(pa[x]);
}

bool cmp(int a,int b){
    return a < b;
}
int main(){
    
    scanf("%d",&T);
    while(T--) {
        scanf("%d%d",&n,&q);
        for(int i=0;i<q;i++) {
            scanf("%d%d",&p[i].n,&p[i].c);
            for(int j=1;j<=p[i].n;j++) scanf("%d",&p[i].a[j]);
        }
        for(int i=1;i<=n;i++) scanf("%d%d",&x[i],&y[i]);
        cnt = 0;
        

        for(int i = 0;i < n;i++){
            int fx = findf(x[i]);
            int fy = findf(y[i]);

            if(fx!=fy){
                pa[x[i]] = y[i];
            }
        }


    }
    return 0;
}
