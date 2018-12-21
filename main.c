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

typedef struct Monomial {
    int coefficient;
    int exponent;
} Monomial;

/// stack start
typedef union StackItem {
    char char_item;
    Monomial *monomial_item;
} StackItem;

typedef struct Stack {
    int top;
    StackItem **items;
} Stack;

Stack* new_stack() {
    Stack* stack = malloc(sizeof(Stack));
    stack->top = -1;
    stack->items = malloc(sizeof(StackItem) * POLYNOMIALS_AMOUNT);
    return stack;
}

void delete_stack(Stack *stack) {
    for (int i = 0; i < POLYNOMIALS_AMOUNT; ++i) {
        if(stack->items[i]->monomial_item) free(stack->items[i]->monomial_item);
    }
    free(stack->items);
    free(stack);
}

void push(Stack *stack, StackItem *item) {
    stack->items[++stack->top] = item;
}

StackItem* peek(Stack *stack) {
    return stack->items[stack->top];
}

StackItem* pop(Stack *stack) {
    return stack->items[stack->top--];
}

bool is_empty(Stack *stack) {
    return stack->top == -1;
}
/// stack end


int max(int x, int y);

void add_digit(int *variable, int digit_char, bool negative);

struct Polynomial *multiply_polynomials(struct Polynomial *pol1, struct Polynomial *pol2);

void clear(int array[], size_t size);

int estimate_degree(struct Polynomial *polynomial, int max_degree_found);

int main() {



    return 0;
}


int max(int x, int y) {
    if (x > y) return x;
    else return y;
}

//void print(struct Polynomial *polynomial, int max_degree_found) {
//    bool found_degree = false;
//    for (int i = max_degree_found; i >= 0; i--) {
//        if (!polynomial) continue;
//        int num = polynomial->monomials[i];
//        if (num != 0) {
//            if (num < 0 && found_degree) printf(" - ");
//            else if (num < 0) printf("-");
//            else if (found_degree) printf(" + ");
//            found_degree = true;
//
//            if (abs(num) != 1 || i == 0) {
//                printf("%d", abs(num));
//            }
//            if (i > 0) {
//                printf("x");
//            }
//            if (i > 1) {
//                printf("^%d", i);
//            }
//        }
//    }
//}

//void add_monomial(struct Polynomial *polynomial, int constant, int degree, int *max_degree) {
//    polynomial->monomials[degree] += constant;
//    *max_degree = max(*max_degree, degree);
//}
//
//void add_digit(int *variable, int digit_char, bool negative) {
//    int digit_int = (digit_char - '0');
//    *variable *= 10;
//    *variable += (negative ? -digit_int : digit_int);
//}
//
//struct Polynomial *multiply_polynomials(struct Polynomial *pol1, struct Polynomial *pol2) {
//    struct Polynomial *polynomial = new_polynomial();
//
//    for (int i = 0; i <= pol1->degree; ++i) {
//        for (int j = 0; j <= pol2->degree; ++j) {
//            polynomial->monomials[i + j] += pol1->monomials[i] * pol2->monomials[j];
//            if (polynomial->monomials[i + j] != 0) polynomial->degree = i + j;
//        }
//    }
//
//    delete_polynomial(pol1);
//    delete_polynomial(pol2);
//
//    return polynomial;
//}
//
//void clear(int array[], size_t size) {
//    memset(array, 0, size);
//}
//
//int estimate_degree(struct Polynomial *polynomial, int max_degree_found) {
//    while (!polynomial->monomials[max_degree_found--]);
//    return ++max_degree_found;
//}
