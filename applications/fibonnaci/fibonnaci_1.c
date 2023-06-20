#include "fibonnaci.h"

static char end_print[] =   "Fibonacci_1 finished.\n";
static char start_print[] =   "Starting Fibonnaci_1.\n";
static char new_value[] =   "> Fibonnaci1 new value. \n";

volatile static Message msg;                            

int main(void)
{
  static int n=0, result=0;
  
  sys_Prints((unsigned int)&start_print);

  sys_Receive(&msg, divider);
  
  sys_Printi(msg.msg[1]);

  sys_Prints((unsigned int)&start_print);

  n = msg.msg[1];

  result = ++n;

  sys_Printi(result);

  sys_Prints((unsigned int)&end_print);

  //scanf("%d", &n);
  //printf("%d\n", fib(n));
  return 0;
}