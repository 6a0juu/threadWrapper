#include "../src/coroutine.h"
#include <stdio.h>

struct args {
	int n;
};

#ifdef __ELF__

#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
int timerSwitch = 1; //0: not preemptive, 1: preemptive
struct schedule * totS;
void sig_handler (int sig) {
    if (sig == SIGALRM) {
        printf("%d\n",sig);
		coroutine_yield(totS); //yield to scheduler
        alarm (1);  //restart timer
    }
}

#endif

static void foo(struct schedule * S, void *ud) {	//coroutine task function
	struct args * arg = ud;
	int start = arg->n;
	int i, j;
#ifdef __ELF__
	if (timerSwitch){
		struct sigaction sa;
    	memset(&sa, 0, sizeof(sa));
    	sa.sa_handler = sig_handler;
    	sa.sa_flags |= SA_RESTART;
		assert (sigaction (SIGALRM, &sa, NULL) != -1);
		totS = S;
		alarm(1);
	}
#endif
	for (i = 0; i < 5; i++) {
		int k = 0;
		printf("coroutine %d : %d\n",coroutine_running(S) , start + i);
		for (j = 0; j < 500000000; j++) {
			if (j % 2 == 1) {
				k++;
			} else {
				k--;
			}
		}	//delay for more than 1sec
#ifdef __ELF__
		if (timerSwitch){
			coroutine_yield(totS);
		}
#endif
	}

}

static void test(struct schedule *S) {	//coroutine scheduling function
	struct args arg1 = { 0 };
	struct args arg2 = { 100 };

	int co1 = coroutine_new(S, foo, &arg1);
	int co2 = coroutine_new(S, foo, &arg2);
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

