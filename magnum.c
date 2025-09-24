#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "display.h" //gcc magnum.c display.c


int get_precision(uint8_t * at){    
    //get the magnum adress and return its precision value

    return (((at[0])&(~(1<<7))<<8)+at[1]);
}



int get_power(uint8_t * at){        
    //get the magnum adress and return its signed power value

    return (((at[2])&(~(1<<7))<<8)+at[3])^(((at[2])&(1<<7))<<24);
}



int get_sign(uint8_t * at){         
    //get the magnum adress and return its sign

    return ((at[0])&(1<<7))>>7;
}



uint8_t * to_magnum_from_int(int a){
    //get an int and return the adress of the corresponding magnum

    int precision=0;
    int valeur_absolue =a;
    if (a<0)
        valeur_absolue=~a+1;

    while (((valeur_absolue>>(precision*8))>0)&&(precision<4)){ //get needed precision
        precision++;
    }

    uint8_t *at = (uint8_t*) malloc((precision+4)*sizeof(uint8_t));

    at[0]=((a<0)<<7)+(precision/256);   //sign of the number and strong bits for precision
    at[1]=precision%256;                //weak bits for precision
    at[2]=0;                            //sign and strong bits for power
    at[3]=0;                            //weak bits for power

    for (int i = 0; i < precision; i++){
        at[i+4]=(valeur_absolue>>((precision-i-1)*8))%256;  //copy the int value into a magnum
    }

    return at;
}



int from_magnum_to_int(uint8_t * at){
    //get an magnum adress and return the corresponding int 
    //if it's not possible it return 0 and a consol message

    int value=0;
    int precision=get_precision(at);
    int power=get_power(at);

    if (power+precision>4){
        printf("the number is too big to be converted in integer\n");
        return 0;
    }
    if (power<0){
        printf("the number is too small to be converted in integer\n");
        return 0;
    }

    for (int i = 0; i < precision; i++){
        value+=at[precision-i+3]<<8*(i+power);
    }

    if (get_sign(at))
        value=~value+1;

    return value;
}


int main(void){
    
    uint8_t * at =to_magnum_from_int(-2147483647);

    afficher_tableau(at, (((at[0])&(~(1<<7))<<8)+at[1]) + 4);
    printf("precision : %d\n", get_precision(at));
    printf("power : %d\n", get_power(at));
    printf("sign : %d\n", get_sign(at));
    printf("a = %d", from_magnum_to_int(at));

    free(at);
    return 0;
}
