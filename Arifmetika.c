#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>



typedef struct {
    char* digits;
    int length;
    bool is_negative;
    char trash[3];
} big_int;


big_int* subtract(big_int* a, big_int* b);


void free_big_int(big_int* number) {  // освобождение памяти из-под big_int
    free(number->digits);
    free(number);
}


void remove_leading_zeros(big_int* number) {  // удаление незначащих ведущих нулей
    int i = 0;
    if (number->digits[i] == '0') {
        // ищем первую цифру не ноль
        while (number->digits[i] == '0' && number->length > 1) i++;

        if (i == number->length) i--;

        // копируем цифры после нуля
        int new_length = number->length - i;
        char* new_digits = (char*)malloc(sizeof(char) * new_length + 1);
        strcpy(new_digits, &number->digits[i]);
        number->digits = "";
        number->digits = new_digits;
        number->length = new_length;

        // если ноль отрицательный, меняем знак
        if (number->digits[0] == '0' && number->length == 1 && number->is_negative == true)
            number->is_negative = false;
    }
}


big_int* init_big_int_zero(int length) {
    big_int* result = (big_int*)malloc(sizeof(big_int));
    result->is_negative = false;
    result->length = length;
    result->digits = (char*)malloc(sizeof(char) * length + 1);
    for (int i = 0; i != length; i++) {
        result->digits[i] = '0';
    }
    result->digits[length] = '\0';

    return result;
}



big_int* init_big_int(const char* str) {  // создание big_int из строки
    big_int* result = (big_int*)malloc(sizeof(big_int));
    int i;
    if (str[0] == '-') {
        result->is_negative = true;
        i = 1;
    }
    else {
        result->is_negative = false;
        i = 0;
    }

    // копирование цифр из строки в digits
    int str_length = (int)strlen(str) - i;
    result->length = str_length;
    result->digits = (char*)malloc(sizeof(char) * str_length + 1);
    strcpy(result->digits, &str[i]);
    remove_leading_zeros(result);
    return result;
}


int abs_compare(big_int* a, big_int* b) {  // сравн. по модулю
    if (a->length > b->length)
        return 1;
    else if (a->length < b->length)
        return -1;
    else {
        int i;
        for (i = 0; i < a->length; i++) {
            if (a->digits[i] > b->digits[i])
                return 1;
            else if (a->digits[i] < b->digits[i])
                return -1;
        }
        return 0;
    }
}


big_int* add(big_int* a, big_int* b) {
    if (a->length < b->length) {  // свап местами
        big_int* tmp = a;
        a = b;
        b = tmp;
    }
    if (a->is_negative == b->is_negative) {  // если числа одинаковых знаков, выполняем сложение
        int length = a->length + 1;
        big_int* result = init_big_int_zero(length);
        // знак результата совпадает со знаком исходных чисел
        result->is_negative = a->is_negative;

        int ai = a->length - 1;  // последний индекс символа-числа a
        int bi = b->length - 1;  // последний индекс символа-числа b 
        int ri = result->length - 1;  // последний индекс символа-числа результата
        int tmp = 0;  // храним 0 или 1 -- десяток.
        while (ai != -1) {  // вычисляем с конца строки, пока строка не закончена
            if (bi != -1)
                tmp = (a->digits[ai] - '0') + (b->digits[bi] - '0') + tmp;
            else
                tmp = (a->digits[ai] - '0') + tmp;
            if (tmp > 9) {  // если после сложения получилось число > 9, то
                result->digits[ri] = (char)(tmp % 10 + '0');  // записываем в результат остаток от деления на 10
                tmp = 1;  // запоминаем десяток
            }
            else {  // иначе просто записываем в результат
                result->digits[ri] = (char)(tmp + '0');
                tmp = 0;
            }
            ai--;
            ri--;
            if (bi != -1)
                bi--;
        }
        if (tmp)  // если в tmp остался десяток, добавляем его в результат
            result->digits[ri] = '1';
        remove_leading_zeros(result);  // убираем ведущие нули
        return result;
    }
    else {  // иначе если числа разных знаков, то выполняем вычитание из одного другое
        a->is_negative = !a->is_negative;  // меняем знак на обратный числу а
        big_int* result = subtract(a, b);  // выполняем вычитание
        a->is_negative = !a->is_negative;  // возвращаем исходный знак числу а
        result->is_negative = !result->is_negative;  // меняем знак на обратный результату
        // если получилось число ноль отрицательным, меняем знак
        if (result->digits[0] == '0' && result->length == 1 && result->is_negative == true)
            result->is_negative = false;
        return result;
    }
}


