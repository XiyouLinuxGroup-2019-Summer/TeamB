#include<stdlib.h>
#include<stdio.h>
#include<mysql/mysql.h>

MYSQL *conn_ptr;
unsigned int timeout = 7;   //超时时间7秒
int main(){
    //初始化
    int ret = 0;
    //错误返回null
    conn_ptr = mysql_init(NULL);
    
    if(!conn_ptr){
        printf("mysql_init failed! line %d\n",__LINE__);
        return -1;
    }

    //设置超时选项，正常返回0
    ret = mysql_options(conn_ptr,MYSQL_OPT_CONNECT_TIMEOUT,(const char*)&timeout);
    if(ret){
        printf("Options Set ERRO!\n");
    }
    
    //连接MySQL testdb数据库,正常返回指针
    conn_ptr = mysql_real_connect(conn_ptr,"localhost","root","1245215743","study",0,NULL,0);
    
    if(conn_ptr){
        printf("Connection Succeed!\n");

        //执行sql语句,在数据库表中添加内容，正常返回0
        //int mysql_real_query(MYSQL *mysql, const char *q, unsigned long length);
        ret = mysql_query(conn_ptr,"insert into children(fname,age)values('ann',3)");
        /*变量插入示例：
        int id =11;
        char name[10]="哈哈";
        char sex[3] = "女";
        char password[15]="1234456";
        char str[200];
        sprintf(str,"insert into User values(%d,'%s','%s','%s',%d)",id,name,sex,password,0);
        rc = mysql_real_query(&mysql,str,strlen(str));*/
        
            if(!ret){
                //返回上一次update更改行数
                printf("inserted %lu rows\n",(unsigned long)mysql_affected_rows(conn_ptr));
            }else{
                //返回错误代码，错误消息
                printf("Connect Error: %d %s\n",mysql_errno(conn_ptr),mysql_error(conn_ptr));
            }

        //关闭连接
        mysql_close(conn_ptr);
        printf("Connection closed!\n");
    }else{    //错误处理
        printf("Connection Failed!\n");
        
        if(mysql_errno(conn_ptr)){
            printf("Connect Erro:%d %s\n",mysql_errno(conn_ptr),mysql_error(conn_ptr));//返回错误代码、错误消息
        }
        
        return -2;
    }

    return 0;
}
