#include "my_serthread.h"
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include "my_recv.h"
#include <stdlib.h>
#include <mysql/mysql.h>

int threadlogin();
void mysqlinit(MYSQL *mysql);
void *myallthread(PACK *pack){

    switch (pack->type)
    {
        case LOGIN:
            threadlogin();
            break;
        case FRI_ADD:
            /* code */
            break;
        case FRI_DEL:
            
            break;

        case FRI_CHA:
            
            break;

        case GROUP_CREATE:

            break;

        case GROUP_JOIN:
            break;

        case GROUP_QIUT:
            break;

        case GROUP_DEL:
            break;

        case GROUP_SEE:
            break;

        case REGISTER:
            break;

        case CHAT_ONE:
            break;

        case CHAT_MANY:
            break;

        case GROUP_KICK:
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


int threadlogin(){
    MYSQL mysql;
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;  
    int res, i, j;  

    mysqlinit(&mysql);
    if (1) 
    {  
        res = mysql_query(&mysql, "select password from 用户数据 where `name` = pack"); //查询语句  
        if (res) 
        {         
            printf("SELECT error:%s\n",mysql_error(&mysql));     
        } else 
        {        
            res_ptr = mysql_store_result(&mysql);             //取出结果集  mysql_store_result()立即检索所有的行，
            if(res_ptr) 
            {   
                //	int num = (unsigned long)mysql_num_rows(res_ptr);
                printf("%lu Rows\n",(unsigned long)mysql_num_rows(res_ptr));   //返回所有的行
                j = mysql_num_fields(res_ptr);//获取 列数    
                while((sqlrow = mysql_fetch_row(res_ptr)))  
                {   //依次取出记录  
                    for(i = 0; i < j; i++)
                        printf("%s\t", sqlrow[i]);              //输出  
                    printf("\n");          

                    
                }              
                if (mysql_errno(&mysql)) 
                {                      
                    fprintf(stderr,"Retrive error:s\n",mysql_error(&mysql));               
                }        
            }        
            mysql_free_result(res_ptr);        //释放空间
        }  
    } else 
    {  
        printf("Connection failed\n");  
    }  
    mysql_close(&mysql);  
    return EXIT_SUCCESS;  
}