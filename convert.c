#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "magnum_def.h"


struct magnum * to_magnum_from_int(int a){
    //get an int and return the adress of the corresponding magnum structure allocated on the stack
    struct magnum * mag1 = (struct magnum*) malloc(sizeof(struct magnum));
    int precision=0;
    while (((abs(a)>>(precision*8))>0)&&(precision<4)){ //get needed precision
        precision++;
    }

    if (a<0)
        mag1->sign_n_prec = -precision;
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
        value=-value;

    return value;
}


struct magnum * to_magnum_from_double(double a){ 
    //get an double and return the adress of the corresponding magnum structure allocated on the stack
    // ! the argument have to be a double !
    int64_t a_as_int = *(int64_t*)&a;
    int64_t fl_power = ((int16_t)((a_as_int>>(int64_t)52)&((int64_t)2047))-(int16_t)1075);
    int64_t power_correction = (fl_power%8+8)%8;    //get positive modulo
    int64_t precision=7;
    struct magnum * mag1 = (struct magnum*) malloc(sizeof(struct magnum));

    mag1->power = fl_power/8;
    if (fl_power<0&&power_correction<4)
        mag1->power--;

    if (power_correction>=4){
        precision=8;
        power_correction-=8;
        mag1->power--;
    }
    if (a_as_int<0)
        mag1->sign_n_prec = -precision;
    else
        mag1->sign_n_prec = precision;

    mag1->value = (uint8_t *) malloc(abs(mag1->sign_n_prec)*sizeof(uint8_t));

    int64_t fraction_part_mask = ~((int64_t)4095<<(int64_t)52);
    for (int64_t i = 0; i < precision; i++){ 
        //copy the fraction part of the double into magnum->value
        //apply power correction to match the correct double power 
        //(power is by octet in magnum while it's by bit in double)
        if (power_correction>0)
            mag1->value[i]=(((a_as_int&fraction_part_mask)<<power_correction)>>
            (((precision-i-(int64_t)1)*(int64_t)8)))%256;    
        else
            mag1->value[i]=(((a_as_int&fraction_part_mask))>>
            (((precision-i-(int64_t)1)*(int64_t)8)-power_correction))%256;
    }

    if (fl_power!=-1023){   //correct mantissa
        if (power_correction>0)
            mag1->value[0]+=16<<power_correction;
        else
            mag1->value[0]+=16>>(-power_correction);
    }
    
    clean_magnum(mag1);
    return mag1;
}

