#include <stdio.h>
#if defined(__ELF__)
#define E 1
#else 
#define E 0
#endif
int main(int argc, char *argv[]) {
  puts("Hello world");
  printf("%d",E);
  return 0;
}

