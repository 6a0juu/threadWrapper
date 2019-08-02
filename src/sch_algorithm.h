#ifndef _SCH_ALGORITHM_
#define _SCH_ALGORITHM_

#define RR 1 	//Round-Robin
#define FCFS 2	//First Come First Serve
#define MFQ 3	//Multi-level Feedback Queue

#include "coroutine.h"

void sch_init(struct schedule *S，int type);
int sch_choose(struct schedule *S);

#endif