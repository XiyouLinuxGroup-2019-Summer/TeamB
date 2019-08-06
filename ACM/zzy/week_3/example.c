#include <cstdio>
#include <cmath>
int qizi[20];//qizi【i】=j表示 第i行第j列下有棋 
int biao[11];//结果存到表中，不存会超时 
int n;
int qingkuang;
bool judge(int hang)
{
	for(int i=1;i<hang;i++)//扫之前下过棋每一行是否有与此次所下棋的位置同列的 或同对角线的 
	{
		if(qizi[i]==qizi[hang]||abs(hang-i)==abs(qizi[hang]-qizi[i]))//对角线的话斜率的绝对值=1 
		return false; 
	}
	return true;
}
 
void dfs(int hang)
{
	if(hang==n+1)//比如n=2，然后该第二行下棋了，第二行如果能成功选择的话，那么那么新的行数3就等于n+1=3了 ，实在不懂举个例子看看 
	qingkuang++;
	else
	{
		for(int j=1;j<=n;j++)//在该行选第几列 
		{
			qizi[hang]=j;
			if(judge(hang))
			{
				dfs(hang+1);//在本行能下棋的话，就接着下下一行的棋 
			}
		} 
	}
}
void cnt(int n)
{
	dfs(1);//从第一行开始选地方下棋 
}
int main()
{
	for(n=1;n<=10;n++)
	{
		qingkuang=0;
		cnt(n);
		biao[n]=qingkuang;
	}
	int q;
	while(scanf("%d",&q)!=EOF)
	{
		if(q==0)
		break;
		printf("%d\n",biao[q]);
	}
	return 0;
}
