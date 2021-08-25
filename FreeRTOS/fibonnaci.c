#include <stdio.h>

int fibonnaci( int n ){
    if( n == 1 || n == 0 ) return n;
    return fibonnaci(n-1) + fibonnaci(n-2);
}

