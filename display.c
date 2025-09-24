#include <stdio.h>
#include <stdint.h>

void afficher (int n){ //voir ex 17 tp2
    int i = 0;
    // while (n>0){
    //     n=n<<1;
    //     i++;
    // }
    while (i<32){ 
        printf("%d", n<0);
        n=n<<1;
        i++;
    }
    printf("\n");
}

void afficher_tableau(uint8_t * t, int taille){ //voir ex 36 tp5
    printf("[");
    for (int i = 0; i < taille-1; i++){
        printf("%d, ",t[i]);
    }
    printf("%d]\n",t[taille-1]);
}
