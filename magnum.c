#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "display.h" //gcc magnum.c display.c

struct magnum{
    int16_t sign_n_prec;//signed int 16 
    //abs(sign_n_prec) giving the precision 
    //(sign_n_prec<0) giving the sign of the magnum 
    int16_t power;//signed power value
    int8_t * value;//adress of the magnum array on the stack
};



struct magnum * to_magnum_from_int(int a){
    //get an int and return the adress of the corresponding magnum structure allocated on the stack
    struct magnum * mag1 = (struct magnum*) malloc(sizeof(struct magnum));
    int precision=0;
    while (((abs(a)>>(precision*8))>0)&&(precision<4)){ //get needed precision
        precision++;
    }

    if (a<0)
        mag1->sign_n_prec = ~precision+1;
    else
        mag1->sign_n_prec = precision;
    
    mag1->power = 0;
    mag1->value = (uint8_t *) malloc(abs(mag1->sign_n_prec)*sizeof(uint8_t));

    for (int i = 0; i < precision; i++){
        mag1->value[i]=(abs(a)>>((precision-i-1)*8))%256;  //copy the int value into a magnum value
    }

    return mag1;
}



int from_magnum_to_int(struct magnum * magnum){
    //get a magnum structure adress and return the corresponding int 
    //if it's not possible it return 0 and a consol message
    int64_t value = 0;

    if (magnum->power+abs(magnum->sign_n_prec)>4){
        printf("the number is too big to be converted in integer\n");
        return 0;
    }
    if (magnum->power<0){
        printf("the number is too small to be converted in integer\n");
        return 0;
    }

    for (int i = 0; i < abs(magnum->sign_n_prec); i++){
        value+=magnum->value[abs(magnum->sign_n_prec)-i-1]<<8*(i+magnum->power);
    }

    if (magnum->sign_n_prec<0)
        value=~value+1;

    return value;
}

void free_magnum(struct magnum * magnum){
    free(magnum->value);
    free(magnum);
}



int main(void){
    // int8_t t[1] = {15};
    // struct magnum mag1 = {156,0,t};
    // struct magnum * ptr_mag1 = &mag1;
    
    struct magnum * at =to_magnum_from_int(-549);

    afficher_tableau(at->value, abs(at->sign_n_prec));
    printf("precision : %d\n", abs(at->sign_n_prec));
    printf("power : %d\n", at->power);
    printf("sign : %d\n", at->sign_n_prec<0);
    printf("a = %d", from_magnum_to_int(at));

    free_magnum(at);
    return 0;
}