#if defined(__aarch64__) || defined(__x86_64__)
#   define __BIT_64__
#   define CPU_BITBYTE (8)
#else
#   define __BIT_32__
#   define CPU_BITBYTE (4)
#endif

/*! function
 * 
 * @code
    function(func_xxxx)
        ...
    endfunc
   @endcode
 */

#ifdef __ELF__
#   define function(name) \
    .text ;\
    .align CPU_BITBYTE ; \
    .global name ;\
    .type   name, %function; \
    .hidden name; \
name:
#else
#   define function(name) \
    .text ;\
    .align CPU_BITBYTE ; \
    .global name ;\
name:
#endif

// end function
#define endfunc