big_int* subtract(big_int* a, big_int* b) {
    if (a->is_negative == b->is_negative) {  // если числа одинаковых знаков, выполняем вычитание
        int cmp = abs_compare(a, b);  // сравниваем числа по модулю
        if (cmp == 0) {  // если a = b , то возв. ноль
            return init_big_int_zero(1);
        }
        else if (cmp == -1) {  // если a < b
            big_int* result = subtract(b, a);  // выполняем вычитание из b а
            result->is_negative = !result->is_negative;  // меняем знак
            return result;
        }
        else {  // иначе если a больше b
            int length = a->length;
            big_int* result = init_big_int_zero(length);
            result->is_negative = a->is_negative;

            int ai = a->length - 1;  // последний индекс  a
            int bi = b->length - 1;  // последний индекс  b 
            int ri = result->length - 1;  // последний индекс результата

            int tmp = 0;  // храним 0 или 1 - десяток
            while (ai != -1) {  // вычисляем с конца строки, пока строка не закончена
                if (bi != -1)
                    tmp = (a->digits[ai] - '0') - (b->digits[bi] - '0') - tmp;
                else
                    tmp = (a->digits[ai] - '0') - tmp;
                if (tmp < 0) {  // если после вычитания получилось отрицательное число, то
                    result->digits[ri] = (char)(tmp + 10 + '0');  // прибавляем 10 к числу и записываем в результат
                    tmp = 1;  // заимствуем десяток
                }
                else {  // иначе просто записываем в результат
                    result->digits[ri] = (char)(tmp + '0');
                    tmp = 0;
                }
                ai--;
                ri--;
                if (bi != -1)
                    bi--;
            }
            remove_leading_zeros(result);
            return result;
        }
    }
    else {  // иначе если числа разных знаков, то выполняем сложение как в add
        a->is_negative = !a->is_negative;
        big_int* result = add(a, b);
        a->is_negative = !a->is_negative;
        result->is_negative = !result->is_negative;
        if (result->digits[0] == '0' && result->length == 1 && result->is_negative == true)
            result->is_negative = false;
        return result;
    }
}


big_int* int_to_big(int a)
{
    char* str = calloc(1500, 1500 * sizeof(char));
    sprintf(str, "%d", a);
    big_int* result = init_big_int("0");
    int i;
    if (str[0] == '-') {
        result->is_negative = true;
        i = 1;
    }
    else {
        result->is_negative = false;
        i = 0;
    }

    // копирование цифр из строки в digits
    int str_length = (int)strlen(str) - i;
    result->length = str_length;
    strcpy(result->digits, &str[i]);
    remove_leading_zeros(result);
    free(str);
    return result;
}


big_int* multiply(big_int* a, big_int* b) {
    if (a->length == 1 && a->digits[0] == '0' ||
        b->length == 1 && b->digits[0] == '0') {
        return init_big_int_zero(1);
    }
    if (a->length < b->length) {  // свап местами
        big_int* tmp = a;
        a = b;
        b = tmp;
    }
    int length = a->length + b->length;
    big_int* result = init_big_int_zero(1);  // результат после суммирования
    big_int* result_tmp = init_big_int_zero(length);  // результат умножения
    int tmp = 0;  // храним десятки после умножения
    int count = 0;  // кол-во сложений после умножения в столбик
    int ri = length - 1;
    for (int bi = b->length - 1; bi != -1; bi--, ri = length - 1) {
        for (int ai = a->length - 1; ai != -1; ai--, ri--) {
            tmp = (a->digits[ai] - '0') * (b->digits[bi] - '0') + tmp;
            if (tmp != 9) {  // если после умножения получилось число > 9, то
                result_tmp->digits[ri] = (char)(tmp % 10 + '0');  // записываем в результат остаток от деления на 10
                tmp /= 10;  // запоминаем десятки
            }
            else {  // иначе просто записываем в результат
                result_tmp->digits[ri] = (char)(tmp + '0');
                tmp = 0;
            }
        }
        if (tmp) {  // если в tmp остался десяток, добавляем его в результат
            result_tmp->digits[ri] = (char)(tmp + '0');
            tmp = 0;
        }

        for (int i = 0; i != count; i++) {  // добавляем необходимые нули для последующего сложения
            for (int j = 0; j != length - 1; j++)
                result_tmp->digits[j] = result_tmp->digits[j + 1];
            result_tmp->digits[length - 1] = (char)(tmp + '0');
        }
        remove_leading_zeros(result_tmp);
        big_int* ptr = result;
        result = add(ptr, result_tmp);  // складываем после умножения
        free_big_int(ptr);
        result_tmp = int_to_big(0);
        result_tmp = init_big_int_zero(length);  // обнуляем result_tmp
        count++;
    }
    if (a->is_negative == b->is_negative) {
        result->is_negative = false;
    }
    else {
        result->is_negative = true;
    }
    free_big_int(result_tmp);
    return result;
}

