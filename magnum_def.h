#ifndef _MAGNUM_DEF_
#define _MAGNUM_DEF_
struct magnum{
    int16_t sign_n_prec;
    int16_t power;
    int8_t * value;
};
void clean_magnum(struct magnum * magnum);
void free_magnum(struct magnum * magnum);
void print_magnum_info(struct magnum * magnum);
#endif