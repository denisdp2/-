#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include "Arifmetika.h"
#include "gmp.h"

#define SIZEOF_COMMAND 63
#define RSA 1
#define Gamal 2
#define MAX_SIZE_PRIME_NUM 1300



int* check_null(void* ptr) {
	if (ptr == NULL) {
		wprintf_s(L"Ошибка выделения памяти");
		exit(EXIT_FAILURE);
	}
	return ptr;
}

//int is_prime(int num) {
//	if (num <= 1) {
//		return 0;
//	}
//	for (int i = 2; i <= sqrt(num); i++) {
//		if (num % i == 0) {
//			return 0;
//		}
//	}
//	return 1;
//}
//
//int is_prime_miler(big_int* n) {
//	if (abs_compare(n, int_to_big(1)) < 0) return 0;
//	if (abs_compare(n, int_to_big(3)) < 0) return 1;
//	if (int_to_big(divide_remainder(n, int_to_big(2)))->digits[0] == "0" || (int_to_big(divide_remainder(n, int_to_big(3)))->digits[0] == "0")) return 0;
//	for (unsigned long long i = 5; abs_compare(int_to_big(i * i), n) > 0; i += 6) {
//		if (int_to_big(divide_remainder(n, int_to_big(i)))->digits[0] == "0" || int_to_big(divide_remainder(n,(add(int_to_big(i), int_to_big(2)))))->digits[0] == "0") {
//			return 0;
//		}
//	}
//	return 1;
//}


void help(void) {
	wprintf_s(L"Генерирует ключи. Открытый сохраняется в \
pk.txt, закрытый – в secret.txt. Размер может быть любой, \
кратный 256: 256, 512, 1024, 2048, 4096 и т.д: crypt genkey \
--size 1024 --pubkey pk.txt --secret secret.txt\n\
Вычислят подпись файла, сохраняет ее в signature.txt: crypt sign --infile file.txt --secret secret.txt --sigfile signature.txt\n\
Проверяет подпись файла, результат пишет в stdout: crypt check --infile file.txt --pubkey pk.txt --sigfile signature.txt\n\
Шифрует файл для получателя. Результат помещает в file.txt.enc: crypt encrypt --infile file.txt --pubkey pk.txt --outfile file.txt.enc \n\
Расшифровывает файл. Результат помещает в file.txt: crypt decrypt --infile file.txt.enc --secret secret.txt --outfile file.txt\n");
}

big_int* powek(big_int* base, big_int* exponent) {  // возведение в степень
	if (base->length == 1 && base->digits[0] == '0')  // если base == 0, возвращаем 0
		return int_to_big(0);
	if (base->length == 1 && base->digits[0] == '1' ||
		exponent->length == 1 && exponent->digits[0] == '0') // если base == 1 или exp == 0, возвращаем 1
		return init_big_int("1");
	big_int* result = init_big_int("1");
	big_int* count = init_big_int("1");  // счетчик от 1 до exponent
	big_int* _1 = init_big_int("1");
	while (abs_compare(count, exponent) < 1) {
		big_int* tmp = result;
		result = multiply(result, base);
		free_big_int(tmp);

		/*big_int* tmp2 = count;*/
		count = add(count, _1);
		/*free_big_int(tmp2);*/
	}
	free_big_int(count);
	free_big_int(_1);
	return result;
}


void digit_shift(big_int* n, int d)		/* multiply n by 10^d */
{
	int i;				/* counter */

	if ((n->length == 0) && (n->digits[0] == '0')) return;

	for (i = n->length; i >= 0; i--)
		n->digits[i + d] = n->digits[i];

	for (i = 0; i < d; i++) n->digits[i] = '0';

	n->length = n->length + d;
}


