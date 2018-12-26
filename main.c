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

Monomial *new_monomial(int coefficient, int exponent) {
    Monomial *monomial = malloc(sizeof(Monomial));
    monomial->coefficient = coefficient;
    monomial->exponent = exponent;
    return monomial;
}

void delete_monomial(Monomial *monomial) {
    free(monomial);
}

/// stack start
typedef struct Item {
    char char_item;
    Monomial *monomial_item;
} Item;

Item *new_char_item(char item) {
    Item *new_item = malloc(sizeof(Item));
    new_item->char_item = item;
    new_item->monomial_item = NULL;
    return new_item;
}

Item *new_monomial_item(Monomial *item) {
    Item *new_item = malloc(sizeof(Item));
    new_item->char_item = '\0';
    new_item->monomial_item = item;
    return new_item;
}

void delete_item(Item *item) {
    if (item && item->monomial_item) {
        delete_monomial(item->monomial_item);
    }
    free(item);
}

typedef struct Stack {
    int top;
    Item **items;
} Stack;

bool is_empty_stack(Stack *stack) {
    return stack->top == -1;
}

Stack *new_stack() {
    Stack *stack = malloc(sizeof(Stack));
    stack->top = -1;
    stack->items = malloc(sizeof(Item) * POLYNOMIALS_AMOUNT);
    return stack;
}

void delete_stack(Stack *stack) {
    for (int i = 0; i < POLYNOMIALS_AMOUNT; ++i) {
        delete_item(stack->items[i]);
    }
    free(stack->items);
    free(stack);
}

void push_stack(Stack *stack, Item *new_item) {
    stack->items[++(stack->top)] = new_item;
}

Item *peek_stack(Stack *stack) {
    return is_empty_stack(stack) ? NULL : stack->items[stack->top];
}

Item *pop_stack(Stack *stack) {
    Item *item = peek_stack(stack);
    stack->top--;
    return item;
}

bool is_monomial(Item *new_item) {
    return new_item->monomial_item != NULL;
}
/// stack end


//napisz nowe queue po skonczeniu
typedef struct Queue {
    int top;
    int bottom;
    int items_count;
    Item **items;
} Queue;

Queue *new_queue() {
    Queue *queue = malloc(sizeof(Queue));
    queue->items = malloc(sizeof(Item) * POLYNOMIALS_AMOUNT);
    queue->top = 0;
    queue->bottom = -1;
    queue->items_count = 0;
    return queue;
}

void delete_queue(Queue *queue) {
    for (int i = 0; i < POLYNOMIALS_AMOUNT; ++i) {
        delete_item(queue->items[i]);
    }
    free(queue->items);
    free(queue);
}

Item *peek_queue(Queue *queue) {
    return queue->items[queue->top];
}

Item *pop_queue(Queue *queue) {
    Item *item = queue->items[(queue->top)++];
    if (queue->top == POLYNOMIALS_AMOUNT)
        queue->top = 0;

    (queue->items_count)--;
    return item;
}

void push_queue(Queue *queue, Item *item) {
    if (queue->bottom == POLYNOMIALS_AMOUNT - 1)
        queue->bottom = -1;

    queue->items[++(queue->bottom)] = item;
    (queue->items_count)++;
}

bool is_full_queue(Queue *queue) {
    return queue->items_count == POLYNOMIALS_AMOUNT;
}

bool is_empty_queue(Queue *queue) {
    return queue->items_count == 0;
}

bool is_operator(int token) {
    return token == '+' || token == '-' || token == '*';
}

void add_digit(int token, int *variable, bool negative_instead_subtraction) {
    int digit = token - '0';
    if(negative_instead_subtraction) {
        *variable = -digit;
        return;
    }
    *variable *= 10;
    *variable += *variable < 0 ? -digit : digit;
}

int precedence(int token) {
    switch (token) {
        case '*':
            return 3;
        case '+':
        case '-':
            return 2;
        default:
            return 1;
    }
}

