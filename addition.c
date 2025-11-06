#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "magnum_def.h"


void _carry(struct magnum * magnum, int n, int i){
    //add a carry of n to magnum->value at indice i
    while (n!=0){
        if (i<0){
            if (n<0){
                printf("negative carry greater than magnum.");
                abort();
            }
            
            _change_prec(magnum, 1);
            i++;
            continue;
        }
        
        if (magnum->value[i] + n % 256 >=256)
            n+=256; // take overflow into account
        if (magnum->value[i] + n % 256 < 0)
            n-=256; // take underflow into account
        
        magnum->value[i] += n%256;
        n /= 256;
        i--;
    }
}



void _level(struct magnum * mag1, struct magnum * mag2){
    //set two magnum to the same power and precision in order to sum them
    if (mag1->power > mag2->power)
        _decrease_power(mag1, mag1->power - mag2->power);
    else if (mag1->power < mag2->power)
        _decrease_power(mag2, mag2->power - mag1->power);

    if (abs(mag1->sign_n_prec) > abs(mag2->sign_n_prec))
        _change_prec(mag2, abs(mag1->sign_n_prec) - abs(mag2->sign_n_prec));
    else if (abs(mag1->sign_n_prec) < abs(mag2->sign_n_prec))
        _change_prec(mag1, abs(mag2->sign_n_prec) - abs(mag1->sign_n_prec));
        

}



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
    _carry(magnum, n, i);

    _clean_magnum(magnum);
}



void add(struct magnum * mag1, struct magnum * mag2){
    //add mag1 to mag2
    _level(mag1, mag2);

    if ((mag1->sign_n_prec > 0) == (mag2->sign_n_prec > 0)){
        int prec = abs(mag1->sign_n_prec);
        for (int i = 0; i < prec; i++){
            mag1->value[i] += mag2->value[i];
            if (mag1->value[i] < mag2->value[i])
                _carry(mag1, 1, i + 1);
        }
    }

    else {
        struct magnum* magmax = mag1;

        if (abs(mag1->sign_n_prec) > abs(mag2->sign_n_prec))
            magmax = mag1;
        else if (abs(mag1->sign_n_prec) < abs(mag2->sign_n_prec))
            magmax = mag2;

        else {
            int prec1 = abs(mag1->sign_n_prec);
            for (int i = 0; i < prec1; i++){

                if (mag1->value[i] > mag2->value[i]){
                    magmax = mag1;
                    break;
                }
                else if (mag1->value[i] < mag2->value[i]){
                    magmax = mag2;
                    break;
                }
            }
        }

        struct magnum* magmin = mag2;
        if (magmax == mag2){
            mag1->sign_n_prec = -mag1->sign_n_prec;
            magmin = mag1;
        }

        int prec = abs(mag1->sign_n_prec);
        for (int i = prec-1; i > -1; i--){
            if (magmax->value[i] < magmin->value[i]){
                if (mag1==magmin)
                    _carry(mag1, 1, i - 1);
                else
                    _carry(mag1, -1, i - 1);
            }
            mag1->value[i] = magmax->value[i] - magmin->value[i];
        }
    }
    _clean_magnum(mag1);
    _clean_magnum(mag2);
}


struct magnum * sum(struct magnum * mag1, struct magnum * mag2){
    //return a magnum which is the sum of the two given in argument
    struct magnum *result = _new_magnum();
    copy(mag1);
    add(result, mag2);
    return result;
}

void opposite(struct magnum * magnum){
    magnum->sign_n_prec = -magnum->sign_n_prec;
}