big_int* divide(big_int* num1, big_int* num2) {
	int temp2 = 0;
	big_int* temp1 = int_to_big(0);
	big_int* temp = int_to_big(0);
	big_int* result = int_to_big(0);
	int k = num1->length - num2->length;
	int k_len = k;
	int m = num1->length;
	int n = num2->length;
	if (abs_compare(num2, num1) == 1)
		return int_to_big(0);
	else if (abs_compare(num2, num1) == 0) {
		return int_to_big(1);
	}
	else {
		if (abs_compare(num1, multiply(num2, int_to_big((int)pow(10, k)))) == 1 || abs_compare(num1, multiply(num2, int_to_big((int)pow(10, k)))) == 0)
			result->digits[k_len - k] = '0';
		else {
			result->digits[k_len - k] = '0';
		}
		while (k > 0) {
			k--;
			temp->digits[0] = num1->digits[0];
			temp->digits[1] = num1->digits[1];
			temp->digits[2] = '\0';
			temp1->digits[0] = num2->digits[0];
			temp1->digits[1] = num2->digits[1];
			temp1->digits[2] = '\0';
			if (strcmp(temp->digits, temp1->digits) == 0) {
				result->digits[k_len - k] = 9;
				result->length = result->length + 1;
				result->digits[k_len - k + 1] = '\0';
			}
			else if (num1->length == multiply(num2, powek(int_to_big(10), int_to_big(k)))->length) {
				temp2 = atoi(temp->digits) / atoi(temp1->digits);
				result->digits[k_len - k] = '0' + temp2;
				result->length += 1;
				result->digits[k_len - k + 1] = '\0';
			}
			else {
				temp->digits[2] = num1->digits[2];
				temp->digits[3] = '\0';
				temp2 = atoi(temp->digits) / atoi(temp1->digits);
				if (temp2 < 10) {
					result->digits[k_len - k] = '0' + temp2;
					result->length += 1;
					result->digits[k_len - k + 1] = '\0';
				}
				else {
					result->digits[k_len - k] = '0' + (temp2 / 10);
					result->length += 1;

					k_len++;

					result->digits[k_len - k] = '0' + (temp2 % 10);
					result->length += 1;
					result->digits[k_len - k + 1] = '\0';
				}
			}
			num1 = subtract(num1, multiply(multiply(int_to_big(temp2), num2), powek(int_to_big(10), int_to_big(k))));
			if (num1->is_negative == true) {
				num1 = add(num1, multiply(num2, powek(int_to_big(10), int_to_big(k))));
				if (result->digits[0] == '0') {
					remove_leading_zeros(result);
					result = subtract(result, int_to_big(1));
					for (int i = k_len - k; i >= 0; i--) {
						result->digits[i + 1] = result->digits[i];
					}
					result->digits[0] = '0';
					result->length++;
				}
				else
					result = subtract(result, int_to_big(1));
			}
		}
	}
	remove_leading_zeros(result);
	return(result);
}

big_int* divide_remainder(big_int* num1, big_int* num2) {
	big_int* res2 = init_big_int("1");
	big_int* result = divide(num1, num2);
	if (abs_compare(result, int_to_big(0)) == 0)
	{
		return num1;
	}
	else {
		res2 = subtract(num1, multiply(num2, result));
	}
	return res2;
}


big_int* NOD(big_int* a, big_int* b) {
	big_int* zero = init_big_int_zero(1);

	if (abs_compare(b, zero) == 0) {
		free_big_int(zero);
		return a;
	}
	else {
		big_int* remainder = int_to_big(divide_remainder(a, b));
		big_int* result = NOD(b, remainder);
		free_big_int(zero);
		free_big_int(remainder);
		return result;
	}
}



/////////////////////////////////////////////////////////         НЕПОНЯТНОООО!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// 
//big_int* gcdExtended(big_int* a, big_int* b, big_int** x, big_int** y) {
//	if (abs_compare(a, int_to_big(0)) == 0) {
//		*x = int_to_big(0);
//		*y = int_to_big(1);
//		return b;
//	}
//
//	big_int* x1, *y1, *c;
//	c = divide_remainder(b, a);
//	big_int* gcd = gcdExtended(c, a, &x1, &y1);
//
//	*x = subtract(y1, multiply((divide(b,a)),x1));
//	*y = subtract(y1, multiply((divide(b, a)), y1));
//
//	return gcd;
//}
//
//big_int* gcdExtended(big_int* a, big_int* b, big_int** x, big_int** y) {
//	big_int *x1 = int_to_big(0);
//	big_int* y1 = int_to_big(1);
//	big_int* x0 = int_to_big(1);
//	big_int* y0 = int_to_big(0);
//	big_int* q, *temp;
//	big_int* re = int_to_big(0);
//
//	while (abs_compare(a, int_to_big(0)) == 1) {
//		q = divide(b, a);
//		temp = a;
//
//		a = (divide_remainder(b, a));
//		b = temp;
//
//		temp = x0;
//		x0 = subtract(*x, multiply(q, x0));
//		*x = temp;
//
//		temp = y0;
//		y0 = subtract(*y, multiply(q, y0));
//		*y = temp;
//	}
//
//	return b;
//}
//
//big_int* modInverse(big_int* a, big_int* m) {
//	big_int* x = int_to_big(0), * y = int_to_big(0);
//	big_int* gcd = gcdExtended(a, m, &x, &y);
//	if (abs_compare(a, int_to_big(1)) == -1) {
//		printf("Обратный по модулю элемент не существует\n");
//		return -1; // Обратного элемента не существует
//	}
//
//	// Нормализация результата
//	big_int* result = (divide_remainder(add(int_to_big(divide_remainder(x, m)), m), m));
//	return result;
//}

