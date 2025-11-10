#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "magnum_def.h"
#include "addition.h"
#include "convert.h"


void magnum_time_int(struct magnum *magnum, int n){
    if (n==0)
        _init_magnum(magnum);
    
    if (abs(n)>=256){
        struct magnum *nt = to_magnum_from_int(n);

        struct magnum *product(struct magnum *mag1, struct magnum *mag2);
        struct magnum *result = product(magnum, nt);

        free_magnum(nt);

        magnum->power = result->power;
        magnum->sign_n_prec = result->sign_n_prec;
    
        free(magnum->value);
        int prec = abs(result->sign_n_prec);
        magnum->value = (uint8_t *) malloc(prec*sizeof(uint8_t));
        for (int i = 0; i < prec; i++){
            magnum->value[i] = result->value[i];
        }
        return;
    }

    else {
        if (n<0)
            magnum->sign_n_prec = -magnum->sign_n_prec;

        int prec = abs(magnum->sign_n_prec);
        for (int i = 0; i < prec; i++){
            int res_i = (int)magnum->value[i] * n;
            magnum->value[i] = res_i;
            // correct overflow
            if (res_i >= 256)
                _carry(magnum, res_i/256, i - 1);
        }
    }
}


struct magnum *_karatsuba(struct magnum *mag1, struct magnum *mag2){
    int prec1 = abs(mag1->sign_n_prec);
    int prec2 = abs(mag2->sign_n_prec);

    if(prec1 == 1 && prec2 == 1){
        struct magnum *result = to_magnum_from_int((int)mag1->value[0] * (int)mag2->value[0]);
        result->power += mag1->power + mag2->power;
        return result;
    }

    if (prec1 == 1){
        struct magnum *result = _new_magnum();
        copy(result, mag2);
        magnum_time_int(result, mag1->value[0]);
        return result;
    }
    if (prec2 == 1){
        struct magnum *result = _new_magnum();
        copy(result, mag1);
        magnum_time_int(result, mag2->value[0]);
        return result;
    }

    else {
        if (prec1>=10000){
            printf("precision error");
            abort();
        }
        // printf("prec1 = %d\nprec2 = %d\n", prec1, prec2);
        print_magnum_info(mag1);
        print_magnum_info(mag2);
        printf("\n");
        //divide the number in 4 parts : (az + b)(cz + c)
        int z;
        if (prec1 <= prec2)
            z = prec1 / 2;
        else 
            z = prec2 / 2;
        // printf("z = %d\n", z);

        struct magnum *a = _new_magnum();{
            a->power = mag1->power;
            a->sign_n_prec = prec1 - z;
            a->value = mag1->value;
        }
        struct magnum *b = _new_magnum();{
            b->power = mag1->power;
            b->sign_n_prec = z;
            b->value = &(mag1->value[prec1 - z]);
        }
        struct magnum *c = _new_magnum();{
            c->power = mag2->power;
            c->sign_n_prec = prec2 - z;
            c->value = mag2->value;
        }
        struct magnum *d = _new_magnum();{
            d->power = mag2->power;
            d->sign_n_prec = z;
            d->value = &(mag2->value[prec2 - z]);
        }
        // print_magnum_info(a);
        // print_magnum_info(b);
        // print_magnum_info(c);
        // print_magnum_info(d);
        // printf("hello !\n\n");

        struct magnum *a_n_b = sum(a, b);
        struct magnum *c_n_d = sum(c, d);
        // print_magnum_info(c_n_d);
        struct magnum *x3 = _karatsuba(a_n_b, c_n_d);
        free_magnum(a_n_b);
        free_magnum(c_n_d);

        struct magnum *x1 = _karatsuba(a, c);
        free(a);
        free(c);
        struct magnum *x2 = _karatsuba(b, d);
        free(b);
        free(d);
        // print_magnum_info(x1);
        // print_magnum_info(x2);
        // print_magnum_info(x3);
        // printf("\n");

        // calcul x3-x1-x2
        x3->sign_n_prec = -x3->sign_n_prec;
        add(x3, x2);
        add(x3, x1);
        x3->sign_n_prec = -x3->sign_n_prec;
        
        // calcul x1*z^2 + (x3-x1-x2)*z + x2 = (az + b)(cz + c)
        x1->power += z + z;
        x3->power += z;
        add(x1, x3);
        // print_magnum_info(x1);
        // printf("\n");
        add(x1, x2);

        free_magnum(x3);
        free_magnum(x2);
        return x1;
    }
}


struct magnum *product(struct magnum *mag1, struct magnum *mag2){
    // _level(mag1, mag2);
    int prec = abs(mag1->sign_n_prec);

    struct magnum *result = _new_magnum();

    result = _karatsuba(mag1, mag2);

    _clean_magnum(mag1);
    _clean_magnum(mag2);
    _clean_magnum(result);
    return result;
}

