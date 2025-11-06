#include <stdlib.h>
#include <stdint.h>

//faire passer le magnum plutôt que juste la value ?
//bits are numeroted that way :
//[012345678 , 9..., 17..., ...]

uint8_t _get_bit_magnum(uint8_t* m_value, int i){
    // get the i-th bit of an uint8 array
    if (m_value == NULL)
        abort;

    return (m_value[i/8] & (1<<(7-i%8))) >> (7-i%8) & 1;
}


void _set_bit_magnum(uint8_t* m_value, int i, int new_val){
    // set the i-th bit of an uint8 array with new_val
    // new_val have to be 0 or 1
    if (m_value == NULL)
        abort;

    m_value[i/8] = (m_value[i/8] & ~(1<<(7-(i%8)))) + (new_val<<(7-(i%8)));
}