big_int* modInverse(big_int* a, big_int* b) {
	char* x_str = a->digits;
	char* m_str = b->digits;
	mpz_t x, m, y;
	mpz_init_set_str(x, x_str, 10);
	mpz_init_set_str(m, m_str, 10);
	mpz_init_set_str(y, "0", 10);
	mpz_invert(y, x, m);
	gmp_printf("Res of modInverse: %Zd\n", y);
	big_int* result = init_big_int(mpz_get_str(NULL, 10, y));
	return result;
}


////////////////////////////////////////////////////////////            НЕПОНЯТНОООО!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


big_int* generate_prime_gamal(unsigned long long bits) {
	FILE* prime_numbers = NULL;
	char prime_num_str[MAX_SIZE_PRIME_NUM] = { 0 };
	int rand_num = rand() % 50;
	if (bits == 256)
		prime_numbers = fopen("generated_primes256.txt", "rb");
	else if (bits == 512)
		prime_numbers = fopen("generated_primes512.txt", "rb");
	else if (bits == 1024)
		prime_numbers = fopen("generated_primes1024.txt", "rb");
	else if (bits == 2048)
		prime_numbers = fopen("generated_primes2048.txt", "rb");
	else if (bits == 4096)
		prime_numbers = fopen("generated_primes4096.txt", "rb");
	else {
		printf("Необходимо ввести один из данных размеров: 256, 512, 1024, 2048, 4096\n");
		exit(EXIT_FAILURE);
	}
	for (int i = 1; i < rand_num; i++) {
		fgets(prime_num_str, sizeof(prime_num_str), prime_numbers);
		if (!strcmp(prime_num_str, "\r\n"))
			i--;
	}
	prime_num_str[strlen(prime_num_str) - 2] = '\0';
	big_int* prime_num_int = init_big_int(prime_num_str);
	fclose(prime_numbers);
	return prime_num_int;
}


big_int* generate_prime_rsa(unsigned long long bits) {
	FILE* prime_numbers = NULL;
	char prime_num_str[MAX_SIZE_PRIME_NUM] = { 0 };
	int rand_num = rand() % 50;
	if (bits == 256)
		prime_numbers = fopen("generated_primes128.txt", "rb");
	else if (bits == 512)
		prime_numbers = fopen("generated_primes256.txt", "rb");
	else if (bits == 1024)
		prime_numbers = fopen("generated_primes512.txt", "rb");
	else if (bits == 2048)
		prime_numbers = fopen("generated_primes1024.txt", "rb");
	else if (bits == 4096)
		prime_numbers = fopen("generated_primes2048.txt", "rb");
	else {
		printf("Необходимо ввести один из данных размеров: 256, 512, 1024, 2048, 4096\n");
		exit(EXIT_FAILURE);
	}
	for (int i = 1; i < rand_num; i++) {
		fgets(prime_num_str, sizeof(prime_num_str), prime_numbers);
		if (!strcmp(prime_num_str, "\r\n"))
			i--;
	}
	prime_num_str[strlen(prime_num_str) - 2] = '\0';
	big_int* prime_num_int = init_big_int(prime_num_str);
	fclose(prime_numbers);
	return prime_num_int;
}
//
//big_int* generate_prime(unsigned long long size) {
//	big_int* n;
//	big_int* temp;
//	big_int* random_number;
//	big_int* zero = init_big_int_zero(1);
//
//	do {
//		random_number = int_to_big(rand());
//		temp = int_to_big((int)size);
//		random_number = int_to_big(divide_remainder(random_number, temp));
//		n = random_number;
//		free_big_int(random_number);
//	} while (is_prime(n) == 0);
//
//	free_big_int(temp);
//	free_big_int(zero);
//
//	return n;
//}


/* Функция возведения числа a в степень b по модулю m */
big_int* power(big_int* base, big_int* exponent, big_int* modulus) {
	big_int* result = int_to_big(1);
	big_int* temp_base = init_big_int(base->digits);
	big_int* temp_exponent = init_big_int(exponent->digits);
	big_int* temp_modulus = init_big_int(modulus->digits);
	big_int* two = int_to_big(2);

	while (abs_compare(temp_exponent, init_big_int_zero(1)) > 0) {
		if (abs_compare(int_to_big(divide_remainder(temp_exponent, two)), int_to_big(1)) == 0) {
			result = multiply(result, temp_base);
			result = divide_remainder(result, temp_modulus);
		}
		temp_base = multiply(temp_base, temp_base);
		temp_base = divide_remainder(temp_base, temp_modulus);
		temp_exponent = int_to_big(divide(temp_exponent, two));
	}

	free_big_int(temp_base);
	free_big_int(temp_exponent);
	free_big_int(temp_modulus);
	free_big_int(two);

	return result;
}

