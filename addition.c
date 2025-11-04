#include <stdlib.h>
#include <stdint.h>
#include "magnum_def.h"

void incremental(struct magnum * magnum, unsigned int n){
    // increase the value of a magnum by n (n have to be a natural number).
    int i;
    if (magnum->power<0){
        _change_prec(magnum, -abs(magnum->sign_n_prec)-magnum->power+1);
        i = 0;
    }
    else 
        i = abs(magnum->sign_n_prec)-1;
    
    if (magnum->power>0){
        _decrease_power(magnum, magnum->power);
    }


    while (n!=0){
        if (i<0){
            _change_prec(magnum, 1);
            i++;
            continue;
        }
        
        if (magnum->value[i] + n % 256 >=256)
            n+=256; // take overflow into account
        
        magnum->value[i] += n%256;
        n /= 256;
        i--;
    }
    _clean_magnum(magnum);
}