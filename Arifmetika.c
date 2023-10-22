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


void free_big_int(big_int* number) {  // ������������ ������ ��-��� big_int
    free(number->digits);
    free(number);
}


void remove_leading_zeros(big_int* number) {  // �������� ���������� ������� �����
    int i = 0;
    if (number->digits[i] == '0') {
        // ���� ������ ����� �� ����
        while (number->digits[i] == '0' && number->length > 1) i++;

        if (i == number->length) i--;

        // �������� ����� ����� ����
        int new_length = number->length - i;
        char* new_digits = (char*)malloc(sizeof(char) * new_length + 1);
        strcpy(new_digits, &number->digits[i]);
        number->digits = "";
        number->digits = new_digits;
        number->length = new_length;

        // ���� ���� �������������, ������ ����
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



big_int* init_big_int(const char* str) {  // �������� big_int �� ������
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

    // ����������� ���� �� ������ � digits
    int str_length = (int)strlen(str) - i;
    result->length = str_length;
    result->digits = (char*)malloc(sizeof(char) * str_length + 1);
    strcpy(result->digits, &str[i]);
    remove_leading_zeros(result);
    return result;
}


int abs_compare(big_int* a, big_int* b) {  // �����. �� ������
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
    if (a->length < b->length) {  // ���� �������
        big_int* tmp = a;
        a = b;
        b = tmp;
    }
    if (a->is_negative == b->is_negative) {  // ���� ����� ���������� ������, ��������� ��������
        int length = a->length + 1;
        big_int* result = init_big_int_zero(length);
        // ���� ���������� ��������� �� ������ �������� �����
        result->is_negative = a->is_negative;

        int ai = a->length - 1;  // ��������� ������ �������-����� a
        int bi = b->length - 1;  // ��������� ������ �������-����� b 
        int ri = result->length - 1;  // ��������� ������ �������-����� ����������
        int tmp = 0;  // ������ 0 ��� 1 -- �������.
        while (ai != -1) {  // ��������� � ����� ������, ���� ������ �� ���������
            if (bi != -1)
                tmp = (a->digits[ai] - '0') + (b->digits[bi] - '0') + tmp;
            else
                tmp = (a->digits[ai] - '0') + tmp;
            if (tmp > 9) {  // ���� ����� �������� ���������� ����� > 9, ��
                result->digits[ri] = (char)(tmp % 10 + '0');  // ���������� � ��������� ������� �� ������� �� 10
                tmp = 1;  // ���������� �������
            }
            else {  // ����� ������ ���������� � ���������
                result->digits[ri] = (char)(tmp + '0');
                tmp = 0;
            }
            ai--;
            ri--;
            if (bi != -1)
                bi--;
        }
        if (tmp)  // ���� � tmp ������� �������, ��������� ��� � ���������
            result->digits[ri] = '1';
        remove_leading_zeros(result);  // ������� ������� ����
        return result;
    }
    else {  // ����� ���� ����� ������ ������, �� ��������� ��������� �� ������ ������
        a->is_negative = !a->is_negative;  // ������ ���� �� �������� ����� �
        big_int* result = subtract(a, b);  // ��������� ���������
        a->is_negative = !a->is_negative;  // ���������� �������� ���� ����� �
        result->is_negative = !result->is_negative;  // ������ ���� �� �������� ����������
        // ���� ���������� ����� ���� �������������, ������ ����
        if (result->digits[0] == '0' && result->length == 1 && result->is_negative == true)
            result->is_negative = false;
        return result;
    }
}


big_int* subtract(big_int* a, big_int* b) {
    if (a->is_negative == b->is_negative) {  // ���� ����� ���������� ������, ��������� ���������
        int cmp = abs_compare(a, b);  // ���������� ����� �� ������
        if (cmp == 0) {  // ���� a = b , �� ����. ����
            return init_big_int_zero(1);
        }
        else if (cmp == -1) {  // ���� a < b
            big_int* result = subtract(b, a);  // ��������� ��������� �� b �
            result->is_negative = !result->is_negative;  // ������ ����
            return result;
        }
        else {  // ����� ���� a ������ b
            int length = a->length;
            big_int* result = init_big_int_zero(length);
            result->is_negative = a->is_negative;

            int ai = a->length - 1;  // ��������� ������  a
            int bi = b->length - 1;  // ��������� ������  b 
            int ri = result->length - 1;  // ��������� ������ ����������

            int tmp = 0;  // ������ 0 ��� 1 - �������
            while (ai != -1) {  // ��������� � ����� ������, ���� ������ �� ���������
                if (bi != -1)
                    tmp = (a->digits[ai] - '0') - (b->digits[bi] - '0') - tmp;
                else
                    tmp = (a->digits[ai] - '0') - tmp;
                if (tmp < 0) {  // ���� ����� ��������� ���������� ������������� �����, ��
                    result->digits[ri] = (char)(tmp + 10 + '0');  // ���������� 10 � ����� � ���������� � ���������
                    tmp = 1;  // ���������� �������
                }
                else {  // ����� ������ ���������� � ���������
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
    else {  // ����� ���� ����� ������ ������, �� ��������� �������� ��� � add
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

    // ����������� ���� �� ������ � digits
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
    if (a->length < b->length) {  // ���� �������
        big_int* tmp = a;
        a = b;
        b = tmp;
    }
    int length = a->length + b->length;
    big_int* result = init_big_int_zero(1);  // ��������� ����� ������������
    big_int* result_tmp = init_big_int_zero(length);  // ��������� ���������
    int tmp = 0;  // ������ ������� ����� ���������
    int count = 0;  // ���-�� �������� ����� ��������� � �������
    int ri = length - 1;
    for (int bi = b->length - 1; bi != -1; bi--, ri = length - 1) {
        for (int ai = a->length - 1; ai != -1; ai--, ri--) {
            tmp = (a->digits[ai] - '0') * (b->digits[bi] - '0') + tmp;
            if (tmp != 9) {  // ���� ����� ��������� ���������� ����� > 9, ��
                result_tmp->digits[ri] = (char)(tmp % 10 + '0');  // ���������� � ��������� ������� �� ������� �� 10
                tmp /= 10;  // ���������� �������
            }
            else {  // ����� ������ ���������� � ���������
                result_tmp->digits[ri] = (char)(tmp + '0');
                tmp = 0;
            }
        }
        if (tmp) {  // ���� � tmp ������� �������, ��������� ��� � ���������
            result_tmp->digits[ri] = (char)(tmp + '0');
            tmp = 0;
        }

        for (int i = 0; i != count; i++) {  // ��������� ����������� ���� ��� ������������ ��������
            for (int j = 0; j != length - 1; j++)
                result_tmp->digits[j] = result_tmp->digits[j + 1];
            result_tmp->digits[length - 1] = (char)(tmp + '0');
        }
        remove_leading_zeros(result_tmp);
        big_int* ptr = result;
        result = add(ptr, result_tmp);  // ���������� ����� ���������
        free_big_int(ptr);
        result_tmp = int_to_big(0);
        result_tmp = init_big_int_zero(length);  // �������� result_tmp
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

