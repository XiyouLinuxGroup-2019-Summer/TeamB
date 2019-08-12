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
void srv_groupsee(PACK *pack);
void srv_friendsee(PACK *pack);
void srv_changenum(PACK *pack);
int close_mysql(MYSQL mysql);


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
        case FRI_SEE:
            srv_friendsee(pack);
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
            srv_groupsee(pack);

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

        case CHANGE_NUM:
            srv_changenum(pack);
            break;
        
        case FIND_PASSWD:
            srv_findpassword(pack);
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
    MYSQL_RES *res_ptr = NULL;  
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

    char buffer[150];
    //printf("dasdsadsa %s\n%s\n",data1->username,data1->password);
    sprintf(buffer,"select password from 用户数据 where `name` = '%s' and `password` = '%s'",data1->username,data1->password);

    res = mysql_query(&mysql, buffer); //查询语句  
    if (res){         
        printf("SELECT error:%s\n",mysql_error(&mysql));     
    }
    else{
        res_ptr = mysql_store_result(&mysql);             //取出结果集  mysql_store_result()立即检索所有的行，
        
        PACK * senddata = NULL;
        senddata = (PACK *)malloc(sizeof(PACK));

        if(mysql_num_rows(res_ptr) == 0){

            printf("login error");
            senddata->type = LOGIN;
		    senddata->data.recv_fd = pack->data.send_fd;
            senddata->data.mes[0] = 'n';
            strcpy(senddata->data.send_name,"server");

            if(send(pack->data.send_fd,senddata,sizeof(PACK),0) < 0)
                my_err("send",__LINE__);
        
        }
        else if(mysql_num_rows(res_ptr) > 0){
            
            printf("登录成功\n");  
            senddata->type = LOGIN;
		    senddata->data.recv_fd = pack->data.send_fd;
            senddata->data.mes[0] = 'y';
            strcpy(senddata->data.send_name,"server");

            if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
                my_err("send",__LINE__);
        }
             
        mysql_free_result(res_ptr);        //释放空间    
    }
    close_mysql(mysql);  
    return EXIT_SUCCESS;  
}

void userregister(PACK *pack){

    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    info_user * data1 = NULL;
    data1 = (info_user *)malloc(sizeof(info_user));
    int temp;

    if((temp == my_recvregister(pack,data1,20)) == -1){
        my_err("myrecvtemp",__LINE__);
    }

    char buffer[150];
   
    sprintf(buffer,"select password from 用户数据 where `name` = '%s'",data1->username);
    res = mysql_query(&mysql, buffer); //查询语句  
    if (res){
        printf("SELECT error:%s\n",mysql_error(&mysql));         
    }
    else{        
        res_ptr = mysql_store_result(&mysql);             //取出结果集  mysql_store_result()立即检索所有的行，
        
        
        PACK * senddata = NULL;
        senddata = (PACK *)malloc(sizeof(PACK));

        if(mysql_num_rows(res_ptr) == 0){
            printf("没有此人可以注册此账户\n");
            sprintf(buffer,"insert into 用户数据 values(NULL,%s,%s,%s,%s,0)",data1->username,data1->sex,data1->password,data1->mibao);     
            
            res = mysql_query(&mysql, buffer); //查询语句 
            if (res){
                printf("SELECT error:%s\n",mysql_error(&mysql));
                senddata->type = REGISTER;
                senddata->data.recv_fd = pack->data.send_fd;
                senddata->data.mes[0] = 'n';
                if(send(pack->data.send_fd,senddata,sizeof(PACK),0) < 0)
                    my_err("send",__LINE__);
                    return ;         
            }

            senddata->type = REGISTER;
		    senddata->data.recv_fd = pack->data.send_fd;
            senddata->data.mes[0] = 'y';
            strcpy(senddata->data.send_name,"server");

            if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
                my_err("send",__LINE__);
            

        }
        else if(mysql_num_rows(res_ptr) > 0){
            printf("该账户已存在，请提示重新输入\n");
            senddata->type = REGISTER;
		    senddata->data.recv_fd = pack->data.send_fd;
            senddata->data.mes[0] = 'n';
            if(send(pack->data.send_fd,senddata,sizeof(PACK),0) < 0)
                my_err("send",__LINE__);

        }

       
        

    /*    printf("%lu Rows\n",(unsigned long)mysql_num_rows(res_ptr));   //返回所有的行
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
        } */              
        mysql_free_result(res_ptr);        //释放空间
    }
    close_mysql(mysql); 
}

void srv_deletefriend(PACK *pack){

    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    close_mysql(mysql); 
}

void srv_addfriend(PACK *pack){

    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    close_mysql(mysql); 
}

void srv_groupquit(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    close_mysql(mysql); 
}

void srv_groupjoin(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    close_mysql(mysql); 
}

void srv_findpassword(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    close_mysql(mysql); 
}

void srv_chatwithfriend(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    close_mysql(mysql); 
}

void srv_chatwithgroup(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    close_mysql(mysql); 
}

void srv_groupkick(PACK *pack){
    
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    close_mysql(mysql); 
}

void srv_frienchat(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    close_mysql(mysql); 
}


void srv_groupsee(PACK *pack){

    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    close_mysql(mysql); 
}


void srv_friendsee(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    close_mysql(mysql); 
}

void srv_groupdel(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    close_mysql(mysql); 
}

void srv_creategroup(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    close_mysql(mysql); 
}

void srv_findpassword(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    close_mysql(mysql); 
}

void srv_changenum(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    close_mysql(mysql); 
}


int close_mysql(MYSQL mysql){
	mysql_close(&mysql);
	mysql_library_end();
	printf("end\n");
	return 0;
}