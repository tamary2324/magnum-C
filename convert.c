#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "magnum_def.h"
#include "bit_op.h"


struct magnum * to_magnum_from_int(int a){
    //get an int and return the adress of the corresponding magnum structure allocated on the stack
    struct magnum * mag1 = _new_magnum();
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

    _clean_magnum(mag1);
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
    struct magnum * mag1 = _new_magnum();

    if (a==0.){
        mag1->power = 0;
        mag1->sign_n_prec = 0;
        mag1->value = (uint8_t *) malloc(sizeof(uint8_t));
        mag1->value[0] = 0;
        return mag1;
    }

    if (a<0.)
        mag1->sign_n_prec = -7;
    else
        mag1->sign_n_prec = 7;

    mag1->value = (uint8_t *) malloc(abs(mag1->sign_n_prec)*sizeof(uint8_t));
    for (int i = 0; i < 7; i++){
        mag1->value[i] = 0;
    }

    int64_t a_as_int = *(int64_t*)&a;
    int64_t a_power = ((a_as_int >> (int64_t)52) & (int64_t)2047) - 1023;
    
    if (a_power != -1023){
        mag1->power = a_power / 8 - 6;
        int power_correction = (a_power % 8);

        if (power_correction<0){ //normalize correction
            power_correction = 8+power_correction;
            mag1->power--;
        }
        
        int64_t a_value = ~((int64_t)4095 << (int64_t)52) & a_as_int;
        a_value += (int64_t)1 << (int64_t)52; // correct mantissa
        
        for (int i = 0; i < 53; i++){
            _set_bit_magnum(mag1->value, 59 - i - power_correction, (a_value >> (int64_t)(i)) & 1);
        }
    }

    else {
        int64_t decalage = 0;
        while (a_as_int >> ((int64_t)51 - decalage) == 0){
            decalage++;
        }
        
        a_power -= decalage;
        mag1->power = a_power / 8 - 6;

        int power_correction = (a_power % 8);
        if (power_correction<0){ //normalize correction
            power_correction = 8+power_correction;
            mag1->power--;
        }

        int64_t a_value = ~((int64_t)4095 << (int64_t)52) & a_as_int;
        for (int i = 0; i < 52-decalage; i++){
            _set_bit_magnum(mag1->value, 58 - i - power_correction - decalage, (a_value >> (int64_t)(i)) & 1);
        }
    }

    _clean_magnum(mag1);
    return mag1;
}


double from_magnum_to_double(struct magnum * magnum){
    //get a magnum structure adress and return the corresponding double 
    //if it's not possible it return 0 and a consol message

    if (magnum->value[0]==0)
        _clean_magnum(magnum);

    
    int power_correction = 0;
    while (_get_bit_magnum(magnum->value,power_correction)!=1){
        power_correction++;
    }

    int64_t value = 0;

    int double_power = (magnum->power + abs(magnum->sign_n_prec)) * 8 - power_correction + 1022;

    if (double_power>=2047){
        printf("the number is too big to be converted in float\n");
        return 0.;
    }
    if (double_power<-51){
        printf("the number is too small to be converted in float\n");
        return 0.;
    }

    if (double_power>0){
        power_correction++; //skip mantissa
        value += ((int64_t)double_power << (int64_t)52);
        for (int i = 0; i < 52; i++){
            if (power_correction + i >= abs(magnum->sign_n_prec) * 8)
                break;

            value += (int64_t)_get_bit_magnum(magnum->value, i + power_correction) << (int64_t)(51-i);
        }
    }
    else{
        for (int i = 0; i < 52 + double_power; i++){
            if (power_correction + i >= abs(magnum->sign_n_prec) * 8)
                break;

            value+=(int64_t)_get_bit_magnum(magnum->value, i+power_correction) << (int64_t)(51-i+double_power);
        }
    }

    double value_as_double = *((double *)&value);

    if (magnum->sign_n_prec<0)
        value_as_double=-value_as_double;
    
    // display_double(value_as_double);
    // printf("value_as_double = %f\n", value_as_double);

    return value_as_double;
}