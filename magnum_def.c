#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "display.h" 
//gcc magnum_def.c convert.c display.c mag_c_test.c


struct magnum{
    int16_t sign_n_prec;//signed int 16 
    //abs(sign_n_prec) giving the precision 
    //(sign_n_prec<0) giving the sign of the magnum 
    int16_t power;//signed power value
    uint8_t * value;//adress of the magnum array on the stack
};


void _change_prec(struct magnum * magnum, int n){
    //change the precision of a magnum of n octet by adding somme 0 or by rounding the value if n<0
    if (n == 0)
        return;
    
    if (abs(magnum->sign_n_prec) + n <= 0){
        printf("the precision change is out of limit");
        return;
    }

    uint8_t* new_adress = (uint8_t *) malloc((abs(magnum->sign_n_prec)+n)*sizeof(uint8_t));

    if (n>0){
        for (int i = 0; i < n; i++){
            new_adress[i] = 0;
        }
        for (int i = 0; i < abs(magnum->sign_n_prec); i++){
            new_adress[i+n] = magnum->value[i];
        }
        
    }

    if (n<0){
        if (magnum->value[abs(magnum->sign_n_prec)+n+1]>=128){
            int i = 0;
            while (magnum->value[abs(magnum->sign_n_prec) + n - i]==255){

                if (abs(magnum->sign_n_prec) + n - i < 0){
                    //check the case where the rounding exced the magnum size
                    new_adress[0] = 1;
                    for (int i = 1; i < abs(magnum->sign_n_prec) + n; i++){
                        new_adress[i] = 0;
                    }

                    if (magnum->sign_n_prec<0)
                        magnum->sign_n_prec = magnum->sign_n_prec - n;
                    else
                        magnum->sign_n_prec = magnum->sign_n_prec + n;

                    magnum->power++;

                    free(magnum->value);
                    magnum->value = new_adress;
                    return;
                }

                i++;
            }
            magnum->value[abs(magnum->sign_n_prec) + n - i] += 1;
        }
        for (int i = 0; i < abs(magnum->sign_n_prec) + n; i++){
            new_adress[i] = magnum->value[i];
        }
        
    }

    if (magnum->sign_n_prec<0)
        magnum->sign_n_prec = magnum->sign_n_prec - n;
    else
        magnum->sign_n_prec = magnum->sign_n_prec + n;
    
    free(magnum->value);
    magnum->value = new_adress;
}


void _decrease_power(struct magnum * magnum, int n){
    if (n<0)
        _change_prec(magnum, -n);
    if (n>0){
        uint8_t* new_adress = (uint8_t *) malloc((abs(magnum->sign_n_prec) + n) * sizeof(uint8_t));
        for (int i = 0; i < abs(magnum->sign_n_prec); i++){
            new_adress[i] = magnum->value[i];
        }
        for (int i = abs(magnum->sign_n_prec); i < abs(magnum->sign_n_prec) + n; i++){
            new_adress[i] = 0;
        }

        free(magnum->value);
        magnum->value = new_adress;

        magnum->power -= n;

        if (magnum->sign_n_prec<0)
            magnum->sign_n_prec = magnum->sign_n_prec - n;
        else
            magnum->sign_n_prec = magnum->sign_n_prec + n;
    }
}


void _clean_magnum(struct magnum * magnum){
    //get a magnum address and remove the unused part at the begining/end of the value
    int down = 0;
    while (magnum->value[down]==0&&down<abs(magnum->sign_n_prec)-1){
        down++;
    }
    
    int up = 0;
    while (magnum->value[abs(magnum->sign_n_prec)-up-1]==0&&up<abs(magnum->sign_n_prec)-1){
        up++;
    }

    if (up==0&&down==0)
        return;
    else {
        int new_size = abs(magnum->sign_n_prec) - up - down;
        uint8_t* new_adress = (uint8_t *) malloc((new_size)*sizeof(uint8_t));

        for (int i = 0; i < new_size; i++){
            new_adress[i] = magnum->value[i+down];
        }
        
        if (magnum->sign_n_prec<0)
            magnum->sign_n_prec = -(new_size);
        else
            magnum->sign_n_prec = (new_size);
        magnum->power+=up;
        free(magnum->value);
        magnum->value = new_adress;
    }
}


struct magnum * copy(struct magnum * magnum){
    struct magnum * new_magnum = (struct magnum*) malloc(sizeof(struct magnum));

    new_magnum->power = magnum->power;
    new_magnum->sign_n_prec = magnum->sign_n_prec;

    int prec = abs(magnum->sign_n_prec);
    new_magnum->value = (uint8_t *) malloc(prec*sizeof(uint8_t));
    for (int i = 0; i < prec; i++){
        new_magnum->value[i] = magnum->value[i];
    }
    return new_magnum;
}


void free_magnum(struct magnum * magnum){
    free(magnum->value);
    free(magnum);
}


void print_magnum_info(struct magnum * magnum){
    display_tableau(magnum->value, abs(magnum->sign_n_prec));
    printf("precision : %d\n", abs(magnum->sign_n_prec));
    printf("power : %d\n", magnum->power);
    printf("sign : %d\n", magnum->sign_n_prec<0);
}