#include "fibonnaci.h"

static char end_print[] =   "Fibonacci_2 finished.\n";
static char start_print[] =   "Starting Fibonnaci_2.\n";
static char new_value[] =   "> Fibonnaci2 new value. \n";

volatile static Message msg;                            

int main(void)
{
  static int n=0, result=0;

  sys_Receive(&msg, divider);
  
  sys_Printi(msg.msg[2]);

  sys_Prints((unsigned int)&start_print);

  n = msg.msg[2];

  result = ++n;

  sys_Printi(result);

  sys_Prints((unsigned int)&end_print);

  //scanf("%d", &n);
  //printf("%d\n", fib(n));
  return 0;
}