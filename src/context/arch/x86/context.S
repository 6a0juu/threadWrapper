/* make context 
 *
 *
 *             -----------------------------------------------------------------------------------------
 * stackdata: |                                                          |         context        |||||||
 *             -----------------------------------------------------------------------------------|-----
 *                                                                                           (16-align)
 *
 *
 *                                    func      __end   __entry                  from
 *             -----------------------------------------------------------------------------------------
 * context:   |   edi   |   esi   |   ebx   |   ebp   |   eip   | retval | context |  priv  |  padding  |
 *             -----------------------------------------------------------------------------------------
 *            0         4         8         12        16        20        24    arguments 
 *                                                              |         |
 *                                                              |      16-align
 *                                                              |
 *                                                   esp when jump to function
 *
 * @param stackdata     the stack data (esp + 4)
 * @param stacksize     the stack size (esp + 8)
 * @param func          the entry function (esp + 12)
 *
 * @return              the context pointer (eax)
 */
function(context_make)

    // save the stack top to eax
    movl 4(%esp), %eax
    addl 8(%esp), %eax

    // reserve space for first argument(from) of context-function
    leal -8(%eax), %eax

    // 16-align of the stack top address for macosx
    andl $-16, %eax

    // reserve space for context-data on context-stack 
    leal -24(%eax), %eax

    /* context.ebx = func
     *
     * @note ebp will be affected only when enter into the context function first
     */
    movl 12(%esp), %edx
    movl %edx, 8(%eax)

    /* init retval = a writeable space (context)
     *
     * it will write context.edi and context.esi (unused) when jump to a new context function entry first
     */
    movl %eax, 20(%eax) 

    // context.eip = the address of label __entry
    call 1f
1:  popl %ecx
    addl $__entry - 1b, %ecx
    movl %ecx, 16(%eax) 

    /* context.ebp = the address of label __end
     *
     * @note ebp will be affected only when enter into the context function first
     */
    call 2f
2:  popl %ecx
    addl $__end - 2b, %ecx
    movl %ecx, 12(%eax) 

    // return the context pointer
    ret

__entry:
    
    /* pass arguments(context: eax, priv: edx) to the context function
     *
     *              patch __end
     *                  |
     *                  |        old-context
     *              ----|------------------------------------
     * context: .. | retval | context |   priv   |  padding  |  
     *              -----------------------------------------
     *             0        4     arguments 
     *             |        |
     *            esp    16-align
     *           (now)
     */
    movl %eax, (%esp)
    movl %edx, 0x4(%esp)

    // retval = the address of label __end
    pushl %ebp

    /* jump to the context function entry
     *
     * @note need not adjust stack pointer(+4) using 'ret $4' when enter into function first
     */
    jmp *%ebx

__end:
    // exit(0)
    xorl  %eax, %eax
    movl  %eax, (%esp)
#ifdef TB_ARCH_ELF
    call  _exit@PLT
#else
    call  __exit
#endif
    hlt

//endfunc

/* jump context 
 *
 * @code
 *
 * subl $4, %esp  <---- padding (need ret $4) 
 * pushl priv
 * pushl context 
 * pushl retval
 * call tb_context_jump
 * addl $12, %esp
 *
 * tb_context_jump():
 *     ret 4
 *
 * @endcode
 *
 * @param retval        the from-context (esp + 4)
 * @param context       the to-context (esp + 8)
 * @param priv          the passed user private data (esp + 12)
 *
 * @return              the from-context (retval (esp + 4))
 */
function(context_jump)

    // save registers and construct the current context
    pushl %ebp 
    pushl %ebx 
    pushl %esi 
    pushl %edi 

    // save the old context(esp) to eax
    movl %esp, %eax

    // ecx = argument(context)
    movl 24(%esp), %ecx

    // edx = argument(priv)
    movl 28(%esp), %edx

    // switch to the new context(esp) and stack
    movl %ecx, %esp

    // restore registers of the new context
    popl %edi 
    popl %esi
    popl %ebx
    popl %ebp

    /* return from-context(retval: [to_esp + 4](context: eax, priv: edx)) from jump 
     *
     * it will write context.edi and context.esi (unused) when jump to a new context function entry first
     */
    movl 4(%esp), %ecx
    movl %eax, (%ecx)
    movl %edx, 4(%ecx)

    /* jump to the return or entry address(eip)
     *
     * @note need adjust stack pointer(+4) when return from tb_context_jump()
     *
     *                           old-context
     *              ---------------------------------------------------
     * context: .. |   eip   | retval | context |   priv   |  padding  |  
     *              ---------------------------------------------------
     *             0         4        8     arguments 
     *             |                  |
     *            esp              16-align
     *           (now)
     */
    ret $4

//endfunc

