#include "my_serthread.h"
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include "my_recv.h"
#include <stdlib.h>
#include <mysql/mysql.h>

int threadlogin(PACK *pack);
void userregister(PACK *pack);
void mysqlinit(MYSQL *mysql);
void recive(login_data * data1,PACK *recvdata){
void srv_deletefriend(PACK *pack);
void srv_addfriend(PACK *pack);
void srv_groupquit(PACK *pack);
void srv_groupdel(PACK *pack);
void srv_groupjoin(PACK *pack);
void srv_findpassword(PACK *pack);
void srv_frienchat(PACK *pack);
void srv_chatwithfriend(PACK *pack);
void srv_chatwithgroup(PACK *pack);
void srv_creategroup(PACK *pack);
void srv_groupkick(PACK *pack);


    static char *pread; //指向下一次读取数据的位置
    int i;

    //如果自定义缓冲区没有数据，则从套接字中读取数据
    //从自定义缓冲区中读取一次数据
    int len_remain = strlen(recvdata->data.mes);
    int lenn = recvdata->data.mes;
    int len = 100;
    int wei = 0;

    for(i = 0;*pread != '\0';i++){
        if(wei == 0){
            if(i > len){
                return -1;
            }
            if(*pread!='\n'){
                data1->username[i] = *pread++;
                len_remain--;
            }
            else{
                wei++;
            }
        }
        else if(wei == 1){
            if(i > len){
                return -1;
            }
            if(*pread!='\n'){
                data1->password[i] = *pread++;
                len_remain--;
            }
            else{
                wei++;
            }
        }
    }

    //去除结束标志
    len_remain--;
    pread++;

    return i;
    //读取成功
}

void *myallthread(PACK *pack){

    switch(pack->type)
    {
        case LOGIN:
            threadlogin(pack);
            break;
        case REGISTER:
            userregister(pack);
            break;
        case FRI_ADD:
            srv_addfriend(pack);
            break;
        case FRI_DEL:
            srv_deletefriend(pack);
            break;

        case FRI_CHA:
            //srv_frienchat(pack);
            break;

        case GROUP_CREATE:
            srv_creategroup(pack);
            break;

        case GROUP_JOIN:
            srv_groupjoin(pack);
            break;

        case GROUP_QIUT:
            srv_groupquit(pack);
            break;

        case GROUP_DEL:
            srv_groupdel(pack);
            break;

        case GROUP_SEE:
            
            break;

        case CHAT_ONE:
            srv_frienchat(pack);
            break;

        case CHAT_MANY:
            break;

        case GROUP_KICK:
            srv_groupkick(pack);
            break;

        case GROUP_SET:
            break; 

        default:
            break;
    }
}


void mysqlinit(MYSQL *mysql){


    if(NULL == mysql_init(mysql)){
		my_err("mysql_init", __LINE__);
	}

    //初始化一个句柄;
    //初始化数据库
	if(mysql_library_init(0, NULL, NULL) != 0){
		my_err("mysql_library_init", __LINE__);
	}


    if(mysql_real_connect(mysql, "127.0.0.1", "root", "123456", "xunchat",0, NULL, 0) == NULL){
        my_err("mysql_real_connect", __LINE__);
    }
    //连接数据库的重要一步：

    //设置中文字符集
	if(mysql_set_character_set(mysql, "utf8") < 0){
		my_err("mysql_set_character_set", __LINE__);
	}
}


int threadlogin(PACK *pack){
    
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);  

    login_data * data1 = NULL;
    data1 = (login_data *)malloc(sizeof(login_data));
    int temp;

    if((temp == my_recvlogin(pack,data1,20)) == -1){
        my_err("myrecvtemp",__LINE__);
    }

    char buffer[20];
    sprintf(buffer,"select password from 用户数据 where `name` = '%s'",data1->username);

    res = mysql_query(&mysql, buffer); //查询语句  
    if (res){         
        printf("SELECT error:%s\n",mysql_error(&mysql));     
    }
    else{        
        res_ptr = mysql_store_result(&mysql);             //取出结果集  mysql_store_result()立即检索所有的行，
        if(res_ptr){   
            //	int num = (unsigned long)mysql_num_rows(res_ptr);
            printf("%lu Rows\n",(unsigned long)mysql_num_rows(res_ptr));   //返回所有的行
            j = mysql_num_fields(res_ptr);//获取 列数    
            while((sqlrow = mysql_fetch_row(res_ptr)))  
            {   //依次取出记录  
                for(i = 0; i < j; i++){
                    printf("%s\t", sqlrow[i]);              //输出  
                }
                printf("\n");          
            }              
            if (mysql_errno(&mysql)){                      
                fprintf(stderr,"Retrive error:s\n",mysql_error(&mysql));               
            }        
        }        
        mysql_free_result(res_ptr);        //释放空间


        PACK * senddata = NULL;
		senddata = (PACK *)malloc(sizeof(PACK));
		senddata->type = LOGIN;
		senddata->data.recv_fd = pack->data.send_fd;
		//senddata->data.send_fd = conn_fd;
		strcmp(senddata->data.send_name,"server");
		


        if(strcmp(sqlrow[1],data1->password) == 0){
            senddata->data.mes[0] = 'y';
            if(send(pack->data.send_fd,senddata,sizeof(PACK),0) < 0)
                my_err("send",__LINE__);
        }
        else{
            senddata->data.mes[0] = 'n';
            if(send(pack->data.send_fd,senddata,sizeof(PACK),0) < 0)
                my_err("send",__LINE__);
        }
        
    }
    /*else 
    {  
        printf("Connection failed\n");  
    }  */

    mysql_close(&mysql);  
    return EXIT_SUCCESS;  
}

void userregister(PACK *pack){

    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    INFO_USER * data1 = NULL;
    data1 = (INFO_USER *)malloc(sizeof(INFO_USER));
    int temp;

    if((temp == my_recvregister(pack,data1,20)) == -1){
        my_err("myrecvtemp",__LINE__);
    }

    char buffer[20];
    sprintf(buffer,"select password from 用户数据 where `name` = '%s'",data1->username);
}

void srv_deletefriend(PACK *pack){

}

void srv_addfriend(PACK *pack){

}

void srv_groupquit(PACK *pack){

}

void srv_groupjoin(PACK *pack){

}

void srv_findpassword(PACK *pack){

}

void srv_chatwithfriend(PACK *pack){

}

void srv_chatwithgroup(PACK *pack){

}

void srv_groupkick(PACK *pack){

}

void srv_frienchat(PACK *pack){

}