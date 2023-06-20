#include "fibonnaci.h"

static char end_print[] =   "Fibonacci_4 finished.\n";
static char start_print[] =   "Starting Fibonnaci_4.\n";
static char new_value[] =   "> Fibonnaci4 new value. \n";

volatile static Message msg;                            

int main(void)
{
  static int n=0, result=0;
  
  sys_Prints((unsigned int)&start_print);

  sys_Receive(&msg, divider);
  
  n = msg.msg[4];

  result = ++n;

  sys_Printi(result);

  //scanf("%d", &n);
  //printf("%d\n", fib(n));
  return 0;
}