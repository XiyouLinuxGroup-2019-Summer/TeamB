#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<dirent.h>


#define normal           0   //一般命令
#define out_redirect     1   //输出重定向
#define in_redirect      2   //输入重定向(会覆盖)
#define in_add_redirect  4   //输入重定向(追加)
#define out_add_redirect 5   //输出重定向(追加)
#define have_pipe        3   //命令中有管道

void print_prompt();                            //打印提示符
void get_input(char *);                         //得到输入命令
void explain_input(char *,int *,char a[][256]);     //对输入命令进行解析
void do_cmd(int,char a[][256]);                     //执行命令
int find_command(char *);                       //查找命令中的可执行程序


int main(int argc,char **argv){
    int i;
    int argcount = 0;
    char arglist[100][256];
    char **arg = NULL;
    char *buf = NULL;
    buf = (char *)malloc(256);
    if(buf == NULL){
        perror("malloc failed");
        exit(-1);
    }

    while(1){
        memset(buf,0,256);  //将buf指向的空间清零
        print_prompt();     //打印提示符
        get_input(buf);     //接收输入命令
        
        if(strcmp(buf,"exit\n") == 0)   break;  //输入exit时，退出本程序
        
        for(i = 0;i < 100;i++){     //初始化
            arglist[i][0] = '\0';
        }
        argcount = 0;   //命令个数

        explain_input(buf,&argcount,arglist);   //对输入命令进行解析
        do_cmd(argcount,arglist);   //执行命令
    }

    if(buf != NULL){
        free(buf);
        buf = NULL;
    }

    exit(0);
}



//打印提示符
void print_prompt(){
    printf("[myshell]zzy@zzy:");
}




void get_input(char *buf){   //  获取用户输入
    int len = 0;
    int ch;

    ch = getchar();
    while(len < 256 && ch != '\n'){ //直到输入‘\n’时停止
        buf[len++] = ch;
        ch = getchar();
    }

    //printf("%s",buf);
    if(len == 256){     //输入命令过长
        printf("Command is too long!\n");
        exit(-1);
    }

    buf[len] = '\n';
    len++;
    buf[len] = '\0';
}

//解析 buf 中存的命令，将结果存入 arglist 中，命令以回车符号 \n 做结尾
//若输入命令为 "ls -l /tmp" 则arglist[0],arglist[1],arglist[2]分别为 ls -l 和 /tmp
void explain_input(char *buf,int *argcount,char arglist[100][256]){
    char  *p = buf;
    char  *q = buf;
    int   number = 0;
    while(1){
        if(p[0] == '\n')
            break;

        if(p[0] == ' '){
            p++;
        }
        else{
            q = p;
            number = 0;
            while((q[0] != ' ') && (q[0] != '\n')){
                number++;
                q++;
            }
            strncpy(arglist[*argcount],p,number+1);
            arglist[*argcount][number] = '\0';
            *argcount = *argcount + 1;
            p = q;
        }
    }
}

//查找命令中的可执行程序
int find_command(char *command){
    DIR *dp;
    struct dirent *dirp;
    char *path[] = {"./","/bin","/usr/bin",NULL};

    //使得当前目录下的程序可以运行
    if(strncmp(command,"./",2) == 0)
        command = command + 2;

    //分别在当前目录，/bin，和/usr/bin目录查找要执行的程序
    int i = 0;
    while(path[i] != NULL){
        if((dp = opendir(path[i])) == NULL)
            printf("Can not open /bin!\n");
        while((dirp = readdir(dp)) != NULL){
            if(strcmp(dirp->d_name,command) == 0){
                closedir(dp);
                return 1;
            }
        }
        closedir(dp);
        i++;
    }
    return 0;
}



