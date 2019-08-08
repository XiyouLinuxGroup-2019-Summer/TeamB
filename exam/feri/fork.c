#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_CHILD_NUMBER 10
#define SLEEP_INTERVAL 2

int proc_number=0;

void do_something();

int main(int argc, char* argv[])
{
        int staloc;
	    int child_proc_number = MAX_CHILD_NUMBER;
	    int i, ch;
	    pid_t  child_pid;
	    pid_t pid[10] = {0};
	    if (argc > 1) {
                child_proc_number = atoi(argv[1]);
                child_proc_number = (child_proc_number > 10) ? 10 :child_proc_number;
        }
        for (i=0; i < child_proc_number; i++) {
                child_pid = fork();
                if (child_pid < 0) {
                        printf("!!!!!!!\n");
                } else if (child_pid == 0) {
                        proc_number = i;
                        do_something();
                } else {
                        pid[i] = child_pid;
                }
        }
	    while ((ch = getchar()) != 'q') {
                if (isdigit(ch)) {
                    kill(pid[ch-'0'], SIGTERM);
                }
        }
        kill(0,SIGTERM);
        wait(&staloc);
	    return 0;
}

void do_something()
{
        for(;;) {
                printf("This is process No.%d and its pid is %d\n", proc_number, getpid());
                sleep(SLEEP_INTERVAL);
        }
}
