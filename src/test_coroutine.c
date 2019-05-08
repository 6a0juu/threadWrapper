#include "coroutine/coroutine.h"
#include "coroutine/schedule.h"

static tb_void_t switchfunc(tb_cpointer_t priv)
{
    // 获取传入的参数
    tb_size_t count = (tb_size_t)priv;
    while (count--)
    {
        // 打印当前协程id
        tb_trace_i("[coroutine: %p]: %lu", tb_coroutine_self(), count);

        // 让出当前协程，进行切换
        tb_coroutine_yield();
    }
}
int main(int argc, char** argv)
{
    // 初始化tbox
    if (!tb_init(tb_null, tb_null)) return -1;

    // 初始化一个调度器实例
    tb_co_scheduler_ref_t scheduler = tb_co_scheduler_init();
    if (scheduler)
    {
        // 开启一个协程，传递switchfunc切换函数和参数10，最后一个参数指定栈大小，传0使用默认值
        tb_coroutine_start(scheduler, switchfunc, (tb_cpointer_t)10, 0);

        // 开启协程，使用指定的栈大小：8192
        tb_coroutine_start(scheduler, switchfunc, (tb_cpointer_t)10, 8192);

        /* 运行调用，开始运行里面的所有协程
         *
         * 第二个参数指定是否启用独占模式，这个稍后细讲
         */
        tb_co_scheduler_loop(scheduler, tb_true);

        // 退出调度器
        tb_co_scheduler_exit(scheduler);
    }

    // 退出tbox
    tb_exit();
}