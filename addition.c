#include <stdlib.h>
#include <stdint.h>
#include "magnum_def.h"

void incremental(struct magnum * magnum, int n){
    if (magnum->power!=0){
        //set magnum->power to 0
    }

    int i = abs(magnum->sign_n_prec)-1;
    while (n!=0){
        if (i<0){
            change_prec(magnum, 1);
            i++;
        }
        
        if (magnum->value[i] + n % 256 >=256)
            n+=256; // take overflow into account
        
        magnum->value[i] += n%256;
        n /= 256;
        i--;
    }
    clean_magnum(magnum);
}