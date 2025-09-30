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
            magnum->sign_n_prec = ~(abs(magnum->sign_n_prec)-up-down)+1;
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


struct magnum * to_magnum_from_double(double  a){ 
    //get an double and return the adress of the corresponding magnum structure allocated on the stack
    // ! the argument have to be a double !
    int64_t fl = *(int64_t*)&a;
    int64_t fl_power = ((int16_t)((fl>>(int64_t)52)&((int64_t)2047))-(int16_t)1075);
    int64_t power_corection = (fl_power%8+8)%8;
    int64_t precision=7;
    struct magnum * mag1 = (struct magnum*) malloc(sizeof(struct magnum));

    mag1->power = fl_power/8;
    if (fl_power<0&&power_corection<4)
        mag1->power--;

    if (power_corection>=4){
        precision=8;
        power_corection=power_corection-8;
        mag1->power--;
    }
    if (fl<0)
        mag1->sign_n_prec = ~precision+1;
    else
        mag1->sign_n_prec = precision;

    mag1->value = (uint8_t *) malloc(abs(mag1->sign_n_prec)*sizeof(uint8_t));

    for (int64_t i = 0; i < precision; i++){
        if (power_corection>0)
            mag1->value[i]=(((fl&~((int64_t)4095<<(int64_t)52))<<power_corection)>>   //put a mask to get only the fraction of the double
            (((precision-i-(int64_t)1)*(int64_t)8)))%256;     //cut the fraction in piece for it to be copy into the magnum
        else
            mag1->value[i]=(((fl&~((int64_t)4095<<(int64_t)52)))>>
            (((precision-i-(int64_t)1)*(int64_t)8)-power_corection))%256;
    }
    if (fl_power!=-1023){   //correct mantissa
        if (power_corection>0)
            mag1->value[0]+=16<<power_corection;
        else
            mag1->value[0]+=16>>(~power_corection+1);
    }
    clean_magnum(mag1);
    return mag1;
}




int main(void){
    // int8_t t[1] = {15};
    // struct magnum mag1 = {156,0,t};
    // struct magnum * ptr_mag1 = &mag1;
    
    struct magnum * at =to_magnum_from_double(2.);
    print_magnum_info(at);
    // printf("a = %d", from_magnum_to_int(at));

    free_magnum(at);
    return 0;
}