/* Функция для проверки, является ли число a первообразным корнем по модулю m */
int is_primitive_root(big_int* a, big_int* m) {
	//big_int* zero = init_big_int("0");
	big_int* one = init_big_int("1");
	big_int* result = init_big_int("1");
	//big_int* powers = init_big_int("0");
	int is_primitive = 1;

	//if (abs_compare(a, zero) <= 0 || abs_compare(m, one) <= 0) {
	//	return 0;
	//}

	// Вычисление степеней a по модулю m
	mpz_t result_mpz, a_mpz, m_mpz, one_mpz;
	char* a_str = a->digits;
	char* m_str = m->digits;
	mpz_init_set_str(result_mpz, "0", 10);
	mpz_init_set_str(a_mpz, a_str, 10);
	mpz_init_set_str(m_mpz, m_str, 10);
	mpz_init_set_ui(one, 1);
	mpz_powm(result_mpz, a_mpz, m_mpz, m_mpz);
	is_primitive = (mpz_cmp(result, one_mpz) != 0);

	/*for (int i = 0; i < m->length - 1; i++) {
		result = multiply(result, a);
		result = (divide_remainder(result, m));

		if (powers->length > result->length) {
			powers->digits[result->length] = '1';
			break;
		}

		powers->digits[result->length - 1] = '1';
	}*/
	return is_primitive;
}

// Функция для генерации первообразного корня по модулю m
big_int* generate_primitive_root(big_int* m) {
	big_int* one = int_to_big(1);
	big_int* upper_bound = subtract(m, one);
	big_int* a = int_to_big(1);

	srand((unsigned int)time(NULL));

	do {
		a = int_to_big(1); // Инициализируем a значением 1

		// Генерируем случайное число a в диапазоне от 1 до upper_bound
		for (int i = 0; i < upper_bound->length - 1; i++) {
			a->digits[i] = (rand() % 10) + '0';
			a->length++;
		}
	} while (!is_primitive_root(a, m));
	a->digits[a->length - 1] = '\0'; // Убираем лишний символ
	return a;
}



big_int* powmod(big_int* base, big_int* exponent, big_int* modulus) {
	char* b = base->digits;
	char* exp = exponent->digits;
	char* m = modulus->digits;
	mpz_t bas, expon, modul, result;
	mpz_init_set_str(bas, b, 10);
	mpz_init_set_str(expon, exp, 10);
	mpz_init_set_str(modul, m, 10);
	mpz_init_set_str(result, "0", 10);
	mpz_powm(result, bas, expon, modul);
	big_int* result2 = init_big_int(mpz_get_str(NULL, 10, result));
	//big_int* result = int_to_big(1);
	//big_int* temp_base = init_big_int(base->digits);
	//big_int* temp_exponent = init_big_int(exponent->digits);
	//big_int* temp_modulus = init_big_int(modulus->digits);
	//big_int* two = int_to_big(2);

	//while (abs_compare(temp_exponent, int_to_big(0)) > 0) {
	//	if (abs_compare((divide_remainder(temp_exponent, two)), int_to_big(1)) == 0) {
	//		result = multiply(result, temp_base);
	//		result = divide_remainder(result, temp_modulus);
	//	}
	//	temp_base = multiply(temp_base, temp_base);
	//	temp_base = (divide_remainder(temp_base, temp_modulus));
	//	temp_exponent = (divide(temp_exponent, two));
	//}

	//free_big_int(temp_base);
	//free_big_int(temp_exponent);
	//free_big_int(temp_modulus);
	//free_big_int(two);

	return result2;
}




big_int* find_module_of_value(big_int* value, big_int* modul) {
	big_int* result = int_to_big(divide_remainder(value, modul));
	if (result->is_negative) {
		big_int* add_value = add(result, modul);
		free_big_int(result);
		result = add_value;
	}
	return result;
}

unsigned __int64 find_crc(char* file_name_in, big_int* modul) {
	FILE* infile = fopen(file_name_in, "rb");
	unsigned long long symbol = fgetc(infile);
	big_int* hash_prev = int_to_big(0);
	big_int* hash_now = int_to_big(symbol);
	int i = 0;
	symbol = fgetc(infile);
	i++;
	while (symbol != EOF) {
		hash_prev = hash_now;
		hash_now = powmod(add(int_to_big(symbol), hash_prev), int_to_big(2), modul);
		symbol = fgetc(infile);
		i++;
	}
	return hash_now;
}


