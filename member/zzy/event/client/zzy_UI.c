#include"zzy_UI.h"
//欢迎登陆界面
int welcome_login_UI(void){
    printf("Welcome to use zzychat!\n");
    printf("please input your choice!\n");
    printf("[1]注册 [2]登录 [3]找回密码 [4]退出\n");
    printf("your choice:");
    char c;
    while((c = getchar()) != '\n'){
        /* getchar(); */
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


//用户登陆后选择界面
int user_menu_UI(){
    printf("\n----------用户选择界面-----------\n");
    printf("[1]查看好友\n[2]私聊好友\n[3]添加好友\n");
    printf("[4]删除好友\n[5]查看群聊\n");
    printf("[6]创建群聊\n[7]加入群聊\n");
    printf("[8]退出群聊\n[9]解散群聊\n[0]退出");
    printf("请输入你的选择:");
    
    char c;
    while((c = getchar()) != '\n'){
          /* getchar(); */                                                  
          if(c == '1'){
              return 1;
          }else if(c == '2'){
              return 2;
          }else if(c == '3'){
              return 3;
          }else if(c == '4'){
              return 4;
          }else if(c == '5'){
              return 5;
          }else if(c == '6'){
              return 6;
          }else if(c == '7'){
              return 7;
          }else if(c == '8'){
              return 8;
          }else if(c == '9'){
              return 9;
          }
          else{
              printf("input wrong!\n");
              printf("please input again:");
              continue;
          }   
      }
      return 0;
}
