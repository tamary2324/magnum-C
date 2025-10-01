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
    int8_t * value;//adress of the magnum array on the stack
};



void clean_magnum(struct magnum * magnum){
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
        uint8_t* new_adress = (uint8_t *) malloc((abs(magnum->sign_n_prec)-up-down)*sizeof(uint8_t));
        for (int i = 0; i < abs(magnum->sign_n_prec)-down-up; i++){
            new_adress[i] = magnum->value[i+down];
        }
        
        if (magnum->sign_n_prec<0)
            magnum->sign_n_prec = -(abs(magnum->sign_n_prec)-up-down);
        else
            magnum->sign_n_prec = (abs(magnum->sign_n_prec)-up-down);
        magnum->power+=up;
        free(magnum->value);
        magnum->value = new_adress;
    }
}


void free_magnum(struct magnum * magnum){
    free(magnum->value);
    free(magnum);
}


void print_magnum_info(struct magnum * magnum){
    afficher_tableau(magnum->value, abs(magnum->sign_n_prec));
    printf("precision : %d\n", abs(magnum->sign_n_prec));
    printf("power : %d\n", magnum->power);
    printf("sign : %d\n", magnum->sign_n_prec<0);
}