#include "my_serthread.h"


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
            srv_groupchat(pack);
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
        case FRIQUE:
            judgeaddfri(pack);
            break;
        case GROQUE:
            judgeaddgro(pack);
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
    memset(buffer,0,sizeof(buffer));
    //printf("dasdsadsa %s\n%s\n",data1->username,data1->password);
    sprintf(buffer,"select uid from 用户数据 where `name` = '%s' and `password` = '%s'",data1->username,data1->password);

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

            if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
                my_err("send",__LINE__);
        
        }
        else if(mysql_num_rows(res_ptr) > 0){   
            printf("登录成功\n"); 

            


            infouser_node_t * pos = NULL;
            pos = (infouser_list_t)malloc(sizeof(infouser_node_t));
            pos->data.socket_id = pack->data.recv_fd;


            printf("%lu Rows\n",(unsigned long)mysql_num_rows(res_ptr));   //返回所有的行
            j = mysql_num_fields(res_ptr);//获取 列数 
            while((sqlrow = mysql_fetch_row(res_ptr)))  
            {   //依次取出记录  

                strcpy(pos->data.username,data1->username);

                for(i = 0; i < j; i++){
                    printf("%s\t", sqlrow[i]);
                    pos->data.uid = atoi(sqlrow[i]);              //输出  
                }
                List_AddHead(head,pos);
                printf("\n");          
            }
            if(mysql_errno(&mysql)){                      
                fprintf(stderr,"Retrive error:s\n",mysql_error(&mysql));               
            }


            
            
            senddata->type = LOGIN;
		    senddata->data.recv_fd = pos->data.uid;
            senddata->data.mes[0] = 'y';
            strcpy(senddata->data.send_name,"server");   
            if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
                my_err("send",__LINE__); 




            
            
            memset(buffer,0,sizeof(buffer));
            sprintf(buffer,"select * from 个人消息 where suid = %d",pos->data.uid);
            res = mysql_query(&mysql, buffer); //查询语句  
            res_ptr = mysql_store_result(&mysql);             //取出结果集  mysql_store_result()立即检索所有的行，

            printf("%lu Rows\n",(unsigned long)mysql_num_rows(res_ptr));   //返回所有的行
            j = mysql_num_fields(res_ptr);//获取 列数 
            while((sqlrow = mysql_fetch_row(res_ptr)))  
            {   //依次取出记录
                senddata = (PACK *)malloc(sizeof(PACK));

                senddata->data.send_fd = atoi(sqlrow[0]);
                senddata->data.recv_fd = atoi(sqlrow[4]);
                strcmp(senddata->data.mes,sqlrow[2]);
                printf("xxxxxxxxxx%s\n\n",senddata->data.mes);
                senddata->type = atoi(sqlrow[3]);

                for(i = 0; i < j; i++){
                    printf("%s\t", sqlrow[i]);              //输出  
                }

                if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
                    my_err("send",__LINE__); 
                //printf("\n");          
            }

            if (mysql_errno(&mysql)){                      
                fprintf(stderr,"Retrive error:s\n",mysql_error(&mysql));               
            }
          
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
    memset(buffer,0,sizeof(buffer));
    sprintf(buffer,"select uid from 用户数据 where `name` = '%s'",data1->username);
    res = mysql_query(&mysql, buffer); //查询语句  
    if (res){
        printf("SELECT error:%s\n",mysql_error(&mysql));         
    }
    else{        
        res_ptr = mysql_store_result(&mysql);             //取出结果集  mysql_store_result()立即检索所有的行，
        
        PACK * senddata = NULL;
        senddata = (PACK *)malloc(sizeof(PACK));

        if(mysql_num_rows(res_ptr) == 0){
            memset(buffer,0,sizeof(buffer));
            printf("没有此人可以注册此账户\n");
            sprintf(buffer,"insert into 用户数据 values(NULL,'%s','%s','%s','%s')",data1->username,data1->sex,data1->password,data1->mibao);     
            printf("%s\n\n",buffer);
            res = mysql_query(&mysql, buffer); //查询语句 
            if (res){
                printf("SELECT error:%s\n",mysql_error(&mysql));
                senddata->type = REGISTER;
                senddata->data.recv_fd = pack->data.send_fd;
                senddata->data.mes[0] = 'n';
                if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
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
            if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
                my_err("send",__LINE__);

        }

       
        

        /*printf("%lu Rows\n",(unsigned long)mysql_num_rows(res_ptr));   //返回所有的行
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

    char buffer[250];
    memset(buffer,0,sizeof(buffer));

    int sendnum,delnum;
    sendnum = pack->data.send_fd;
    delnum = atoi(pack->data.mes);
    
    sprintf(buffer,"delete from 朋友 where (fuid = %d and suid = %d) or (suid = %d and fuid = %d)",sendnum,delnum,sendnum,delnum);
    res = mysql_query(&mysql, buffer); //查询语句  
    if (res){
        printf("SELECT error:%s\n",mysql_error(&mysql));         
    }
    else{
        PACK * senddata = NULL;
        senddata = (PACK *)malloc(sizeof(PACK));
        senddata->type = FRI_DEL;
        senddata->data.recv_fd = pack->data.send_fd;
        senddata->data.mes[0] = 'y';
        if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
            my_err("send",__LINE__);
    }

    close_mysql(mysql); 
}

void srv_addfriend(PACK *pack){

    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);
    char buffer[250];
    memset(buffer,0,sizeof(buffer));

    printf("%s\n\n",pack->data.mes);

    sprintf(buffer,"select uid from 用户数据 where `name` = '%s'",pack->data.mes);

    printf("%s\n\n",buffer);
    res = mysql_query(&mysql, buffer); //查询语句  
    if (res){
        printf("SELECT error:%s\n",mysql_error(&mysql));         
    }
    else{
            int fd = -1;
            res_ptr = mysql_store_result(&mysql);             //取出结果集  mysql_store_result()立即检索所有的行，
            while((sqlrow = mysql_fetch_row(res_ptr)))  
            {   //依次取出记录  
                printf("%s\t", sqlrow[0]);
                fd = atoi(sqlrow[0]);              //输出  
                printf("\n");          
            }

            infouser_node_t *pos;
            pos = (infouser_list_t)malloc(sizeof(infouser_node_t));

            strcpy(pack->data.mes,"find fri\0");            

            memset(buffer,0,sizeof(buffer));
            sprintf(buffer,"insert into 个人消息 values(%d,%d,'%s',%d,null)",pack->data.send_fd,fd,pack->data.mes,FRIQUE);
            res = mysql_query(&mysql, buffer); //查询语句  
            //res_ptr = mysql_store_result(&mysql);
            if (res){
                printf("SELECT error:%s\n",mysql_error(&mysql));         
            }
            else{
                    List_ForEach(head,pos){
                    if(pos->data.uid == fd){
                        PACK * senddata = NULL;
                        senddata = (PACK *)malloc(sizeof(PACK));
                        senddata->type = FRIQUE;                
                        senddata->data.recv_fd = pack->data.send_fd;
                        senddata->data.send_fd = fd;
                        strcpy(senddata->data.mes,pack->data.mes); 
                        strcpy(senddata->data.send_name,pack->data.send_name);
                        if(send(pos->data.socket_id,senddata,sizeof(PACK),0) < 0)
                            my_err("send",__LINE__);
                    }
                }
            }
    }


    PACK * senddata = NULL;
    senddata = (PACK *)malloc(sizeof(PACK));
    senddata->type = FRI_ADD;
    senddata->data.recv_fd = pack->data.send_fd;
    senddata->data.mes[0] = 'n';
    if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
        my_err("send",__LINE__);

    close_mysql(mysql); 
}

void srv_groupquit(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);
    char buffer[150];
    memset(buffer,0,sizeof(buffer));
    char sendnum[20],delnum[20];
    memset(sendnum,0,sizeof(sendnum));
    memset(delnum,0,sizeof(delnum));
    strcpy(sendnum,pack->data.send_name);
    strcpy(delnum,pack->data.mes);

    sprintf(buffer,"delete from 群用户 where `guid` = '%s' and suid = '%s'",sendnum,delnum);
    res = mysql_query(&mysql, buffer); //查询语句  

    if (res){
        printf("SELECT error:%s\n",mysql_error(&mysql));         
    }
    else{


        PACK * senddata = NULL;
        senddata = (PACK *)malloc(sizeof(PACK));
        senddata->type = REGISTER;
        senddata->data.recv_fd = pack->data.send_fd;
        senddata->data.mes[0] = 'n';
        if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
            my_err("send",__LINE__);
    }
    close_mysql(mysql); 
}

void srv_groupjoin(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);
    char buffer[150];
    memset(buffer,0,sizeof(buffer));


    PACK * senddata = NULL;
    senddata = (PACK *)malloc(sizeof(PACK));
    senddata->type = GROUP_JOIN;
    senddata->data.recv_fd = pack->data.send_fd;
    senddata->data.mes[0] = 'n';
    if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
        my_err("send",__LINE__);

    close_mysql(mysql); 
}

void srv_findpassword(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);
    char buffer[150];
    memset(buffer,0,sizeof(buffer));
    sprintf(buffer,"select password from 用户数据 where `mibao` = '%s'",pack->data.mes);
    res = mysql_query(&mysql, buffer); //查询语句 
    if (res){
        printf("SELECT error:%s\n",mysql_error(&mysql));         
    }
    else{
        res_ptr = mysql_store_result(&mysql);             //取出结果集  mysql_store_result()立即检索所有的行，
        if(mysql_num_rows(res_ptr) == 0){
            PACK * senddata = NULL;
            senddata = (PACK *)malloc(sizeof(PACK));
            senddata->type = FIND_PASSWD;
            senddata->data.recv_fd = pack->data.send_fd;
            senddata->data.mes[0] = 'n';
            if(send(pack->data.send_fd,senddata,sizeof(PACK),0) < 0)
                my_err("send",__LINE__);
        }
        else if(mysql_num_rows(res_ptr)!=0){
            PACK * senddata = NULL;
            senddata = (PACK *)malloc(sizeof(PACK));
            senddata->type = FIND_PASSWD;
            senddata->data.recv_fd = pack->data.send_fd;
            senddata->data.mes[0] = 'y';
            if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
                my_err("send",__LINE__);    
        }
    }   
    close_mysql(mysql); 
}


void srv_groupchat(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);
    char buffer[150];
    memset(buffer,0,sizeof(buffer));


    PACK * senddata = NULL;
    senddata = (PACK *)malloc(sizeof(PACK));
    senddata->type = CHAT_MANY;
    senddata->data.recv_fd = pack->data.send_fd;
    senddata->data.mes[0] = 'n';
    if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
        my_err("send",__LINE__);

    close_mysql(mysql); 
}

void srv_groupkick(PACK *pack){
    
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);
    char buffer[150];
    memset(buffer,0,sizeof(buffer));

    char sendnum[20],delnum[20];
    memset(sendnum,0,sizeof(sendnum));
    memset(delnum,0,sizeof(delnum));
    strcpy(sendnum,pack->data.recv_name);
    strcpy(delnum,pack->data.mes);

    sprintf(buffer,"delete from 群用户 where guid = '%s' and suid = '%s'",sendnum,delnum);

    res = mysql_query(&mysql, buffer); //查询语句 
    if (res){
        printf("SELECT error:%s\n",mysql_error(&mysql));         
    }
    else{
        PACK * senddata = NULL;
        senddata = (PACK *)malloc(sizeof(PACK));
        senddata->type = CHAT_MANY;
        senddata->data.recv_fd = pack->data.send_fd;
        senddata->data.mes[0] = 'y';
        if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
            my_err("send",__LINE__);
    }

    PACK * senddata = NULL;
    senddata = (PACK *)malloc(sizeof(PACK));
    senddata->type = GROUP_KICK;
    senddata->data.recv_fd = pack->data.send_fd;
    senddata->data.mes[0] = 'n';
    if(send(pack->data.send_fd,senddata,sizeof(PACK),0) < 0)
        my_err("send",__LINE__);

    close_mysql(mysql); 
}

void srv_frienchat(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);
    char buffer[150];
    memset(buffer,0,sizeof(buffer));

    PACK * senddata = NULL;
    senddata = (PACK *)malloc(sizeof(PACK));
    senddata->type = CHAT_ONE;
    senddata->data.recv_fd = pack->data.send_fd;
    senddata->data.mes[0] = 'n';
    if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
        my_err("send",__LINE__);

    close_mysql(mysql); 
}


void srv_groupsee(PACK *pack){

    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);
    char buffer[150];
    memset(buffer,0,sizeof(buffer));


    sprintf(buffer,"select suid from 群用户 where `suid` = %d",pack->data.send_fd);
    
    res = mysql_query(&mysql, buffer); //查询语句  
    if (res){         
        printf("SELECT error:%s\n",mysql_error(&mysql));     
    }
    else{
        res_ptr = mysql_store_result(&mysql);             //取出结果集  mysql_store_result()立即检索所有的行
        j = mysql_num_fields(res_ptr);//获取 列数    
        while((sqlrow = mysql_fetch_row(res_ptr))){   //依次取出记录

            PACK * senddata = NULL;
            senddata = (PACK *)malloc(sizeof(PACK));  
            senddata->type = GROUP_SEE;
            //senddata->data.recv_fd = pack->data.send_fd;
            strcpy(senddata->data.send_name,"server");

            for(i = 0; i < j; i++){
                printf("%s\t", sqlrow[i]);            
                strcat(senddata->data.mes,sqlrow[i]);
            }
            printf("\n");

            if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
                my_err("send",__LINE__);          
        }
        if (mysql_errno(&mysql)){                      
            fprintf(stderr,"Retrive error:s\n",mysql_error(&mysql));               
        }              
        mysql_free_result(res_ptr);        //释放空间    
    }
    close_mysql(mysql);  
    return ;  

}


void srv_friendsee(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    char buffer[150];
    memset(buffer,0,sizeof(buffer));


    sprintf(buffer,"select suid from 朋友 where `fuid` = %d",pack->data.send_fd);
    res = mysql_query(&mysql, buffer); //查询语句  
    if (res){         
        printf("SELECT error:%s\n",mysql_error(&mysql));     
    }
    else{
        res_ptr = mysql_store_result(&mysql);             //取出结果集  mysql_store_result()立即检索所有的行
        j = mysql_num_fields(res_ptr);//获取 列数    
        while((sqlrow = mysql_fetch_row(res_ptr)))  
        {   //依次取出记录

            PACK * senddata = NULL;
            senddata = (PACK *)malloc(sizeof(PACK));  
            senddata->type = FRI_SEE;
            //senddata->data.recv_fd = pack->data.send_fd;
            strcpy(senddata->data.send_name,"server");

            for(i = 0; i < j; i++){
                printf("%s\t", sqlrow[i]);
                infouser_node_t * pos = NULL;
                pos = (infouser_list_t)malloc(sizeof(infogroup_node_t));

                List_ForEach(head,pos){
                    if(pos->data.uid = atoi(sqlrow[i])){
                        senddata->data.send_fd = 1;
                    }
                }
                senddata->data.recv_fd = atoi(sqlrow[i]);
            }
            printf("\n");
            if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
                my_err("send",__LINE__);  
        }
    }
    close_mysql(mysql); 
}

void srv_groupdel(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);
    char buffer[150];
    memset(buffer,0,sizeof(buffer));
    sprintf(buffer,"delete from 群数据 where `name` = '%s'",pack->data.mes);
    res = mysql_query(&mysql, buffer); //查询语句 
    if (res){
        printf("SELECT error:%s\n",mysql_error(&mysql));         
    }
    else{

        PACK * senddata = NULL;
        senddata = (PACK *)malloc(sizeof(PACK));
        senddata->type = GROUP_DEL;
        senddata->data.recv_fd = pack->data.send_fd;
        senddata->data.mes[0] = 'y';
        if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
            my_err("send",__LINE__);

    }  
    close_mysql(mysql); 
}

void srv_creategroup(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    char buffer[150];
    memset(buffer,0,sizeof(buffer));
    sprintf(buffer,"insert into 群数据(name,boss)values('%s',%d)",pack->data.mes,pack->data.send_fd);
    res = mysql_query(&mysql, buffer); //查询语句 
    if (res){
        printf("SELECT error:%s\n",mysql_error(&mysql));         
    }
    else{
        int guid;
        memset(buffer,0,sizeof(buffer));
        sprintf(buffer,"insert into 群用户(guid,suid,power)values('%d','%s',%d)",guid,pack->data.mes,pack->data.send_fd);
        PACK * senddata = NULL;
        senddata = (PACK *)malloc(sizeof(PACK));
        senddata->type = GROUP_CREATE;
        senddata->data.recv_fd = pack->data.send_fd;
        senddata->data.mes[0] = 'y';
        if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
            my_err("send",__LINE__);
    }
    close_mysql(mysql); 
}


void srv_changenum(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);

    char buffer[150];
    memset(buffer,0,sizeof(buffer));
    sprintf(buffer,"select password from 用户数据 where `mibao` = '%s'",pack->data.mes);
    res = mysql_query(&mysql, buffer); //查询语句 
    if (res){
        printf("SELECT error:%s\n",mysql_error(&mysql));         
    }
    else{

        PACK * senddata = NULL;
        senddata = (PACK *)malloc(sizeof(PACK));
        senddata->type = CHANGE_NUM;
        senddata->data.recv_fd = pack->data.send_fd;
        senddata->data.mes[0] = 'y';
        if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
            my_err("send",__LINE__);



    char buffer[150];
    memset(buffer,0,sizeof(buffer));
    sprintf(buffer,"select password from 用户数据 where `mibao` = '%s'",pack->data.mes);

    }

    close_mysql(mysql); 
}


void judgeaddfri(PACK *pack){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);


    char buffer[150];
    memset(buffer,0,sizeof(buffer));

    int fuid,suid;
    sprintf(buffer,"select fuid,suid from 个人消息 where uid = '%d'",pack->data.send_fd);
    res = mysql_query(&mysql, buffer); //查询语句 

    if(pack->data.mes[0] == 'y'){
        if(res){
            printf("SELECT error:%s\n",mysql_error(&mysql)); 
        }
        else{

            res_ptr = mysql_store_result(&mysql);  //取出结果集  mysql_store_result()立即检索所有的行
            j = mysql_num_fields(res_ptr);//获取 列数 
            while((sqlrow = mysql_fetch_row(res_ptr)))  
            {   //依次取出记录

                /*PACK * senddata = NULL;
                senddata = (PACK *)malloc(sizeof(PACK));  
                senddata->type = FRI_MES;
                senddata->data.recv_fd = pack->data.send_fd;
                strcpy(senddata->data.send_name,"server");*/
                
                for(i = 0; i < j; i++){
                    printf("%s\t", sqlrow[i]);
                    if(i == 0 ){
                        fuid = atoi(sqlrow[i]);
                    }
                    else if(i == 1){
                        suid = atoi(sqlrow[i]);
                    }
                }
                printf("\n");
                /*if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
                    my_err("send",__LINE__);*/  
            }
             memset(buffer,0,sizeof(buffer));
            sprintf(buffer,"insert into 朋友 (fuid,suid)values(%d,%d)",fuid,suid);
            res = mysql_query(&mysql, buffer); //查询语句
            if(res){
                printf("SELECT error:%s\n",mysql_error(&mysql)); 
            }
            else{
                memset(buffer,0,sizeof(buffer));
                sprintf(buffer,"delete from 个人消息 where uid = %d",pack->data.send_fd);
                res = mysql_query(&mysql, buffer); //查询语句
                if(res){
                    printf("SELECT error:%s\n",mysql_error(&mysql));
                }
                else{
                    printf("添加成功！！");
                }
            }
        } 
    }
    else{

        if(res){
            printf("SELECT error:%s\n",mysql_error(&mysql)); 
        }
        else{
            memset(buffer,0,sizeof(buffer));
            sprintf(buffer,"delete from 个人消息 where uid = %d",pack->data.send_fd);
            res = mysql_query(&mysql, buffer); //查询语句
            if(res){
                printf("SELECT error:%s\n",mysql_error(&mysql));
            }
            else{
                printf("已拒绝好友添加！");
            }
        } 

    }


    close_mysql(mysql); 
}
void judgeaddgro(PACK *pack){

    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  
    mysqlinit(&mysql);




    char buffer[150];
    memset(buffer,0,sizeof(buffer));

    int fuid,suid;
    sprintf(buffer,"select fuid,suid from 个人消息 where uid = '%d'",pack->data.send_fd);
    res = mysql_query(&mysql, buffer); //查询语句 

    if(pack->data.mes[0] == 'y'){
        if(res){
            printf("SELECT error:%s\n",mysql_error(&mysql)); 
        }
        else{

            res_ptr = mysql_store_result(&mysql);  //取出结果集  mysql_store_result()立即检索所有的行
            j = mysql_num_fields(res_ptr);//获取 列数 
            while((sqlrow = mysql_fetch_row(res_ptr)))  
            {   //依次取出记录

                /*PACK * senddata = NULL;
                senddata = (PACK *)malloc(sizeof(PACK));  
                senddata->type = FRI_MES;
                senddata->data.recv_fd = pack->data.send_fd;
                strcpy(senddata->data.send_name,"server");*/
                
                for(i = 0; i < j; i++){
                    printf("%s\t", sqlrow[i]);
                    if(i == 0 ){
                        fuid = atoi(sqlrow[i]);
                    }
                    else if(i == 1){
                        suid = atoi(sqlrow[i]);
                    }
                }
                printf("\n");
                /*if(send(pack->data.recv_fd,senddata,sizeof(PACK),0) < 0)
                    my_err("send",__LINE__);*/  
            }
             memset(buffer,0,sizeof(buffer));
            sprintf(buffer,"insert into 群用户 (fuid,suid)values(%d,%d)",fuid,suid);
            res = mysql_query(&mysql, buffer); //查询语句
            if(res){
                printf("SELECT error:%s\n",mysql_error(&mysql)); 
            }
            else{
                memset(buffer,0,sizeof(buffer));
                sprintf(buffer,"delete from 个人消息 where uid = %d",pack->data.send_fd);
                res = mysql_query(&mysql, buffer); //查询语句
                if(res){
                    printf("SELECT error:%s\n",mysql_error(&mysql));
                }
                else{
                    printf("添加成功！！");
                }
            }
        } 
    }
    else{

        if(res){
            printf("SELECT error:%s\n",mysql_error(&mysql)); 
        }
        else{
            memset(buffer,0,sizeof(buffer));
            sprintf(buffer,"delete from 个人消息 where uid = %d",pack->data.send_fd);
            res = mysql_query(&mysql, buffer); //查询语句
            if(res){
                printf("SELECT error:%s\n",mysql_error(&mysql));
            }
            else{
                printf("已拒绝好友添加！");
            }
        } 

    }

    close_mysql(mysql); 
}


int close_mysql(MYSQL mysql){
	mysql_close(&mysql);
	mysql_library_end();
	printf("end\n");
	return 0;
}