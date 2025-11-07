#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "magnum_def.h"
#include "addition.h"
#include "convert.h"




struct magnum *_karatsuba(struct magnum *mag1, struct magnum *mag2){
    int prec1 = abs(mag1->sign_n_prec);
    int prec2 = abs(mag2->sign_n_prec);
    if(prec1 == 1 && prec1 == 1){
        struct magnum *result = to_magnum_from_int(mag1->value[0] * mag2->value[0]);
        result->power += mag1->power + mag2->power;
        return result;
    }
    //prec1 || prec2
    //divide n conquer
}


struct magnum *product(struct magnum *mag1, struct magnum *mag2){
    _level(mag1, mag2);
    int prec = abs(mag1->sign_n_prec);

    struct magnum *result = _new_magnum();

    result = _karatsuba(mag1, mag2);

    _clean_magnum(mag1);
    _clean_magnum(mag2);
    _clean_magnum(result);
    return result;
}


void magnum_time_int(struct magnum *mag1, int n){
    if (n==0)
        _init_magnum(mag1);
    
    if (abs(n)>=256){
        struct magnum *nt = to_magnum_from_int(n);
        struct magnum *result = product(mag1, nt);
        free_magnum(nt);

        mag1->power = result->power;
        mag1->sign_n_prec = result->sign_n_prec;
    
        free(mag1->value);
        int prec = abs(result->sign_n_prec);
        mag1->value = (uint8_t *) malloc(prec*sizeof(uint8_t));
        for (int i = 0; i < prec; i++){
            mag1->value[i] = result->value[i];
        }
        return;
    }

    else {
        if (n<0)
            mag1->sign_n_prec = -mag1->sign_n_prec;

        int prec = abs(mag1->sign_n_prec);
        for (int i = 0; i < prec; i++){
            int res_i = (int)mag1->value[i] * n;
            mag1->value[i] = res_i;
            if (256 <= res_i)
                _carry(mag1, res_i/256, i + 1);
        }
    }
}