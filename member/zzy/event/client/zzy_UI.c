#include"zzy_UI.h"
//欢迎登陆界面
int welcome_login_IU(void){
    printf("Welcome to use zzychat!\n");
    printf("please input your choice!\n");
    printf("[1]注册 [2]登录 [3]找回密码 [4]退出\n");
    printf("your choice:");
    char c;
    while((c = getchar()) != '\n'){
        getchar();
        if(c == '1'){
            return 1;
        }else if(c == '2'){
            return 2;
        }else if(c == '3'){
            return 3;
        }else if(c == '4'){
            return 4;
        }
        else{
            printf("input wrong!\n");
            printf("please input again:");
            continue;
        }
    }                                                                                                                                                    
    return 0;
}


//登陆后选项界面

