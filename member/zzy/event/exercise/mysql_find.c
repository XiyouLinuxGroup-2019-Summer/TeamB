#include<stdio.h>
#include<stdlib.h>
#include<mysql/mysql.h>

MYSQL *conn_ptr;
MYSQL_RES *res_ptr;
MYSQL_ROW sqlrow;

unsigned int timeout = 7;

//打印信息函数
void display_row(MYSQL *ptr){
    unsigned int field_count = 0;
    while(field_count < mysql_field_count(ptr)){
            printf("%s",sqlrow[field_count]);
            field_count++;
    }
    printf("\n");
}


int main(){
    //初始化
    int ret = 0;
    conn_ptr = mysql_init(NULL);
    if(!conn_ptr){
        printf("mysql_init failde\n");
        return -1;
    }
    
    //设置超时选项
    if(mysql_options(conn_ptr,MYSQL_OPT_CONNECT_TIMEOUT,(const char*)&timeout))
        printf("Options Set ERROR\n");

    //连接MYSQL库
    conn_ptr = mysql_real_connect(conn_ptr,"localhost","root","1245215743","study",0,NULL,0);

    if(conn_ptr){
        printf("Connect Succeed!\n");
        
        //执行mysql语句，查找年龄大于五的孩子,成功返回0
        ret = mysql_query(conn_ptr,"SELECT childno,fname,age FROM children WHERE age>5");
        
        if(!ret){
            //获取检索得到的数据,成功返回非NULL
            res_ptr = mysql_store_result(conn_ptr);
            if(res_ptr){
                //在结果集合中返回一共多少行
                printf("Retrieved %lu rows\n",(unsigned long)mysql_num_rows(res_ptr));

                //返回store_result中得到的结构体，并从中检索单行
                while((sqlrow = mysql_fetch_row(res_ptr))){
                    printf("Fetched data\n");
                    
                    //打印信息
                    display_row(conn_ptr);
                }
            }

            if(mysql_errno(conn_ptr)){
                //返回错误代码和消息
                printf("Connect Error %d %s",mysql_errno(conn_ptr),mysql_error(conn_ptr));
                return -2;
            }

            mysql_free_result(res_ptr);
        
        }else{
            //返回错误代码，错误消息
            printf("Connect Erro:%d %s\n",mysql_errno(conn_ptr),mysql_error(conn_ptr));
            return -3;
        }

        mysql_close(conn_ptr);
        printf("Connect closed!\n");
    }else{  //错误处理
        printf("Connect Failed!\n");

        if(mysql_errno(conn_ptr)){
            //返回错误代码，错误消息
            printf("Connect Erro:%d %s\n",mysql_errno(conn_ptr),mysql_error(conn_ptr));
        }
        return -2;
    }

    return 0;
}
