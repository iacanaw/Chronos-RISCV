#include "../FreeRTOS/FreeRTOS/include/message.h"
#include "../FreeRTOS/FreeRTOS/include/system_call.h"
//#include "../FreeRTOS/FreeRTOS/system_call.c"

// https://www.geeksforgeeks.org/write-memcpy/
void memcpy(void *dest, void *src, unsigned int n){
   // Typecast src and dest addresses to (char *)
   char *csrc = (char *)src;
   char *cdest = (char *)dest;
  
   // Copy contents of src[] to dest[]
   for (int i=0; i<n; i++)
       cdest[i] = csrc[i];
}