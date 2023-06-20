#include "fibonacci.h"

static char end_print[] =   "Fibonacci_3 finished.\n";
static char start_print[] =   "Starting fibonacci_3.\n";
static char new_value[] =   "> fibonacci3 new value. \n";

volatile static Message msg;                            

int main(void)
{
  static int n=0, result=0;

  sys_Receive(&msg, divider);

  sys_Printi(msg.msg[3]);

  sys_Prints((unsigned int)&start_print);

  n = msg.msg[3];

  result = ++n;

  sys_Printi(result);

  sys_Prints((unsigned int)&end_print);

  //scanf("%d", &n);
  //printf("%d\n", fib(n));
  return 0;
}