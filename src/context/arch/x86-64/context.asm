; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; declaration
;;

; exit(value)
extern _exit:proc

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; implementation
;;
.code

; make context
;
; 
;             -----------------------------------------------------------------------------------------
; stackdata: |                                                          |         context        |||||||
;             -----------------------------------------------------------------------------------|-----
;                                                                                           (16-align)
;
;
;             ---------------------------------------
; context:   |  fiber  | dealloc |  limit  |  base   | 
;             ---------------------------------------   
;            0         8         16         24
;                                                                                                          
;             ---------------------------------------
;            |   r12   |   r13   |   r14   |   r15   | 
;             ---------------------------------------   
;            32        40        48        56                 ---------------------------------------
;                                                            |                                       |
;                                    func    __end           |         __entry    arguments     retval(from)
;             --------------------------------------------------------------------------------------------------------------------------
;            |   rdi   |   rsi   |   rbx   |   rbp   | retval(saved) |   rip   |   unused  | context(unused) |  priv(unused)  | padding |
;             --------------------------------------------------------------------------------------------------------------------------
;            64        72        80        88        96              104       112         120               128              136                          
;                                                                              |         
;                                                                              | 16-align
;                                                                              |
;                                                                    rsp when jump to function
;
;
; @param stackdata     the stack data (rcx)
; @param stacksize     the stack size (rdx)
; @param func          the entry function (r8)
;
; @return              the context pointer (rax)
;;
context_make proc frame

    ; .xdata for a function's structured exception handling unwind behavior
    .endprolog

    ; save the stack top to rax
    mov rax, rcx
    add rax, rdx

    ; reserve space for first argument(from) and retval(from) item of context-function
    ; 3 * 8 = 24
    sub rax, 24

    ; 16-align of the stack top address 
    and rax, -16

    ; reserve space for context-data on context-stack
    sub rax, 112

    ; context.rbx = func
    mov [rax + 80], r8

    ; save bottom address of context stack as 'limit'
    mov [rax + 16], rcx

    ; save address of context stack limit as 'dealloction stack'
    mov [rax + 8], rcx

    ; save top address of context stack as 'base'
    add rcx, rdx
    mov [rax + 24], rcx

    ; init fiber-storage to zero
    xor rcx, rcx
    mov [rax], rcx

    ; init context.retval(saved) = a writeable space (unused)
    ;
    ; it will write context (unused) and priv (unused) when jump to a new context function entry first
    ;;
    lea rcx, [rax + 128]
    mov [rax + 96], rcx

    ; context.rip = the address of label __entry
    lea rcx, __entry
    mov [rax + 104], rcx

    ; context.end = the address of label __end
    lea rcx, __end
    mov [rax + 88], rcx

    ; return pointer to context-data
    ret 

__entry:

    ; patch return address (__end) on stack
    push rbp

    ; jump to the context function entry(rip)
    ;
    ;
    ;                                     
    ;              -----------------------------------------------------------------
    ; context: .. |   end   |  unused  | context(unused) |  priv(unused)  | padding |
    ;              -----------------------------------------------------------------
    ;             0         8 arguments 
    ;             |         
    ;            rsp 16-align 
    ;           (now)
    ;;
    jmp rbx

__end:
    ; exit(0)
    xor rcx, rcx
    call _exit
    hlt

context_make endp 

; jump context (refer to boost.context)
;
; @param retval        the from-context (rcx)
; @param context       the to-context (rdx)
; @param priv          the passed user private data (r8)
;
; @return              the from-context (retval: rcx)
;;
context_jump proc frame

    ; .xdata for a function's structured exception handling unwind behavior
    .endprolog

    ; save the hidden argument: retval (from-context)
    push rcx

    ; save registers and construct the current context
    push rbp
    push rbx
    push rsi
    push rdi
    push r15
    push r14 
    push r13
    push r12

    ; load TIB
    mov r10, gs:[030h]

    ; save current stack base
    mov rax, [r10 + 08h]
    push rax

    ; save current stack limit
    mov rax, [r10 + 010h]
    push rax
    
    ; save current deallocation stack
    mov rax, [r10 + 01478h]
    push rax
    
    ; save fiber local storage
    mov rax, [r10 + 018h]
    push rax

    ; save the old context(esp) to r9
    mov r9, rsp

    ; switch to the new context(esp) and stack
    mov rsp, rdx

    ; load TIB
    mov r10, gs:[030h]

    ; restore fiber local storage
    pop rax
    mov [r10 + 018h], rax

    ; restore deallocation stack
    pop rax
    mov [r10 + 01478h], rax
    
    ; restore stack limit
    pop rax
    mov [r10 + 010h], rax
    
    ; restore stack base
    pop rax
    mov [r10 + 08h], rax

    ; restore registers of the new context
    pop r12
    pop r13
    pop r14
    pop r15
    pop rdi
    pop rsi
    pop rbx
    pop rbp

    ; restore retval (saved) to rax
    pop rax 

    ; restore the return or function address(r10)
    pop r10

    ; return from-context(retval: [rcx](context: r9, priv: r8)) from jump 
    ;
    ; it will write context (unused) and priv (unused) when jump to a new context function entry first
    ;;
    mov [rax], r9
    mov [rax + 8], r8

    ; pass old-context(rcx(context: r9, priv: r8)) arguments to the context function
    ;
    ; tb_context_from_t from;
    ; func(from)
    ;
    ; lea rcx, address of from
    ; rcx.context = r9
    ; rcx.priv = r8
    ; call func
    ;;
    mov rcx, rax

    ; jump to the return or function address(rip)
    ;
    ;
    ;                                     
    ;              -----------------------------------------------------------------
    ; context: .. |   end   |  unused  | context(unused) |  priv(unused)  | padding |
    ;              -----------------------------------------------------------------
    ;             0         8 arguments 
    ;             |         |
    ;            rsp     16-align 
    ;           (now)
    ;;
    jmp r10

context_jump endp 

end