void genkey(int size, char* pubkey, char* secret) {
	//Программа генерирует два простых числа p и q, вычисляет 
	//их произведение n и функцию Эйлера phi, а затем генерирует 
	//открытый ключ e и вычисляет соответствующий секретный ключ d. 
	//Функция is_prime проверяет, является ли число простым, 
	//а функция gcd находит наибольший общий делитель двух чисел.

	//Для проверки на простоту больших чисел Тест Милера - Рабина
	//Для возведения в степень не обязательно умножать число на себя,
	//можно делать побитовый сдвиг!!

	unsigned short int type = 0;
	printf("Выберите шифрование:\n1. RSA\n2. Эль-Гамаля\n");
	scanf_s("%hu", &type);
	FILE* public_file = NULL, * secret_file = NULL;
	public_file = fopen(pubkey, "wb");
	secret_file = fopen(secret, "wb");
	if (type == RSA) {


		big_int* p, * q, * n, * phi, * e, * d;

		srand((unsigned int)time(NULL)); // Инициализация генератора случайных чисел

		// Генерация простого числа p
		p = generate_prime_rsa(size);

		// Генерация простого числа q
		q = generate_prime_rsa(size);

		n = multiply(p, q); // Вычисление произведения p и q

		// Вычисление phi
		big_int* one = int_to_big(1);
		big_int* p_minus_1 = subtract(p, one);
		big_int* q_minus_1 = subtract(q, one);
		phi = multiply(p_minus_1, q_minus_1);

		// Генерация открытого ключа e
		do (e = generate_prime_rsa(size));
		while (abs_compare(e, phi) == 1);

		// Вычисление секретного ключа d
		big_int* x, * y;
		d = modInverse(e, phi);
		//mpz_t quotient, remainder, dividend, divisor;
		//char* num1 = e->digits;
		//char* num2 = phi->digits;
		//mpz_init_set_str(dividend, num1, 10);
		//mpz_init_set_str(divisor, num2, 10);
		//mpz_init_set_str(quotient, "0", 10);
		//mpz_init_set_str(remainder, "0", 10);
		//mpz_divmod(quotient, remainder, dividend, divisor);
		//gmp_printf("Del: %Zd\n", quotient);
		//gmp_printf("Ost: %Zd\n", remainder);
		//d = divide_remainder(e, phi);

		//Вывод ключей на экран
		printf("Открытый ключ: (%s, %s)\n", e->digits, n->digits);
		printf("Секретный ключ: (%s, %s)\n", d->digits, n->digits);
		fprintf(public_file, "%s %s", e->digits, n->digits);
		fprintf(secret_file, "%s %s", d->digits, n->digits);

		// Освобождение памяти
		free_big_int(p);
		free_big_int(q);
		free_big_int(n);
		free_big_int(phi);
		free_big_int(e);
		free_big_int(d);
		free_big_int(one);
		free_big_int(p_minus_1);
		free_big_int(q_minus_1);

	}
	else if (type == Gamal)
	{
		FILE* public_file = NULL, * secret_file = NULL;
		public_file = fopen(pubkey, "wb");
		secret_file = fopen(secret, "wb");
		srand((unsigned int)time(NULL));
		big_int* p, * g, * x, * y;
		p = generate_prime_gamal(size);
		g = generate_primitive_root(p);
		printf_s("Primitive root of %s is %s\n", p->digits, g->digits);
		big_int* p1 = subtract(p, int_to_big(1));
		x = subtract(p, int_to_big(1));
		y = powmod(g, x, p); // Открытый ключ
		printf_s("Открытый ключ: (%s, %s, %s)\n", p->digits, g->digits, y->digits);
		printf_s("Секретный ключ: (%s)\n", x->digits);
		fprintf(public_file, "%s %s %s", p->digits, g->digits, y->digits);
		fprintf(secret_file, "%s %s", x->digits, p->digits);
		free_big_int(p);
		free_big_int(g);
		free_big_int(x);
		free_big_int(y);
		free_big_int(p1);
	}
	else
	{
		printf_s("Некорректный ввод.");
	}
}


