#include "sch_algorithm.h"

typedef struct node  
{  
    int elem;                // this node's id
    struct node *pNext;      // next node
    struct node *pPrev;      // previous node   
}Node,*pNode; 

int algo = -1;   //algorithm type
Node* pHead, pTail; 
bool coflag[100] = {0}; //flag of the coroutines

int rr(struct schedule *S) {    //implement of Round-Robin algorithm
    if (S->nco == 1) { //initialize when only 1 element active
        pHead->elem = S->running;
        coflag[S->running] = true;
        pHead->pNext = pHead;
        pHead->pPrev = pHead;
    } else {
        for (id = 0; id < S->nco; id++) {
            if (S->co[id]->status == -1) {  //delete the inactive link node
                Node *tpNode = pHead;
                int mxit = S->nco;
                while (tpNode->pNext->elem != id && mxit--) {
                    tpNode = tpNode->pNext;
                }
                if (mxit) {
                    coflag[tpNode->pNext->elem] = false;
                    tpNode->pNext = tpNode->pNext->pNext;
                    tpNode = tpNode->pNext;
                    tpNode->pPrev = tpNode->pPrev->pPrev;
                }
            }
        }
        if (coflag[S->running] == false) {  //add new active coroutine
            coflag[S->running] = true;
            Node *tpNode = pHead;
            tpNode->pNext = pHead->pNext;
            pHead->pNext = tpNode;
            tpNode->pPrev = pHead;
            pHead = tpNode->pNext;
            pHead->pPrev = tpNode;
        } else {
            pHead = pHead->pNext;   //select next coroutine
        }

    }
    return pHead->elem;
}

int sch_choose(struct schedule *S) {
    if (algo == RR) {
        return rr(S);
    }                                                                         
    return -1;
}

void sch_init(struct schedule *S, int type) {
    algo = type;
    pHead = (pNode)malloc(sizeof(Node)); 
    pTail = (pNode)malloc(sizeof(Node));
    pHead->pNext = pTail;
    pTail->pPrev = pHead;                                                                    
    return;
}