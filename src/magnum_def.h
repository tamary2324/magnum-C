#ifndef _MAGNUM_DEF_
#define _MAGNUM_DEF_
struct magnum{
    int16_t sign_n_prec;
    int16_t power;
    uint8_t * value;
};
struct magnum *_new_magnum(void);
void _init_magnum(struct magnum *magnum);
void _change_prec(struct magnum *magnum, int n);
void _decrease_power(struct magnum *magnum, int n);
void _clean_magnum(struct magnum *magnum);
void copy(struct magnum *new_magnum, struct magnum *magnum);
void free_magnum(struct magnum *magnum);
void print_magnum_info(struct magnum *magnum);
#endif