void sign(char* name_infile, char* name_secret, char* name_sigfile) {
	big_int* crc;
	char str[1000], key[1000] = { 0 };
	int i = 0, j = 0;
	unsigned short int type = 0;
	printf("Выберите шифрование:\n1. RSA\n2. Эль-Гамаля\n");
	scanf_s("%hu", &type);

	FILE* sigfile = fopen(name_sigfile, "wb");
	FILE* secret_key = fopen(name_secret, "rb");
	fgets(str, sizeof(str), secret_key);
	if (type == RSA) {
		big_int* d, * n;
		while (str[i] != ' ') {
			key[i] = str[i];
			i++;
		}
		d = init_big_int(key);
		i++;

		while (str[i] != '\0') {
			key[j] = str[i];
			i++;
			j++;
		}
		key[j] = '\0';
		n = init_big_int(key);

		crc = find_crc(name_infile, n);
		big_int* res = powmod(crc, d, n);
		fprintf(sigfile, "%s", res->digits);
		printf("Вычисленная подпись: %s\n", crc->digits);
	}

	else if (type == Gamal) {
		int list_of_prime_numbers[] = { 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107 };
		char name_pubkey[50] = { 0 };
		int random_prime_number = rand() % 27;
		big_int* p, * x, * k, * g, * r, * s;
		while (str[i] != '\0') {
			key[i] = str[i];
			i++;
		}
		x = init_big_int(key);
		printf("Введите название файла, который хранит открытый ключ: ");
		scanf_s("%s", &name_pubkey, 50);
		FILE* pubkey = fopen(name_pubkey, "rb");
		fgets(str, sizeof(str), pubkey);
		i = 0;
		while (str[i] != ' ') {
			i++;
		}
		key[i] = '\0';
		i++;
		while (str[i] != ' ') {
			key[j] = str[i];
			i++;
			j++;
		}
		key[j] = '\0';
		g = init_big_int(key);
		i++;
		j = 0;
		while (str[i] != '\0') {
			key[j] = str[i];
			i++;
			j++;
		}
		key[j] = '\0';
		p = init_big_int(key);

		crc = find_crc(name_infile, p); // Нахождение хэш значения файла по модулю p

		do {
			random_prime_number = rand() % 27;
			k = list_of_prime_numbers[random_prime_number];
		} while (NOD(k, subtract(p, int_to_big(1))) != 1 && abs_compare(subtract(p, int_to_big(1)), k) == 1 && abs_compare(k, int_to_big(0)) == 1);  // Выбор случайного числа k


		r = power(g, k, p);

		big_int* inverse_k = power(k, subtract(p, int_to_big(2)), subtract(p, int_to_big(1))); // Вычисление обратного элемента для k
		s = (divide_remainder(multiply(subtract(crc, multiply(x, r)), inverse_k), subtract(p, 1)));
		fprintf(sigfile, "%s %s", r->digits, s->digits);
		printf("Вычисленная подпись: %s\n", crc->digits);
	}
}


void check(char* name_infile, char* name_pubkey, char* name_sigfile) {
	big_int* crc, * e, * n, * h;
	char str[1000], key[1000] = { 0 }, signature[1000];
	int i = 0, j = 0;
	unsigned short int type = 0;
	printf("Выберите шифрование:\n1. RSA\n2. Эль-Гамаля\n");
	scanf_s("%hu", &type);

	FILE* sigfile = fopen(name_sigfile, "rb");
	FILE* pubkey = fopen(name_pubkey, "rb");
	if (type == RSA) {
		fgets(str, sizeof(str), pubkey);
		while (str[i] != ' ') {
			key[i] = str[i];
			i++;
		}

		key[i] = '\0';
		e = init_big_int(key);
		i++;

		while (str[i] != '\0') {
			key[j] = str[i];
			i++;
			j++;
		}

		key[j] = '\0';
		n = init_big_int(key);
		
		fgets(signature, sizeof(signature), sigfile);

		crc = find_crc(name_infile, n);

		h = powmod(init_big_int(signature), e, n);

		if (abs_compare(h, crc) == 0)
			printf("Передаваемая подпись: %s\nВычисленная подпись: %s\nПодписи совпадают.\n", h->digits, crc->digits);
		else
			printf("Передаваемая подпись: %s\nВычисленная подпись: %s\nПодписи не совпадают.\n", h->digits, crc->digits);
	}
	else if (type == Gamal) {
		fgets(str, sizeof(str), pubkey);
		fgets(signature, sizeof(signature), sigfile);
		big_int* p, * g, * y, * r, * s;
		big_int* podpis;
		while (str[i] != ' ') {
			key[j] = str[i];
			i++;
			j++;
		}
		key[j] = '\0';
		y = init_big_int(key);
		i++;
		j = 0;
		while (str[i] != ' ') {
			key[j] = str[i];
			i++;
			j++;
		}
		key[j] = '\0';
		g = init_big_int(key);
		i++;
		j = 0;
		while (str[i] != '\0') {
			key[j] = str[i];
			i++;
			j++;
		}
		key[j] = '\0';
		p = init_big_int(key);

		i = 0;
		j = 0;

		while (signature[i] != ' ') {
			key[j] = signature[i];
			i++;
			j++;
		}
		key[j] = '\0';
		r = init_big_int(key);
		i++;
		j = 0;
		while (signature[i] != '\0') {
			key[j] = signature[i];
			i++;
			j++;
		}
		key[j] = '\0';
		s = init_big_int(key);

		crc = find_crc(name_infile, p); // Нахождение хэш значения файла по модулю p

		if (r > 0 && r < p && s > 0 && s < p - 1) {
			podpis =
				podpis = find_module_of_value(multiply((powek(y, r)), (powek(r, s))), p);
			if (podpis == find_module_of_value((powek(g, crc)), p))
				printf("Yuhuuuu");
		}
	}
}
long long int modExp(long long int a, long long int b, long long int m) { // а в степень b, mod m
	long long int res = 1;
	while (b > 0) {
		if (b & 1) {
			res = (res * a) % m;
		}
		a = (a * a) % m;
		b >>= 1;
	}
	return res;
}

