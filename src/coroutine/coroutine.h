#ifndef TW_COROUTINE_IMPL_COROUTINE_H
#define TW_COROUTINE_IMPL_COROUTINE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "copre.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
extern "C" {

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

struct __tw_co_scheduler_dummy_t{
    signed int dummy;
} const* co_scheduler_ref_t

// get scheduler
#define coroutine_scheduler(coroutine)           ((coroutine)->scheduler)

// is original?
#define coroutine_is_original(coroutine)         ((coroutine)->scheduler == (co_scheduler_ref_t)(coroutine))

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

typedef void       (*coroutine_func_t)(void const* priv);

// the coroutine function type
typedef struct __tw_coroutine_rs_func_t
{
    // the function 
    coroutine_func_t             func;

    // the user private data as the argument of function
    void const*                   priv;

}coroutine_rs_func_t;

// the coroutine wait type
typedef struct __tw_coroutine_rs_wait_t
{
    /* the timer task pointer for ltimer or timer
     *
     * for ltimer:  task
     * for timer:   task & 0x1
     */
    void const*                   task;

    // the waiting events
    unsigned short                     events          : 6;

    // the cached events
    unsigned short                     events_cache    : 6;

    // is waiting?
    unsigned short                     waiting         : 1;

}tb_coroutine_rs_wait_t;

// the coroutine type
typedef struct __tb_coroutine_t
{
    /* the list entry for ready, suspend and dead lists
     *
     * be placed in the head for optimization
     */
    //tb_list_entry_t                 entry;

    // the scheduler
    co_scheduler_ref_t              scheduler;

    // the context 
    context_ref_t                   context;

    // the stack base (top)
    unsigned char*                  stackbase;

    // the stack size
    unsigned long long              stacksize;

    // the passed user private data between priv = resume(priv) and priv = suspend(priv)
    void const*                     rs_priv;

    // the passed private data between resume() and suspend()
    union 
    {
        // the function
        coroutine_rs_func_t      func;

        // the arguments for wait()
        coroutine_rs_wait_t      wait;

        // the single entry
        //single_list_entry_t      single_entry;

    }                               rs;

    // the guard
    unsigned short                     guard;

}coroutine_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* init coroutine 
 *
 * @param scheduler     the scheduler
 * @param func          the coroutine function
 * @param priv          the passed user private data as the argument of function
 * @param stacksize     the stack size, uses the default stack size if be zero
 *
 * @return              the coroutine 
 */
coroutine_t*         coroutine_init(co_scheduler_ref_t scheduler, coroutine_func_t func, void const* priv, unsigned long long stacksize);

/* reinit the given coroutine 
 *
 * @param coroutine     the coroutine
 * @param func          the coroutine function
 * @param priv          the passed user private data as the argument of function
 * @param stacksize     the stack size, uses the default stack size if be zero
 *
 * @return              the coroutine
 */
coroutine_t*         tb_coroutine_reinit(coroutine_t* coroutine, coroutine_func_t func, void const* priv, unsigned long long stacksize);

/* exit coroutine
 *
 * @param coroutine     the coroutine
 */
void               tb_coroutine_exit(coroutine_t* coroutine);


/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
}

#endif