//#include "../context.h"


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

#if defined(__aarch64__) || defined(__x86_64__)
#   define __BIT_64__
#   define CPU_BITBYTE (8)
#else
#   define __BIT_32__
#   define CPU_BITBYTE (4)
#endif

#if defined(__i386__)
#   include "x86/context.S"
#elif defined(__x86_64__)
#   include "x86-64/context.S"
#elif defined(__aarch64__)
#   include "aarch64/context.S"
#elif defined(__arm__)
#   include "aarch32/context.S"
#elif defined(__mips__)
#   include "mips/context.S"
#endif
