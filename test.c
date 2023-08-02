#include<stdio.h>
#include<stdlib.h>

int main(){
    int i, a, b, c, da, db, dc;
    a =  0;
    b =  0;
    c =  0;
    da = 0;
    db = 0;
    dc = 0;

    i = 0;

    while(1){
        if( a <= 4 ){
            if(b <= 4 ){
                if( c <= 4 ){
                    if( da <= 4 ){
                        if( db <= 4 ){
                            if( dc <= 4 ){
                                
                                if( (a + b + c) <= 4 && (da + db + dc) <= 4 ){
                                    printf("%d: (%d %d %d) (%d %d %d)\n", i++, a,b,c,da,db,dc);
                                }
                                dc++;
                            } else{
                                dc = 0;
                                db++;
                            }
                        } else{
                            db = 0;
                            da++;
                        }
                    } else{
                        da = 0;
                        c++;
                    } 
                } else{ 
                    c = 0;
                    b++;
                }
            } else{
                b = 0;
                a++;
            }
        } else{
            a = 0;
            b = 0;
            c = 0;
            da = 0;
            db = 0;
            dc = 0;
        }
        if(i>35*35) return 0;
    }
}