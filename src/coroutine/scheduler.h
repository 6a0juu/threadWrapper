#ifndef TW_COROUTINE_IMPL_SCHEDULER_H
#define TW_COROUTINE_IMPL_SCHEDULER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "copre.h"
#include "coroutine.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
extern "C" {

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// get the running coroutine
#define co_scheduler_running(scheduler)             ((scheduler)->running)

// get the ready coroutines count
#define co_scheduler_ready_count(scheduler)         list_entry_size(&(scheduler)->coroutines_ready)

// get the suspended coroutines count
#define co_scheduler_suspend_count(scheduler)       list_entry_size(&(scheduler)->coroutines_suspend)

// get the io scheduler
#define co_scheduler_io(scheduler)                  ((scheduler)->scheduler_io)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the io scheduler type
//struct __co_scheduler_io_t;

// the scheduler type
typedef struct __tw_co_scheduler_t
{   
    /* the original coroutine (in main loop)
     *
     * coroutine->scheduler == (tb_co_scheduler_ref_t)coroutine
     */
    coroutine_t                  original;

    // is stopped
    bool                       stopped;

    // the running coroutine
    coroutine_t*                 running;

    // the io scheduler
    //struct __co_scheduler_io_t*  scheduler_io;

    // the dead coroutines
    //tb_list_entry_head_t            coroutines_dead;

    /* the ready coroutines
     * 
     * ready: head -> ready -> .. -> running -> .. -> ready -> ..->
     *         |                                                   |
     *          ---------------------------<-----------------------
     */
    //tb_list_entry_head_t            coroutines_ready;

    // the suspend coroutines
    //tb_list_entry_head_t            coroutines_suspend;

}co_scheduler_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* start the coroutine function 
 *
 * @param scheduler         the scheduler, uses the default scheduler if be null
 * @param func              the coroutine function
 * @param priv              the passed user private data as the argument of function
 * @param stacksize         the stack size
 *
 * @return                  tb_true or tb_false
 */
bool                   co_scheduler_start(co_scheduler_t* scheduler, coroutine_func_t func, void const* priv, unsigned long long stacksize);

/* yield the current coroutine
 *
 * @param scheduler         the scheduler
 *
 * @return                  tb_true(yield ok) or tb_false(yield failed, no more coroutines)
 */
bool                   co_scheduler_yield(co_scheduler_t* scheduler);

/*! resume the given coroutine (suspended)
 *
 * @param scheduler         the scheduler
 * @param coroutine         the suspended coroutine
 * @param priv              the user private data as the return value of suspend() or sleep()
 *
 * @return                  the user private data from suspend(priv)
 */
void*                co_scheduler_resume(co_scheduler_t* scheduler, coroutine_t* coroutine, void const* priv);

/* suspend the current coroutine
 *
 * @param scheduler         the scheduler
 * @param priv              the user private data as the return value of resume() 
 *
 * @return                  the user private data from resume(priv)
 */
void*                co_scheduler_suspend(co_scheduler_t* scheduler, void const* priv);

/* finish the current coroutine
 *
 * @param scheduler         the scheduler
 */
void                   co_scheduler_finish(co_scheduler_t* scheduler);

/* sleep the current coroutine
 *
 * @param scheduler         the scheduler
 * @param interval          the interval (ms), infinity: -1
 *
 * @return                  the user private data from resume(priv)
 */
void*                co_scheduler_sleep(co_scheduler_t* scheduler, signed long interval);

/* switch to the given coroutine
 *
 * @param scheduler         the scheduler
 * @param coroutine         the coroutine
 */
void                   co_scheduler_switch(co_scheduler_t* scheduler, coroutine_t* coroutine);

/* wait io events 
 *
 * @param scheduler         the scheduler
 * @param sock              the socket
 * @param events            the waited events
 * @param timeout           the timeout, infinity: -1
 *
 * @return                  > 0: the events, 0: timeout, -1: failed
 */
//signed long                   co_scheduler_wait(co_scheduler_t* scheduler, tb_socket_ref_t sock,  events,signed long timeout);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
}

#endif