#include "coroutine/coroutine.h"
#include "coroutine/schedule.h"

static void switchfunc(void const* priv)
{
    // 获取传入的参数
    unsigned long count = (unsigned long)priv;
    while (count--)
    {
        // 打印当前协程id
        printf("[coroutine: %p]: %lu", tw_coroutine_self(), count);

        // 让出当前协程，进行切换
        tw_coroutine_yield();
    }
}
int main(int argc, char** argv)
{
    // 初始化一个调度器实例
    tw_co_scheduler_ref_t scheduler = tw_co_scheduler_init();
    if (scheduler)
    {
        // 开启一个协程，传递switchfunc切换函数和参数10，最后一个参数指定栈大小，传0使用默认值
        tw_coroutine_start(scheduler, switchfunc, (void const*)10, 0);

        // 开启协程，使用指定的栈大小：8192
        tw_coroutine_start(scheduler, switchfunc, (void const*)10, 8192);

        /* 运行调用，开始运行里面的所有协程
         *
         * 第二个参数指定是否启用独占模式，这个稍后细讲
         */
        tw_co_scheduler_loop(scheduler, true);

        // 退出调度器
        tw_co_scheduler_exit(scheduler);
    }
}