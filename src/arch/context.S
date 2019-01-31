#include "prefix.S"
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