bool loaded_monomial(int token, int coefficient, int exponent) {
    if (coefficient == 0 && exponent == 0) return false;
    return !isdigit(token) && !isspace(token);
}

void move_stronger_operators_to_output(int token, Queue* output, Stack* operators) {
    Item *top_operator = peek_stack(operators);
    while (top_operator && !is_empty_stack(operators) && precedence(top_operator->char_item) >= precedence(token)
           && (top_operator->char_item != '(')) {
        push_queue(output, pop_stack(operators));
        top_operator = peek_stack(operators);
    }
    push_stack(operators, new_char_item((char) token));
}

void push_monomial(Queue *output, int *coefficient, int *exponent) {
    Monomial *monomial = new_monomial(*coefficient, *exponent);
    push_queue(output, new_monomial_item(monomial));
    *exponent = 0;
    *coefficient = 0;
}

int infer_coefficient(int token, int previous_token, int actual_coefficient, bool *negative_started) {
    static bool is_first_token = true;
    int result = actual_coefficient;
    if(token == '-' && (is_first_token || previous_token == '(' || is_operator(previous_token))) {
        //not supported negative exponents
        *negative_started = true;
        result = -1;
    } else if (token == 'x' && actual_coefficient == 0) {
        result = 1;
    }

    is_first_token = false;
    return result;
}

bool infer_if_negative(int previous_token, int pre_previous_token) {
    return previous_token == '-' && (pre_previous_token == '\0' || pre_previous_token == '(');
}

void move_operators_until_bracket_to_output(Stack* operators, Queue *output) {
    Item *item;
    while ((item = pop_stack(operators))->char_item != '(') {
        push_queue(output, item);
    }
}

int max(int x, int y);

//TODO operaje na kolejce wyjscia - wielomiany
int main() {
    Queue *output = new_queue();
    Stack *operators = new_stack();

    int coefficient = 0;
    int exponent = 0;
    int *current_num = &coefficient;
    int token;
    int previous_token = '\0';
    int pre_previous_token = '\0';

    //loading input using shunting-yard algorithm, modified to accept negative numbers
    do {
        token = getchar();
        if(isspace(token)) continue;

        bool negative_started = false;
        coefficient = infer_coefficient(token, previous_token, coefficient, &negative_started);

        if (isdigit(token) && current_num != NULL) {
            add_digit(token, current_num, infer_if_negative(previous_token, pre_previous_token));
        } else if (token == 'x') {
            current_num = NULL;
            exponent = 1;
        } else if (token == '^') {
            current_num = &exponent;
            exponent = 0;
        } else if (loaded_monomial(token, coefficient, exponent) && !negative_started) {
            push_monomial(output, &coefficient, &exponent);
            current_num = &coefficient;
        }

        if (is_operator(token) && !negative_started) {
            move_stronger_operators_to_output(token, output, operators);
        } else if (token == '(') {
            push_stack(operators, new_char_item((char) token));
        } else if (token == ')') {
            move_operators_until_bracket_to_output(operators, output);
        }

        printf("%c ||", token);
        for (int i = 0; i < output->items_count; ++i) {
            if (output->items[i]->monomial_item) printf(" %dx^%d ", output->items[i]->monomial_item->coefficient, output->items[i]->monomial_item->exponent);
            else printf(" %c", output->items[i]->char_item);
        }
        printf(" ||");
        for (int i = 0; i <= operators->top; ++i) {
            printf(" %c",operators->items[i]->char_item);
        }
        printf("\n");

        pre_previous_token = previous_token;
        previous_token = token;

    } while (token != EOF);

    while (!is_empty_stack(operators)) {
        push_queue(output, pop_stack(operators));
    }

    while (!is_empty_queue(output)) {
        Item *item = pop_queue(output);
        if (item->monomial_item) printf("%dx^%d\n", item->monomial_item->coefficient, item->monomial_item->exponent);
        else printf("%c\n", item->char_item);
    }



    return 0;
}

int max(int x, int y) {
    if (x > y) return x;
    else return y;
}
