// by @rextco
// Dictionary generator for IDA 7.2 (drand48)
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// drand48 implementation: http://web.mit.edu/cgs/src/math/drand48/drand48.c
#define N 16
#define MASK ((unsigned)(1 << (N - 1)) + (1 << (N - 1)) - 1)
#define LOW(x) ((unsigned)(x) & MASK)
#define HIGH(x) LOW((x) >> N)
#define MUL(x, y, z) { long l = (long)(x) * (long)(y); (z)[0] = LOW(l); (z)[1] = HIGH(l); }
#define CARRY(x, y) ((long)(x) + (long)(y) > MASK)
#define ADDEQU(x, y, z) (z = CARRY(x, (y)), x = LOW(x + (y)))
#define X0 0x330E
#define X1 0xABCD
#define X2 0x1234
#define A0 0xE66D
#define A1 0xDEEC
#define A2 0x5
#define C 0xB
#define SET3(x, x0, x1, x2) ((x)[0] = (x0), (x)[1] = (x1), (x)[2] = (x2))
#define SETLOW(x, y, n) SET3(x, LOW((y)[n]), LOW((y)[(n)+1]), LOW((y)[(n)+2]))
#define SEED(x0, x1, x2) (SET3(x, x0, x1, x2), SET3(a, A0, A1, A2), c = C)
#define REST(v) for (i = 0; i < 3; i++) { xsubi[i] = x[i]; x[i] = temp[i]; } return (v);
#define NEST(TYPE, f, F) TYPE f(xsubi) register unsigned short *xsubi; { \
		register int i; register TYPE v; unsigned temp[3]; \
		for (i = 0; i < 3; i++) { temp[i] = x[i]; x[i] = LOW(xsubi[i]); } \
		v = F(); REST(v); }

#define HI_BIT (1L << (2 * N - 1))

static unsigned x[3] = { X0, X1, X2 }, a[3] = { A0, A1, A2 }, c = C;

static void next() {
	unsigned p[2], q[2], r[2], carry0, carry1;

	MUL(a[0], x[0], p);
	ADDEQU(p[0], c, carry0);
	ADDEQU(p[1], carry0, carry1);
	MUL(a[0], x[1], q);
	ADDEQU(p[1], q[0], carry0);
	MUL(a[1], x[0], r);  x[2] = LOW(carry0 + carry1 + CARRY(p[1], r[0]) + q[1] + r[1] + a[0] * x[2] + a[1] * x[1] + a[2] * x[0]);
	x[1] = LOW(p[1] + r[0]);
	x[0] = LOW(p[0]);
}

double drand48() {
	static double two16m = 1.0 / (1L << N);
	next();
	return (two16m * (two16m * (two16m * x[0] + x[1]) + x[2]));
}

void srand48(unsigned long seedval) {
	SEED(X0, LOW(seedval), HIGH(seedval));
}

// Start script
unsigned int seed_start = 1; // start of seed
unsigned int number_of_passwords = 0xFFFFFFFF; // UINT_MAX for double_rand = 0x7FFFFFFF
const int size_of_key = 12; 
#define size_of_word 13	// size_of_key + null byte

const char pwd[] = "7JpT48a7Y2fv";
const char pool_option[] = "abcdefghijkmpqrstuvwxyzABCDEFGHJKLMPQRSTUVWXYZ23456789";  // Good pool
// Ctrl + K + C | Ctrl + K + U
int main(int argc, const char *argv[]) {	
	char word[size_of_word] = { 0 };

	int index_key = 0;
	unsigned int attempts = 0;

	while (seed_start < number_of_passwords){
		srand48(seed_start);									// srand(seed_start);
		drand48();												// discard first byte
		for(unsigned int n = 0; n < size_of_key; n++){
			index_key = (int)(drand48() * 54 );					// index_key = (int)(double_rand() * 54);
			// printf("%d\n", index_key);
			word[n] = pool_option[index_key];
		}
		printf("%s\n", word);

		/*
		if(strstr(word, pwd) != NULL) {
			printf("Seed founded = %u\n", seed_start);
			printf("Pwd = %s\n", word);
			break;
		}

		if (attempts % (1 << 24) == 0) {
			printf("debug!control: word = %s | seed = %u | attempts = %u\n", word, seed_start, attempts);
		}
		*/

		attempts++;
		seed_start++;
	}

	printf("debug!exit:\n");
	getchar();
	return 0;
}