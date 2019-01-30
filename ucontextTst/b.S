/*tackdata: |                                                |         context        |||||||
 *             -------------------------------------------------------------------------|-----
 *                                                                             (16-align for macosx)
 *
 *
 *             -------------------------------------------------------------------------------
 * context:   |   r12   |   r13   |   r14   |   r15   |   rbx   |   rbp   |   rip   |   end   | ...
 *             -------------------------------------------------------------------------------
 *            0         8         16        24        32        40        48        56        |
 *                                                                                  |  16-align for macosx
 *                                                                                  |
 *                                                                       esp when jump to function
 *
 * @param stackdata     the stack data (rdi)
 * @param stacksize     the stack size (rsi)
 * @param func          the entry function (rdx)
 *
 * @return              the context pointer (rax)
 */
function(tb_context_make)

    // 保存栈顶指针到rax
    addq %rsi, %rdi
    movq %rdi, %rax

    /* 先对栈指针进行16字节对齐
     *
     *                      
     *             ------------------------------
     * context:   | retaddr |    padding ...     |
     *             ------------------------------
     *            |         | 
     *            |     此处16字节对齐
     *            |
     *  esp到此处时，会进行ret
     *
     * 这么做，主要是因为macosx下，对调用栈布局进行了优化，在保存调用函数返回地址的堆栈处，需要进行16字节对齐，方便利用SIMD进行优化
     */
    movabs $-16, %r8
    andq %r8, %rax

    // 保留context需要的一些空间，因为context和stack是在一起的，stack底指针就是context
    leaq -64(%rax), %rax

    // 保存func函数地址到context.rip
    movq %rdx, 48(%rax)

    /* 保存__end地址到context.end，如果在在func返回时，没有指定jump切换到有效context
     * 那么会继续会执行到此处，程序也就退出了
     */
    leaq __end(%rip), %rcx
    movq %rcx, 56(%rax)

    // 返回rax指向的栈底指针，作为context返回
    ret 

__end:
    // exit(0)
    xorq %rdi, %rdi
#ifdef TB_ARCH_ELF
    call _exit@PLT
#else
    call __exit
#endif
    hlt

endfunc

/* jump context (refer to boost.context)
 *
 * @param context       the to-context (rdi)
 * @param priv          the passed user private data (rsi)
 *
 * @return              the from-context (context: rax, priv: rdx)
 */
function(tb_context_jump)

    // 保存寄存器，并且按布局构造成当前context，包括jump()自身的返回地址retaddr(rip)
    pushq %rbp
    pushq %rbx
    pushq %r15
    pushq %r14
    pushq %r13
    pushq %r12

    // 保存当前栈基址rsp，也就是contex，到rax中
    movq %rsp, %rax

    // 切换到指定的新context上去，也就是切换堆栈
    movq %rdi, %rsp

    // 然后按context上的栈布局依次恢复寄存器
    popq %r12
    popq %r13
    popq %r14
    popq %r15
    popq %rbx
    popq %rbp

    // 获取context.rip，也就是make时候指定的func函数地址，或者是对方context中jump()调用的返回地址
    popq %r8

    // 设置返回值(from.context: rax, from.priv: rdx)，也就是来自对方jump()的context和传递参数
    movq %rsi, %rdx

    // 传递当前(context: rax, priv: rdx)，作为function(from)函数调用的入口参数
    movq %rax, %rdi

    /* 跳转切换到make时候指定的func函数地址，或者是对方context中jump()调用的返回地址
     *
     * 切换过去后，此时的栈布局如下：
     *
     * end是func的返回地址，也就是exit
     *
     *             -------------------------------
     * context: .. |   end   | args | padding ... |
     *             -------------------------------
     *             0             8        
     *             |             |  
     *            rsp   16-align for macosx
     */
    jmp *%r8

endfunc
