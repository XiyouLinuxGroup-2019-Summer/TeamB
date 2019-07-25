#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
void print_prompt();
void get_input(char *buf);
void explain_input(char *,int *,char *a[]);
void do_cmd(int ,char **a);
int find_command(char *);
int main(int argc,char *argv[])
{
    int i,argcount=0;
    char arglist[100][256];
    char *buf=NULL;
    buf=(char *)malloc(256*sizeof(char));
    if(buf==NULL)
    {
        perror("malloc failed\n");
        exit(-1);
    }
    while(1)
    {
        memset(buf,0,256);
        print_prompt();
        get_input(buf);
        if(strcmp(buf,"exit\n")==0||strcmp(buf,"logout\n")==0)
        {
            break;
        }
        for(i=0;i<100;i++)
        {
            arglist[i][0]='\0';
        }
        argcount=0;
        explain_input(buf,&argcount,arglist);
        do_cmd(argcount,arglist);
    }
    if(buf!=NULL)
    {
        free(buf);
        buf=NULL;
    }
    exit(0);
}
void print_prompt()
{
    printf("my_shell$$");
}
void get_input(char *buf)
{
    int len=0;
    int ch;
    ch=getchar();
    while(len<256&&ch!='\n')
    {
        buf[len++]=ch;
        ch=getchar();
    }
    if(len==256)
    {
        printf("command is too long\n");
        exit(-1);
    }
    buf[len]='\n';
    len++;
    buf[len]='\0';
}

