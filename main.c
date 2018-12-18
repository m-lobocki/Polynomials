#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_DEGREE 100
#define POLYNOMIALS_AMOUNT 100

#undef DEBUG
#define DEBUG 1 // change here if debug
#if DEBUG
#undef EOF
#define EOF 'q'
#endif

/////////////////////////////////////////////////////////////////////////
/// Polynomial

struct Polynomial {
    int degree;
    int *monomials;
    char operation;
};

struct Polynomial *new_polynomial() {
    struct Polynomial *polynomial = malloc(sizeof(struct Polynomial));
    polynomial->degree = 0;
    polynomial->monomials = malloc(sizeof(int) * (MAX_DEGREE + 1));
    polynomial->operation = '+';
    return polynomial;
}

void delete_polynomial(struct Polynomial *polynomial) {
    if (polynomial) {
        free(polynomial->monomials);
        free(polynomial);
    }
}

void delete_polynomials(struct Polynomial *polynomials[], int length) {
    for (int i = 0; i < length; ++i) {
        delete_polynomial(polynomials[i]);
    }
}

////////////////////////////////////////////////////////////////////////
/// Stack

static struct Polynomial *polynomialStack[POLYNOMIALS_AMOUNT];
int stackTop = -1;

void push(struct Polynomial *polynomial) {
    polynomialStack[++stackTop] = polynomial;
}

struct Polynomial *pop() {
    return polynomialStack[stackTop--];
}

bool is_empty() {
    return stackTop == -1;
}

////////////////////////////////////////////////////////////////////////

int max(int x, int y);

void print(struct Polynomial *polynomial, int max_degree_found);

void add_monomial(struct Polynomial *polynomial, int constant, int degree, int *max_degree);

void add_digit(int *variable, int digit_char, bool negative);

struct Polynomial* multiply_polynomials(struct Polynomial *pol1, struct Polynomial *pol2);

void clear(int array[], size_t size);

struct Polynomial* load_next_polynomial(int *last_char, char *last_operation);

int estimate_degree(struct Polynomial *polynomial, int max_degree_found);

int main() {
    int last_char = 0;
    char last_operation = '+';

    while (last_char != EOF) {
        struct Polynomial *polynomial = load_next_polynomial(&last_char, &last_operation);
        push(polynomial);
    }

    //  TODO
    /// OPERACJE POMIEDZY WIELOMIANAMI
    while(!is_empty()) {
        struct Polynomial *polynomial = pop();
        if(polynomial->degree < 0) continue;
        printf("[%c] ", polynomial->operation);
        print(polynomial, 10);
        printf(" [lvl %d]", polynomial->degree);
        printf("\n");
    }

    delete_polynomials(polynomialStack, POLYNOMIALS_AMOUNT);
    return 0;
}

struct Polynomial* load_next_polynomial(int *last_char, char *last_operation) {
    int max_degree_found = 0;
    int constant = 0;
    int degree = 0;
    bool negative = false;
    bool loading_degree = false;
    bool loading_constant = true;
    struct Polynomial *polynomial = new_polynomial();
    polynomial->operation = *last_operation;

    int c;
    do {
        c = getchar();
        *last_char = c;

        if(c == '+' || c == '-' || c == '*') *last_operation = (char)c;

        if (isspace(c)) continue;
        if (isdigit(c) && !loading_degree) loading_constant = true;
        if (!isdigit(c)) loading_degree = false;

        if(c=='('){
            polynomial->degree = estimate_degree(polynomial, max_degree_found);
            return  polynomial;
        }

        if (c == '^') {
            loading_degree = true;
            loading_constant = false;
            degree = 0;
        }

        if (!isdigit(c) && c != 'x' && c != '^') {
            add_monomial(polynomial, constant, degree, &max_degree_found);
            constant = 0;
            degree = 0;
            negative = false;
            loading_constant = false;
            loading_degree = false;
        }

        if (c == '-') {
            negative = true;
        }

        if (loading_constant && isdigit(c)) {
            add_digit(&constant, c, negative);
        }

        if (loading_degree && !loading_constant && c != '^') {
            add_digit(&degree, c, false);
        }

        if (c == 'x') {
            degree = 1;
            if (!constant) {
                constant = negative ? -1 : 1;
                loading_constant = true;
                loading_degree = false;
            }
        }

        if ((c == EOF || c == ')') && !loading_constant && constant != 0) {
            add_monomial(polynomial, constant, 1, &max_degree_found);
        }

        if ((c == EOF || c == ')') && loading_constant) {
            add_monomial(polynomial, constant, 0, &max_degree_found);
        }

        if ((c == EOF || c == ')') && loading_degree) {
            add_monomial(polynomial, constant, degree, &max_degree_found);
        }
    } while (c != EOF && c != ')');

    polynomial->degree = estimate_degree(polynomial, max_degree_found);
    return polynomial;
}

int max(int x, int y) {
    if (x > y) return x;
    else return y;
}

void print(struct Polynomial *polynomial, int max_degree_found) {
    bool found_degree = false;
    for (int i = max_degree_found; i >= 0; i--) {
        if (!polynomial) continue;
        int num = polynomial->monomials[i];
        if (num != 0) {
            if (num < 0 && found_degree) printf(" - ");
            else if (num < 0) printf("-");
            else if (found_degree) printf(" + ");
            found_degree = true;

            if (abs(num) != 1 || i == 0) {
                printf("%d", abs(num));
            }
            if (i > 0) {
                printf("x");
            }
            if (i > 1) {
                printf("^%d", i);
            }
        }
    }
}

void add_monomial(struct Polynomial *polynomial, int constant, int degree, int *max_degree) {
    polynomial->monomials[degree] += constant;
    *max_degree = max(*max_degree, degree);
}

void add_digit(int *variable, int digit_char, bool negative) {
    int digit_int = (digit_char - '0');
    *variable *= 10;
    *variable += (negative ? -digit_int : digit_int);
}

struct Polynomial *multiply_polynomials(struct Polynomial *pol1, struct Polynomial *pol2) {
    struct Polynomial *polynomial = new_polynomial();

    for (int i = 0; i <= pol1->degree; ++i) {
        for (int j = 0; j <= pol2->degree; ++j) {
            polynomial->monomials[i + j] += pol1->monomials[i] * pol2->monomials[j];
            if (polynomial->monomials[i + j] != 0) polynomial->degree = i + j;
        }
    }

    delete_polynomial(pol1);
    delete_polynomial(pol2);

    return polynomial;
}

void clear(int array[], size_t size) {
    memset(array, 0, size);
}

int estimate_degree(struct Polynomial *polynomial, int max_degree_found) {
    while(!polynomial->monomials[max_degree_found--]);
    return ++max_degree_found;
}
