#include <stdio.h>
#include <malloc.h>
struct node
{
    int num;
    struct node *next;
};
struct node *buy(struct node *head);
int main()
{
    int flag=6,i;
    struct node *ptemp,*s,*r;
    int a[6];
    for(i=0;i<6;i++)
    {
        a[i]=i;
    }
    r=(struct node *)malloc(sizeof(struct node));
    r->next=NULL;
    ptemp=r;
    i=0;
    while(flag--)
    {
        s=(struct node *)malloc(sizeof(struct node));
        s->num=a[i];
        i++;
        ptemp->next=s;
        ptemp=s;
    }
    ptemp->next=NULL;
    struct node *idex;
    idex=r->next;
    while(idex)
    {
        printf("%d\n",idex->num);
        idex=idex->next;
    }
    struct node *idexa;
    idexa=buy(r);
    while(idexa)
    {
        printf("%d\n",idexa->num);
        idexa=idexa->next;
    }
    return 0;
}
struct node *buy(struct node *head)
{
    struct node *p,*q;
    p=head->next;
    head->next=NULL;
    while(p)
    {
        q=p;
        p=p->next;
        q->next=head->next;
        head->next=q;//新入节点所指向的下一位节点
    }
    return q;
}