void encrypt(char* name_infile, char* name_pubkey, char* name_outfile) {
	unsigned short int type = 0;
	printf("Выберите шифрование:\n1. RSA\n2. Эль-Гамаля\n");
	scanf_s("%hu", &type);
	if (type == RSA)
	{
		FILE* input = fopen(name_infile, "r+");
		FILE* output = fopen(name_outfile, "w+");
		FILE* pk = fopen(name_pubkey, "r");
		big_int* e, * n;
		char num_str[5000] = { 0 };
		char num_str2[5000] = { 0 };
		fscanf_s(pk, "%s %s", num_str, num_str2);
		e = init_big_int(num_str);
		n = init_big_int(num_str2);
		fclose(pk);
		const int block = 128;
		unsigned char* buffer = calloc(block, sizeof(char));
		while (!feof(input)) {
			if (buffer != NULL) {
				short int bytesRead = fread(buffer, 1, block, input);
				// Шифрование блока данных
				for (size_t i = 0; i < bytesRead; i++) {
					long long int encrypted = atoi(powmod(int_to_big(buffer[i]), e, n)->digits);
					fwrite(&encrypted, sizeof(long long int), 1, output);
				}
			}
		}

		// Закрытие файлов
		fclose(input);
		fclose(output);

		printf("File encrypted successfully.\n");
	}
	else if (type == Gamal)
	{
		FILE* input = fopen(name_infile, "r");
		FILE* output = fopen(name_outfile, "w");
		FILE* pubkey = fopen(name_pubkey, "r");

		char g_str[5000] = { 0 };
		char p_str[5000] = { 0 };
		char y_str[5000] = { 0 };
		fscanf(pubkey, "%s %s %s", &g_str, &p_str, &y_str);
		big_int* g = init_big_int(g_str);
		big_int* p = init_big_int(p_str);
		big_int* y = init_big_int(y_str);
		fclose(pubkey);
		big_int* c1 = int_to_big(0);
		big_int* c2 = int_to_big(0);
		const int block_size = 128;
		unsigned char* buffer = calloc(block_size, sizeof(unsigned char));
		while (!feof(input)) {
			size_t bytes_read = fread(buffer, 1, block_size, input);

			// Шифрование блока данных
			for (size_t i = 0; i < bytes_read; i++) {
				unsigned char text = buffer[i];

				unsigned long long int k = rand();  // Случайное число k
				c1 = powmod(g, k, p);
				c2 = (divide_remainder(multiply(powmod(y, k, p), text), p));
				fwrite(&c1, sizeof(c1), 1, output);
				fwrite(&c2, sizeof(c2), 1, output);
			}
		}

		// Закрытие файлов
		fclose(input);
		fclose(output);

		printf("File encrypted successfully.\n");
	}
	else
	{
		printf("Выберите верный вариант.");
	}
}

void decrypt(char* name_infile, char* name_secret, char* name_outfile) {
	unsigned short int type = 0;
	printf("Выберите шифрование:\n1. RSA\n2. Эль-Гамаля\n");
	scanf_s("%hu", &type);
	if (type == RSA) {
		FILE* input = fopen(name_infile, "rb");
		FILE* output = fopen(name_outfile, "w");
		FILE* secretKey = fopen(name_secret, "r");
		if (input == NULL || output == NULL || secretKey == NULL) {
			exit(1);
		}
		big_int* d, * n;
		char num_str[5000] = { 0 };
		char num_str2[5000] = { 0 };
		fscanf_s(secretKey, "%s %s", num_str, num_str2);
		d = init_big_int(num_str);
		n = init_big_int(num_str2);
		fclose(secretKey);
		const int blockSize = sizeof(long long int);
		unsigned char* buffer = calloc(blockSize, sizeof(unsigned char));
		if (buffer != NULL) {
			while (fread(buffer, blockSize, 1, input) == 1) {
				big_int* encrypted = int_to_big(0);
				memcpy(encrypted, buffer, blockSize);
				big_int* decrypted = powmod(encrypted, d, n);
				fprintf(output, "%s", decrypted->digits);
			}
		}
		free(buffer);
		fclose(input);
		fclose(output);
		printf("File decrypted successfully.\n");
	}
	else if (type == Gamal)
	{
	}
	else
	{
		printf("Выберите верный вариант.");
	}

}


