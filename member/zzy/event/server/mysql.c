#include"mysql.h"
#include"my_recv.h"

MYSQL_RES   *res = NULL;
MYSQL_ROW   row;
MYSQL       mysql;
char        query_etr[MAX_SIZE*4];
int         rc,fields;
int         rows;

//连接，初始化数据库
void mysql_init_t()
{
    if (NULL == mysql_init(&mysql)) {
        my_err("mysql_init error",__LINE__);
        exit(-1);
    }
    if (!mysql_real_connect(&mysql, "localhost", "root", "1245215743", "userinfon", 0, NULL, 0)) {
        my_err("mysql_real_connect error",__LINE__);
        exit(-1);
    }
    mysql_set_character_set(&mysql, "utf8");
    printf("mysql connect success!\n");

}

//关闭mysql
void mysql_close(){
    mysql_free_result(res);
    mysql_close(&mysql);
}

