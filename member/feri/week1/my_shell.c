#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#define normal 0
#define out 1
#define in 2
#define pipe 3
void input(char *buf);
void print_prompt();
void explain_input(char *buf,int *argcount,char analycmd[][256]);
void do_cmd(int argcount,char analycmd[][256]);
int find_command(char *);
int main()
{
    int argcount=0;
    int i;
    char analycmd[100][256];
    char *buf=NULL;
    buf=(char *)malloc(256*sizeof(char));
    if(buf==NULL)
    {
        perror("malloc failed");
        exit(-1);
    }
    while(1)
    {
        memset(buf,0,256);
        print_prompt();
        input(buf);
        if(strcmp(buf,"exit\n")==0||strcmp(buf,"logout\n")==0)
        {
            break;
        }
        for(i=0;i<100;i++)
        {
            analycmd[i][0]='\0';
        }
        argcount=0;
        explain_input(buf,&argcount,analycmd);
        do_cmd(argcount,analycmd);
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
    printf("\033[28;45mferi's_shell$$:\033[0m");
}
void input(char *buf)
{
    char ch;
    int len=0;
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
    buf[len++]='\n';
    buf[len]='\0';
}
void explain_input(char *buf,int *argcount,char analycmd[][256])
{
    int number=0;
    char *p=buf,*q=buf;
    while(1)
   {
        if(*q=='\n')
        {
            break;
        }
        if(*p==' ')
        {
            p++;
        }
        else
        {
            q=p;
            number=0;
            while(*q!=' '&&*q!='\n')
            {
                number++;
                q++;
            }
            strncpy(analycmd[*argcount],p,number+1);
            analycmd[*argcount][number]='\0';
            *argcount=*argcount+1;
            p=q;
            p++;
        }
    }
}
void do_cmd(int argcount,char analycmd[][256])
{
    int fd;
    int i,j;
    char *arg[argcount+1];
    char *argnext[argcount+1];
    int how=0;
    int status;
    int flag=0;
    //int background=0;
    char *file;
    pid_t pid;
    for(i=0;i<argcount;i++)
    {
        arg[i]=analycmd[i];
    }
    arg[argcount]=NULL;
    for(i=0;arg[i]!=NULL;i++)
    {
        if(strcmp(arg[i],">")==0)
        {
            flag=1;
            how=out;
            if(arg[i+1]==NULL)
            {
                flag++;
            }
        }
        if(strcmp(arg[i],"<")==0)
        {
            flag++;
            how=in;
            if(i==0)
            {
                flag++;
            }
        }
        if(strcmp(arg[i],"|")==0)
        {
            flag++;
            how=pipe;
            if(arg[i+1]==NULL)
            {
                flag++;
            }
            if(i==0)
            {
                flag++;
            }
        }
    }
    if(flag>1)
    {
        printf("wrong command\n");
        return;
    }
    if(how==out)
    {
        for(i=0;arg[i]!=NULL;i++)
        {
            if(strcmp(arg[i],">")==0)
            {
                file=arg[i+1];
                arg[i]=NULL;
            }
        }
    }
    if(how==pipe)
    {
        for(i=0;arg[i]!=NULL;i++)
        {
            if(strcmp(arg[i],"|")==0)
            {
                arg[i]=NULL;
                for(j=i+1;arg[j]!=NULL;j++)
                {
                    argnext[j-i-1]=arg[j];
                }
                argnext[j-i-1]=arg[j];
                break;
            }
        }
    }
    if((pid=fork())<0)
    {
        printf("fork error\n");
       return ;
    }
    switch(how)
    {
    case 0:
        if(pid==0)
        {
            if(!(find_command(arg[0])))
            {
                printf("%s:command not found\n",arg[0]);
                exit(0);
            }
            execvp(arg[0],arg);
            exit(0);
        }
        break;
    case 1:
        if(pid==0)
        {
            if(!(find_command(arg[0])))                 
            {                                           
                printf("%s:command not found\n",arg[0]);
                exit(0);                                
            }                                           
            fd=open(file,O_RDWR|O_CREAT|O_TRUNC,0644);
            dup2(fd,1);
            execvp(arg[0],arg);
            exit(0);
        }
        break;
    case 2:
        break;
    case 3:
    if(pid==0)
    {
        int pid2;
        int status2;
        int fd2;
        if((pid2=fork())<0)
        {
            printf("fork2 error\n");
            return ;
        }
        else if(pid2==0)
        {
            if(!(find_command(arg[0])))
            {
                printf("%s:command not found\n",arg[0]);
                exit(0);
            }
            fd2=open("/tmp/youdonotkonwfile",O_WRONLY|O_CREAT|O_TRUNC,0644);
            dup2(fd2,1);
            execvp(arg[0],arg);
            exit(0);
        }
        if(waitpid(pid2,&status2,0)==-1)//父进程阻塞
        {
            printf("wait for child process error\n");
        }
        if(!(find_command(argnext[0])))
        {
            printf("%s:command not found\n",argnext[0]);
            exit(0);
        }
        fd2=open("/tmp/youdonotkonwfile",O_RDONLY);
        dup2(fd2,1);
        execvp(argnext[0],argnext);
        if(remove("/tmp/youdonotkonwfile"))
            printf("remove error\n");
        exit(0);
    }
    break;
    default :
        break;
    }
    if(waitpid(pid,&status,0)==-1)
    {
        printf("wait for child prosess error\n");
    }
}
int find_command(char *command)
{
    DIR *dir;
    struct dirent *dirp;
    int i=0;
    char *path[4]={"./","/bin","/usr/bin",NULL};
    if(strncmp(command,"./",2)==0)
        command=command+2;
    while(path[i]!=NULL)
    {
        if((dir=opendir(path[i]))==NULL)
            printf("can not open /bin");
        while((dirp=readdir(dir))!=NULL)
        {
            if(strcmp(dirp->d_name,command)==0)
            {
                closedir(dir);
                return 1;
            }
        }
        closedir(dir);
        i++;
    }
    return 0;
}
