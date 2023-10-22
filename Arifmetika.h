#pragma once
#include <stdbool.h>
typedef struct {
    char* digits;
    int length;
    bool is_negative;
    char trash[3];
} big_int;
void free_big_int(big_int* number); // очистка памяти
void remove_leading_zeros(big_int* number); // удаление нулей
big_int* init_big_int_zero(int length); // инициализация нуля
int abs_compare(big_int* a, big_int* b); // сравнение по модулю
big_int* add(big_int* a, big_int* b); // сложение
big_int* subtract(big_int* a, big_int* b); // вычитаение
big_int* multiply(big_int* a, big_int* b); // умножение
big_int* init_big_int(const char* str); // инициализация строки в число
big_int* int_to_big(int a); // число в большое число

