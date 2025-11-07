#ifndef _ADDITION_
#define _ADDITION_
void incremental(struct magnum *magnum, unsigned int n);
void _carry(struct magnum *magnum, int n, int i);
void _level(struct magnum *mag1, struct magnum *mag2);
void add(struct magnum *mag1, struct magnum *mag2);
struct magnum *sum(struct magnum *mag1, struct magnum *mag2);
void opposite(struct magnum *magnum);
#endif