//执行命令
void do_cmd(int argcount,char arglist[100][256]){
    int flag = 0;
    int how = 0;            //指示指令知否含有> ,< ,| ,
    int background = 0;     //标识命令中是否有后台运行标识符&
    int status;
    int i,fd;
    char    *arg[argcount+1];
    char    *argnext[argcount+1];
    char    *file;
    pid_t   pid;

    for(i = 0;i < argcount;i++){    //取出命令
        arg[i] = (char *)arglist[i];
    }
    arg[argcount] = NULL;

    for(i = 0;i < argcount;i++){    //查看是否存在后台运行符
        if(strncmp(arg[i],"&",1) == 0){
            if(i == argcount-1){
                background = 1;
                arg[argcount-1] = NULL;
                break;
            }
            else{
                printf("wrong command!\n");
                return;
            }
        }
    }

    for(i = 0;arg[i] != NULL;i++){
        if(strcmp(arg[i],">") == 0){
            flag++;
            how = out_redirect;
            if(arg[i+1] == NULL)
                flag++;
      
        }

        /*if(strcmp(arg[i],">>") == 0){
            flag++;
            how = out_add_redirect;
            if(arg[i+1] == NULL)
                flag++;
        }*/
        
        if(strcmp(arg[i],"<") == 0){
            flag++;
            how = in_redirect;
            if(i == 0)
                flag++;
        }

        /*if(strcmp(arg[i],"<<") == 0){

            flag++;
            how = out_add_redirect;
            if(i == 0)
                flag++;
        }*/

        if(strcmp(arg[i],"|") == 0){
            flag++;
            how = have_pipe;
            if(arg[i+1] == NULL)
                flag++;
            if(i == 0)
                flag++;
        }

        //flag大于1，说明命令中含有多个>,<,|，不支持,或者格式错误，如"ls -l /tmp >"
        if(flag > 1){
            printf("wrong command!\n");
            return;
        }

        if(how == out_redirect){ //命令只含有一个输出重定向符号 ">"
            for(i = 0;arg[i] != NULL;i++){
                if(strcmp(arg[i],">") == 0){
                    file = arg[i+1];
                    arg[i] = NULL;
                }
            }
        }
        
        /*if(how == out_add_redirect){
		    for(i = 0;arg[i] != NULL;i++){
			    if(strcmp(arg[i],">>") == 0){
				    file = arg[i+1];
				    arg[i] = NULL;
				}
		    }
	    }*/

        if(how == in_redirect){ //命令只含有一个输入重定向符号 "<" 
            for(i = 0;arg[i] != NULL;i++){
                if(strcmp(arg[i],"<") == 0){
                    file = arg[i+1];
                    arg[i] = NULL;
                }
            }
        }

        /*if(how == in_add_redirect){
		    for(i = 0;arg[i] != NULL;i++){
			    if(strcmp(arg[i],"<<") == 0){
				file = arg[i+1];
				arg[i] = NULL;
			    }
		    }
	    }*/
        
        if(how == have_pipe){   //  命令行只含有一个管道符号
            for(i = 0;arg[i] != NULL;i++){  //将管道后面的部分存入argnext中，管道后面部分也是一个可执行的shell命令
                if(strcmp(arg[i],"|") == 0){
                    arg[i] = NULL;
                    int j;
                    for(j = i+1;arg[i] != NULL;j++){
                        argnext[j-i-1] = arg[j];
                    }
                    argnext[j-i-1] = arg[j];
                    break;
                }
            }
        }

        if((pid = fork()) < 0){
            printf("fork error!\n");
            return;
        }

        switch(how){
            case 0:
                //pid == 0说明是子进程,在程序中执行输入命令
                //输入命令不含>,<,和|
                if(pid == 0){
                    if(!(find_command(arg[0]))){
                        printf("%s command not found!\n",arg[0]);
                        exit(0);
                    }
                    execvp(arg[0],arg);
                    exit(0);
                }
                break;
            case 1:
                //输入命令含有输入重定向符>
                if(pid == 0){
                      if(!(find_command(arg[0]))){
                          printf("%s command not found!\n",arg[0]);
                          exit(0);
                      }
                      fd = open(file,O_RDWR|O_CREAT|O_TRUNC,0644);
                      dup2(fd,1);
                      execvp(arg[0],arg);
                      exit(0);
                  }
                  break;
            case 2:
                  //输入命令含有输出重定向符<
                if(pid == 0){
                        if(!(find_command(arg[0]))){
                            printf("%s command not found!\n",arg[0]);
                            exit(0);
                        }
                        fd = open(file,O_RDONLY);
                        dup2(fd,0);
                        execvp(arg[0],arg);
                        exit(0);
                    }
                    break;
            case 3:
                    //输入命令含有管道符|
                if(pid == 0){
                        int pid2;
                        int status2;
                        int fd2;
                        if((pid2 = fork()) < 0){
                            printf("fork2 error!\n");
                            return;
                        }
                        else if (pid2 == 0){
                            if(!(find_command(arg[0]))){
                                printf("%s : command not fount!\n",arg[0]);
                                exit(0);
                            }
                            fd2 = open("tmp/youdonotknowf",O_WRONLY | O_CREAT | O_TRUNC,0644);
                            dup2(fd2,1);
                            execvp(arg[0],arg);
                            exit(0);
                        }
                        if(waitpid(pid2,&status2,0) == -1){
                            printf("wait for child process error!\n");
                            //exit(0);
                        }
                        if( !(find_command(argnext[0])) ){
                            printf("%s : command not found!\n",argnext[0]);
                            exit(0);
                        }
                        fd2 = open("/temp/youdonotknowf",O_RDONLY);
                        dup2(fd2,0);
                        execvp(argnext[0],argnext);

                        if(remove("/temp/youdonotknowd"))
                            printf("remove error!\n");
                        exit(0);
                
                }
        
                break;
            default:
                break;
        }

        //命令中有&,则标识后台执行，父进程直接返回，不等待子进程结束
        if(background == 1){
            printf("[process id %d]",pid);
            return;
        }

        //父进程等待子进程结束
        if(waitpid(pid,&status,0) == -1){
            printf("wait for child process error!\n");
        }

    }
}
