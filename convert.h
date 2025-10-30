#ifndef _CONVERT_
#define _CONVERT_
struct magnum * to_magnum_from_int(int a);
int from_magnum_to_int(struct magnum * magnum);
struct magnum * to_magnum_from_double(double  a);
double from_magnum_to_double(struct magnum * magnum);
#endif