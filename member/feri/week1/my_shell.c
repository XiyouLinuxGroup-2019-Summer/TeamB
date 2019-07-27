#include <stdio.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#define normal 0
#define outa 1
#define outb 4
#define ina 2
#define pipe 3
#define BEGIN(x,y) "\033["#x";"#y"m"
#define BLOD "\033[1m"
#define CLOSE "\033[0m"
char *input();
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
    signal(SIGINT,SIG_IGN);
    //buf=(char *)malloc(256*sizeof(char));
    while(1)
    {
        buf=input();
        if(strcmp(buf,"exit")==0||strcmp(buf,"logout")==0)
        {
            break;
        }
        if(strcmp(buf,"\0")==0)
            continue;
        for(i=0;i<100;i++)
        {
            analycmd[i][0]='\0';
        }
        argcount=0;
        explain_input(buf,&argcount,analycmd);
        do_cmd(argcount,analycmd);
    }
    free(buf);
}
char *input()
{
    char bufb[256];
    char *bufa=NULL;
    int len=0;
    getcwd(bufb,256);
    printf("\033[36;45m%s:\033[0m",bufb);
    bufa=readline(BEGIN(49,45)"FERI'S SHELL$$:"CLOSE);
    len=strlen(bufa);
    bufa[len]='\0';
    return bufa;
}
void explain_input(char *buf,int *argcount,char analycmd[][256])
{
    int number=0;
    char *p=buf,*q=buf;
    while(1)
   {
        if(*q=='\0')
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
            while(*q!=' '&&*q!='\0')
            {
                number++;
                q++;
            }
            strncpy(analycmd[*argcount],p,number);
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
    //int background=0;
    char *file;
    pid_t pid;
    for(i=0;i<argcount;i++)
    {
        arg[i]=analycmd[i];
    }
    arg[argcount]=NULL;
    if(strcmp(arg[0],"cd")==0)
    {
        chdir(arg[1]);
        return ;
    }
    for(i=0;arg[i]!=NULL;i++)
    {
        if(strcmp(arg[i],">")==0)
            how=outa;
        if(strcmp(arg[i],">>")==0)
            how=outb;
        if(strcmp(arg[i],"<")==0)
            how=ina;
        if(strcmp(arg[i],"|")==0)
        {
            how=pipe;
            if(arg[i+1]==NULL)
            {
                //flag++;
            }
            if(i==0)
            {
                //flag++;
            }
        }
    }
    if(how==outa||how==outb)
    {
        for(i=0;arg[i]!=NULL;i++)
        {
            if(strcmp(arg[i],">")==0||strcmp(arg[i],">>")==0)
            {
                file=arg[i+1];
                arg[i]=NULL;
            }
        }
    }
    if(how==ina)                                             
    {                                                        
        for(i=0;arg[i]!=NULL;i++)                            
        {                                                    
            if(strcmp(arg[i],"<")==0)
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
        if(pid==0)                                      
        {                                               
            if(!(find_command(arg[0])))                 
            {                                           
                printf("%s:command not found\n",arg[0]);
                exit(0);                                
            }                                           
            fd=open(file,O_RDWR);  
            dup2(fd,0);                                 
            execvp(arg[0],arg);                         
            exit(0);                                    
        }                                               
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
        dup2(fd2,0);
        execvp(argnext[0],argnext);
        if(remove("/tmp/youdonotkonwfile"))
            printf("remove error\n");
        exit(0);
    }
    break;
    case 4:
        if(pid==0)                                      
        {                                               
            if(!(find_command(arg[0])))                 
            {                                           
                printf("%s:command not found\n",arg[0]);
                exit(0);                                
            }                                           
            fd=open(file,O_RDWR|O_CREAT|O_APPEND,0644);  
            dup2(fd,1);                                 
            execvp(arg[0],arg);                         
            exit(0);                                    
        }                                               
        break;                                          
   /* case 5:
        if(pid==0)                                      
        {                                               
            if(!(find_command(arg[0])))                 
            {                                           
                printf("%s:command not found\n",arg[0]);
                exit(0);                                
            }                                           
            fd=open(file,O_RDONLY);                     
            dup2(fd,0);                                 
            execvp(arg[0],arg);                         
            exit(0);                                    
        }                                               
        break;                      */                    
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
