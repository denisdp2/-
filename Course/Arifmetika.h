#pragma once
#include <stdbool.h>
typedef struct {
    char* digits;
    int length;
    bool is_negative;
    char trash[3];
} big_int;
void free_big_int(big_int* number);
void remove_leading_zeros(big_int* number);
big_int* init_big_int_zero(int length);
int abs_compare(big_int* a, big_int* b);
big_int* add(big_int* a, big_int* b);
big_int* subtract(big_int* a, big_int* b);
big_int* multiply(big_int* a, big_int* b);
big_int* init_big_int(const char* str);

big_int* int_to_big(int a);