int main(int argc, char** argv)
{
	setlocale(LC_ALL, "ru-Ru");
	char command[1000] = { 0 };
	char command_size[SIZEOF_COMMAND] = { 0 },
		size[SIZEOF_COMMAND] = { 0 },
		command_infile[SIZEOF_COMMAND] = { 0 },
		name_infile[SIZEOF_COMMAND] = { 0 },
		command_pubkey[SIZEOF_COMMAND] = { 0 },
		name_pubkey[SIZEOF_COMMAND] = { 0 },
		command_secret[SIZEOF_COMMAND] = { 0 },
		name_secret[SIZEOF_COMMAND] = { 0 },
		command_sigfile[SIZEOF_COMMAND] = { 0 },
		name_sigfile[SIZEOF_COMMAND] = { 0 },
		command_outfile[SIZEOF_COMMAND] = { 0 },
		name_outfile[SIZEOF_COMMAND] = { 0 };
	int count = 0, i = 0;
	wprintf_s(L"Для получения справки по коммандам введите \"crypt --help\"\n");
	while (count != 2) {
		command[i] = (char)getc(stdin);
		if (command[i] == ' ' || command[i] == '\0' || command[i] == '\n')
			count++;
		i++;
	}
	command[strlen(command) - 1] = '\0';

	if (strncmp(command, "crypt", 5) == 0) {
		if (strstr(command, argv[1])) {
			help();
		}
		else if (strstr(command, argv[2])) {
			scanf_s("%s %s %s %s %s %s",
				&command_size, SIZEOF_COMMAND,
				&size, SIZEOF_COMMAND,
				&command_pubkey, SIZEOF_COMMAND,
				&name_pubkey, SIZEOF_COMMAND,
				&command_secret, SIZEOF_COMMAND,
				&name_secret, SIZEOF_COMMAND);
			if (!strcmp(command_size, "--size") && !strcmp(command_pubkey, "--pubkey") && !strcmp(command_secret, "--secret"))
				genkey(atoi(size), name_pubkey, name_secret);
			else
				wprintf_s(L"Неверно введена команда, для получения справки по коммандам введите \"crypt --help\"\n");
		}
		else if (strstr(command, argv[3])) {
			scanf_s("%s %s %s %s %s %s",
				&command_infile, SIZEOF_COMMAND,
				&name_infile, SIZEOF_COMMAND,
				&command_secret, SIZEOF_COMMAND,
				&name_secret, SIZEOF_COMMAND,
				&command_sigfile, SIZEOF_COMMAND,
				&name_sigfile, SIZEOF_COMMAND);
			if (!strcmp(command_infile, "--infile") && !strcmp(command_secret, "--secret") && !strcmp(command_sigfile, "--sigfile"))
				sign(name_infile, name_secret, name_sigfile);
			else
				wprintf_s(L"Неверно введена команда, для получения справки по коммандам введите \"crypt --help\"\n");
		}
		else if (strstr(command, argv[4])) {
			scanf_s("%s %s %s %s %s %s",
				&command_infile, SIZEOF_COMMAND,
				&name_infile, SIZEOF_COMMAND,
				&command_pubkey, SIZEOF_COMMAND,
				&name_pubkey, SIZEOF_COMMAND,
				&command_sigfile, SIZEOF_COMMAND,
				&name_sigfile, SIZEOF_COMMAND);
			if (!strcmp(command_infile, "--infile") && !strcmp(command_pubkey, "--pubkey") && !strcmp(command_sigfile, "--sigfile"))
				check(name_infile, name_pubkey, name_sigfile);
			else
				wprintf_s(L"Неверно введена команда, для получения справки по коммандам введите \"crypt --help\"\n");
		}
		else if (strstr(command, argv[5])) {
			scanf_s("%s %s %s %s %s %s",
				&command_infile, SIZEOF_COMMAND,
				&name_infile, SIZEOF_COMMAND,
				&command_pubkey, SIZEOF_COMMAND,
				&name_pubkey, SIZEOF_COMMAND,
				&command_outfile, SIZEOF_COMMAND,
				&name_outfile, SIZEOF_COMMAND);
			if (!strcmp(command_infile, "--infile") && !strcmp(command_pubkey, "--pubkey") && !strcmp(command_outfile, "--outfile"))
				encrypt(name_infile, name_pubkey, name_outfile);
			else
				wprintf_s(L"Неверно введена команда, для получения справки по коммандам введите \"crypt --help\"\n");
		}
		else if (strstr(command, argv[6])) {
			scanf_s("%s %s %s %s %s %s",
				&command_infile, SIZEOF_COMMAND,
				&name_infile, SIZEOF_COMMAND,
				&command_secret, SIZEOF_COMMAND,
				&name_secret, SIZEOF_COMMAND,
				&command_outfile, SIZEOF_COMMAND,
				&name_outfile, SIZEOF_COMMAND);
			if (!strcmp(command_infile, "--infile") && !strcmp(command_secret, "--secret") && !strcmp(command_outfile, "--outfile"))
				decrypt(name_infile, name_secret, name_outfile);
			else
				wprintf_s(L"Неверно введена команда, для получения справки по коммандам введите \"crypt --help\"\n");
		}
	}
	else {
		wprintf_s(L"Неверная команда");
	}

	return 0;
}