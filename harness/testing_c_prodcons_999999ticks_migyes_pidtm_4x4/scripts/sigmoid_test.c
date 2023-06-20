#include <stdio.h>


float Q_rsqrt( float number ){
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
	return y;
}


int main(){
    float reward;
    float i;
    for(i = -300.0; i < 300.0; i=i+0.01){
        //reward =  (Q_rsqrt(7000+(i*i)) * i * -500) + 500;
		reward =  (Q_rsqrt(1000+(i*i)) * i * -100) + 100;
        printf("%f;%f\n", i, reward);
    }

    return 0;
}