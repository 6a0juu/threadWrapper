#include "context.h"
#include <time.h>
#include <stdio.h>
// the switch count
#define COUNT       (10000000)

#if (defined(__MINGW64__) || defined(__MINGW32__))
typedef __int64                     tw_int64_t;
typedef unsigned __int64            tw_uint64_t;
#elif (defined(__BIT_WIDTH__)) && !(defined(__MINGW64__) || defined(__MINGW32__))
typedef signed long                 tw_int64_t;
typedef unsigned long               tw_uint64_t;
#else
typedef signed long long            tw_int64_t;
typedef unsigned long long          tw_uint64_t;
#endif

static void tb_demo_platform_context_test_func1(context_from_t from)
{
    // check
    context_ref_t* contexts = (context_ref_t*)from.priv;
    //assert_and_check_return(contexts);

    // save main context
    contexts[0] = from.context;

    // jump to context2
    from.context = contexts[2];

    // loop
    unsigned long count = 10;
    while (count--)
    {
        // trace
        printf("func1: %lu", count);

        // switch to the func2
        from = context_jump(from.context, contexts);
    }

    // switch to the main function
    context_jump(contexts[0], NULL);
}
static void tb_demo_platform_context_test_func2(context_from_t from)
{
    // check
    context_ref_t* contexts = (context_ref_t*)from.priv;
    //tb_assert_and_check_return(contexts);

    // loop
    unsigned long count = 10;
    while (count--)
    {
        // trace
        printf("func2: %lu", count);

        // switch to the func1
        from = context_jump(from.context, contexts);
    }

    // switch to the main function
    context_jump(contexts[0], NULL);
}
static context_from_t test()
{
    context_from_t from = {0};
    return from;
}
static void tb_demo_platform_context_test()
{ 
    // the stacks
    static context_ref_t contexts[3];
    static unsigned char    stacks1[8192];
    static unsigned char    stacks2[8192];

    test();

    // make context1
    contexts[1] = context_make(stacks1, sizeof(stacks1), tb_demo_platform_context_test_func1);

    // make context2
    contexts[2] = context_make(stacks2, sizeof(stacks2), tb_demo_platform_context_test_func2);

    // trace
    printf("test: enter");

    // switch to func1
    context_jump(contexts[1], contexts);

    // trace
    printf("test: leave");
}
static void tb_demo_platform_context_perf_func1(context_from_t from)
{
    // check
    context_ref_t* contexts = (context_ref_t*)from.priv;
    //tb_assert_and_check_return(contexts);

    // save main context
    contexts[0] = from.context;

    // jump to context2
    from.context = contexts[2];

    // loop
    volatile unsigned long count = COUNT >> 1;
    while (count--)
    {
        // switch to the func2
        from = context_jump(from.context, contexts);
    }

    // switch to the main function
    context_jump(contexts[0], NULL);
}
static void tb_demo_platform_context_perf_func2(context_from_t from)
{
    // check
    context_ref_t* contexts = (context_ref_t*)from.priv;
    //tb_assert_and_check_return(contexts);

    // loop
    volatile unsigned long count = COUNT >> 1;
    while (count--)
    {
        // switch to the func1
        from = context_jump(from.context, contexts);
    }

    // switch to the main function
    context_jump(contexts[0], NULL);
}
static void tb_demo_platform_context_perf()
{ 
    // the stacks
    static context_ref_t contexts[3];
    static unsigned char        stacks1[8192];
    static unsigned char        stacks2[8192];

    // make context1
    contexts[1] = context_make(stacks1, sizeof(stacks1), tb_demo_platform_context_perf_func1);

    // make context2
    contexts[2] = context_make(stacks2, sizeof(stacks2), tb_demo_platform_context_perf_func2);

    // init start time
    clock_t startime = clock();

    // switch to func1
    context_jump(contexts[1], contexts);

    // computing time
    double duration = difftime(clock(), startime);

    // trace
    printf("perf: %d switches in %lf ms, %lld switches per second", COUNT, duration, (((tw_int64_t)1000 * COUNT) / duration));
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
int main(int argc, char** argv)
{
    tb_demo_platform_context_test();
    tb_demo_platform_context_perf();
    return 0;
}