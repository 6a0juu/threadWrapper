/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "coroutine.h"
#include "scheduler.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the default stack size
#define COROUTINE_STACK_DEFSIZE          (8192 << 1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static void coroutine_entry(context_from_t from)
{
    // get the from-coroutine 
    coroutine_t* coroutine_from = (coroutine_t*)from.priv;
    //assert(coroutine_from && from.context);

    // update the context
    coroutine_from->context = from.context;
    //assert(from.context);

    // get the current coroutine
    coroutine_t* coroutine = (coroutine_t*)coroutine_self();
    //assert(coroutine);


    // trace
    printf("entry: %p stack: %p - %p from coroutine(%p)\n", coroutine, coroutine->stackbase - coroutine->stacksize, coroutine->stackbase, coroutine_from);

    // get function and private data
    coroutine_func_t func = coroutine->rs.func.func;
    void const*       priv = coroutine->rs.func.priv;
    //assert(func);

    // reset rs data first for waiting io
    memset(&coroutine->rs, 0, sizeof(coroutine->rs));

    // call the coroutine function
    func(priv);

    // finish the current coroutine and switch to the other coroutine
    co_scheduler_finish((co_scheduler_t*)co_scheduler_self());
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
coroutine_t* coroutine_init(co_scheduler_ref_t scheduler, coroutine_func_t func, void const* priv, tb_size_t stacksize)
{
    // check
    //assert((scheduler && func) == NULL);

    // done
    bool       ok = false;
    coroutine_t* coroutine = tb_null;
    do
    {
        // init stack size
        if (!stacksize) stacksize = COROUTINE_STACK_DEFSIZE;


        /* make coroutine
         *
         * TODO: 
         *
         * - segment stack 
         *
         *  -----------------------------------------------
         * | coroutine | guard | ... stacksize ... | guard |
         *  -----------------------------------------------
         */
        coroutine = (coroutine_t*)malloc(sizeof(coroutine_t) + stacksize + sizeof(unsigned short));
        //assert(coroutine);

        // save scheduler
        coroutine->scheduler = scheduler;

        // init stack
        coroutine->stackbase = (unsigned char*)&(coroutine[1]) + stacksize;
        coroutine->stacksize = stacksize;

        // fill guard
        //coroutine->guard = TB_COROUTINE_STACK_GUARD;
        //tb_bits_set_u16_ne(coroutine->stackbase, TB_COROUTINE_STACK_GUARD);

        // init function and user private data
        coroutine->rs.func.func = func;
        coroutine->rs.func.priv = priv;

        // make context
        coroutine->context = context_make(coroutine->stackbase - stacksize, stacksize, coroutine_entry);
        //assert_and_check_break(coroutine->context);

        // ok
        ok = true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (coroutine) coroutine_exit(coroutine); 
        coroutine = NULL;
    }

    // trace
    printf("init %p\n", coroutine);

    // ok?
    return coroutine;
}
coroutine_t* coroutine_reinit(coroutine_t* coroutine, coroutine_func_t func, void const* priv, tb_size_t stacksize)
{
    // check
    //assert((coroutine && func) == tb_null);

    // done
    bool ok = false;
    do
    {
        // init stack size
        if (!stacksize) stacksize = COROUTINE_STACK_DEFSIZE;

        // remake coroutine
        if (stacksize > coroutine->stacksize)
            coroutine = (coroutine_t*)realloc(coroutine, sizeof(coroutine_t) + stacksize + sizeof(unsigned short));//bugs
        else stacksize = coroutine->stacksize;
        //assert(coroutine && coroutine->scheduler);

        // init stack
        coroutine->stackbase = (unsigned char*)&(coroutine[1]) + stacksize;
        coroutine->stacksize = stacksize;

        // fill guard
        //coroutine->guard = TB_COROUTINE_STACK_GUARD;
        //tb_bits_set_u16_ne(coroutine->stackbase, TB_COROUTINE_STACK_GUARD);

        // init function and user private data
        coroutine->rs.func.func = func;
        coroutine->rs.func.priv = priv;

        // make context
        coroutine->context = context_make(coroutine->stackbase - stacksize, stacksize, coroutine_entry);
        //assert(coroutine->context);

        // ok
        ok = true;

    } while (0);

    // failed? reset it
    if (!ok) coroutine = NULL
}
void coroutine_exit(coroutine_t* coroutine)
{
    // check
    //assert(coroutine);

    // trace
    print("exit: %p\n", coroutine);

    // exit it
    free(coroutine);
}
/*
#ifdef __debug__
void tb_coroutine_check(tb_coroutine_t* coroutine)
{
    // check
    tb_assert(coroutine);

    // this coroutine is original for scheduler?
    tb_check_return(!tb_coroutine_is_original(coroutine));

    // check stack underflow
    if (coroutine->guard != TB_COROUTINE_STACK_GUARD)
    {
        // trace
        tb_trace_e("this coroutine stack is underflow!");

        // dump stack
        tb_dump_data(coroutine->stackbase - coroutine->stacksize, coroutine->stacksize);

        // abort
        tb_abort();
    }

    // check stack overflow
    if (tb_bits_get_u16_ne(coroutine->stackbase) != TB_COROUTINE_STACK_GUARD)
    {
        // trace
        tb_trace_e("this coroutine stack is overflow!");

        // dump stack
        tb_dump_data(coroutine->stackbase - coroutine->stacksize, coroutine->stacksize);

        // abort
        tb_abort();
    }

    // check
    tb_assert(coroutine->context);
}
#endif
*/