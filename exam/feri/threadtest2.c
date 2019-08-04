 #include <stdio.h>
 #include <stdlib.h>
 #include <memory.h>
 #include <pthread.h>
 #include <errno.h>
 #include <unistd.h>
 #include <math.h>
 pthread_mutex_t chopstick[6] ;//将每一根筷子看做一个临界区，为每一根筷子设置互斥量
 void get(int *left, int *right, char phi) {
 switch (phi){
 case 'A'://给筷子和哲学家们编号，每个哲学家的左右手对应的筷子号已知了
        *left = 5;
        *right = 1;
        break;
 case 'B':
        *left = 1;
        *right = 2;
        break;
 case 'C':
        *left = 2;
        *right = 3;
        break;
 case 'D':
        *left = 3;
        *right = 4;
        break;
 case 'E':
        *left = 4;
        *right = 5;
        break;
 }
 }
 void *eat_think(void *arg)
 {
        char phi = *(char *)arg;//哲学家的序号
        int left,right;
        get(&left,&right,phi);//已知当前哲学家序号和它左右手对应的筷子序号
        for(;;){
                usleep(10);//线程挂起10
                pthread_mutex_lock(&chopstick[left]);//该哲学家将左手的筷子拿起来，意味着其他的人将不能使用
                //这根筷子，所以将这根筷子，也就是这个临界区加锁保护起来，以保证其他的人拿不到这根筷子，也
                //就是当一个线程对共享变量访问时，保证了其他的线程不能访问该变量，达到互斥的目的。
                printf("Philosopher %c fetches chopstick %d\n", phi,left);
                if (pthread_mutex_trylock(&chopstick[right]) ==EBUSY){//trylock和lock的不同就是trylock回去试探一下
                    //某一个共享变量是否处于解锁状态，如果是则和lock行为一样，否则立即失败返回，而lock就会阻塞住
                    //哲学家去摸摸右手边有没有筷子，有的话吃饭，没有的话放下左手的筷子，继续学习
                        pthread_mutex_unlock(&chopstick[left]);//放下左手的筷子
                        continue;
                }
                printf("Philosopher %c fetches chopstick %d\n", phi,right);
                printf("Philosopher %c is eating.\n",phi);
                usleep(10);//吃饭
                pthread_mutex_unlock(&chopstick[left]);//放下左手的筷子，解锁对该临界区的保护，其他等待
                //线程可以访问该共享变量了
                printf("Philosopher %c release chopstick %d\n", phi,left);
                pthread_mutex_unlock(&chopstick[right]);//同理
                printf("Philosopher %c release chopstick %d\n", phi,right);
        }
 }
 int main(){
        pthread_t A,B,C,D,E;//五个哲学家
        int i;
        for (i = 0; i < 5; i++)
                pthread_mutex_init(&chopstick[i],NULL);//动态分配互斥量，但由于该互斥量的属性采取的是
        //默认属性，也可以直接静态分配
        pthread_create(&A,NULL, eat_think, (void *)"A");//为每一个哲学家创建线程调用同一个思考还是吃饭的
        //函数去判断这个哲学家是能吃还是要思考
        pthread_create(&B,NULL, eat_think, (void *)"B");
        pthread_create(&C,NULL, eat_think, (void *)"C");
        pthread_create(&D,NULL, eat_think, (void *)"D");
        pthread_create(&E,NULL, eat_think, (void *)"E");
        pthread_join(A, NULL);//链接第一个参数对应的每个线程，但由于第二个参数设置的是NULL，其实连接没什么
        //意义，可以直接分离
        pthread_join(B, NULL);
        pthread_join(C, NULL);
        pthread_join(D, NULL);
        pthread_join(E, NULL);
        return 0;
}
