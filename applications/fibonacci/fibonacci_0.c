#include "fibonacci.h"

static char end_print[] =   "Fibonacci_0 finished.\n";
static char start_print[] =   "Starting fibonacci_0.\n";
static char new_value[] =   "> fibonacci0 new value. \n";
static char virgula[] = ",";

volatile static Message msg;                            

int main(void)
{
  static int n=0, result=0,step=0,i=0,temp=0;

  sys_Prints((unsigned int)&start_print);

  sys_Receive(&msg, divider);

  sys_Printi(msg.msg[0]);

  sys_Prints((unsigned int)&start_print);

  n = msg.msg[0];

  /*static int array[10] = {2,3,4,1,10,5,8,6,7,9}, size =10;
  
  if(n){
      // loop to access each array element
    for (step = 0; step < size - 1; ++step) {
    // loop to compare array elements
    for (i = 0; i < size - step - 1; ++i) {
      // compare two adjacent elements
      // change > to < to sort in descending order
      if (array[i] > array[i + 1]) {     
        // swapping occurs if elements
        // are not in the intended order
        temp = array[i];
        array[i] = array[i + 1];
        array[i + 1] = temp;
      }
    }
  }
}
    msg.length = size;
    for (i = 0; i < size; ++i) {
      msg.msg[i] = array[i]; 
      sys_Printi(array[i]);
      sys_Prins((unsigned int)&virgula);
    }*/

  result = ++n;

  sys_Printi(result);

  sys_Prints((unsigned int)&end_print);

}