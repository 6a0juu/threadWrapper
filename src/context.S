#if defined(__i386__)
#   include "arch/x86/context.S"
#elif defined(__x86_64__)
#   include "arch/x86-64/context.S"
#elif defined(__aarch64__)
#   include "arch/aarch64/context.S"
#elif defined(__arm__)
#   include "arch/aarch32/context.S"
#elif defined(__mips__)
#   include "arch/mips/context.S"
#endif