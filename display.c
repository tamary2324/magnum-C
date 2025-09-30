#include <stdio.h>
#include <stdint.h>

void afficher (int n){ //voir ex 17 tp2
    //get an integer and print its full binary representation 
    //remove the following comment if you wanna have only the useful part of the number
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

void afficher_float(double n){//voir ex 47 tp6
    //get a double and print the number, markers to differenciate sign, power and fraction part
    //then print the binary representation of the number
    printf("fl = %f\n",n);
    int64_t fl = *(int64_t*)&n;
    printf("spppppppppppffffffffffffffffffffffffffffffffffffffffffffffffffff\n");
    int i = 0;
    int64_t un = 1;
    while (i<64){ 
        printf("%d", fl<0);
        fl=fl<<un;
        i++;
    }
    printf("\n");
}

void afficher_tableau(uint8_t * t, int taille){ //voir ex 36 tp5
    //get the array adress and lenght and print it readibly
    printf("[");
    for (int i = 0; i < taille-1; i++){
        printf("%d, ",t[i]);
    }
    printf("%d]\n",t[taille-1]);
}

void afficher_matrice(int ** matrice,int n, int m){
    //get a two dimmensionnal array and its size and print it readibly
    //n is for the array of pointer and m for every other arrays
    for (int i = 0; i < n; i++){
        printf("[");
        for (int j = 0; j < m-1; j++){
            printf("%d, ",matrice[i][j]);
        }
        printf("%d]\n",matrice[i][m-1]);
    }
}