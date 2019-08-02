#include "../src/coroutine.h"
#include "../src/predef.h"
#include <stdio.h>

struct args {
	int n;
};

#ifdef __ELF__
#include <sys/time.h>
#include <sys/select.h>
#include <time.h>

bool timerSwitch = 1;  //0: not preemptive, 1: preemptive
int tsec = 1, tmsec = 0;

void setTimer(int seconds, int mseconds, struct schedule * S)	//timer for preemptive scheduling
{
        struct timeval temp;

        temp.tv_sec = seconds;
        temp.tv_usec = mseconds;

        select(0, NULL, NULL, NULL, &temp);
        printf("timer up\n");
		coroutine_yield(S);
        return ;
}

#endif

static void foo(struct schedule * S, void *ud) {	//coroutine task function
	struct args * arg = ud;
	int start = arg->n;
	int i;
#ifdef __ELF__
	setTimer(1, 0, S);
#endif
	for (i=0;i<10;i++) {
		printf("coroutine %d : %d\n",coroutine_running(S) , start + i);
		coroutine_yield(S);
	}
	for (i=0;i<1000000;i++) {
                //printf("coroutine %d : %d\n",coroutine_running(S) , start + i);
                coroutine_yield(S);
        }

}

static void test(struct schedule *S) {	//coroutine scheduling function, can be circled list for round-robin
	struct args arg1 = { 0 };
	struct args arg2 = { 100 };

	int co1 = coroutine_new(S, foo, &arg1);
	int co2 = coroutine_new(S, foo, &arg2);
	printf("main start for 1000000 times switch\n");
	while (coroutine_status(S,co1) && coroutine_status(S,co2)) {
		coroutine_resume(S,co1);
		coroutine_resume(S,co2);
	} 
	printf("main end\n");
}

int main() {
	struct schedule * S = coroutine_open();
	test(S);
	coroutine_close(S);
	
	return 0;
}

