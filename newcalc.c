#include <stdio.h>
#include <string.h>

#define MAX 256

enum Type {INTEGER=1, PLUS=43, MINUS=45, MUL=42, DIV=47, OPENPAR=40, CLOSPAR=41};

char error;

typedef struct
{
    char Type;
    int value;
} Token;

int stack[MAX] = {0};
int* stack_ptr = stack;

void push(int op)
{
    stack_ptr++;
    *stack_ptr = op;
}

int pop()
{
    int op = *stack_ptr;
    *stack_ptr = 0;
    stack_ptr--;
    return op;
}

char operatorsinstack(int* stack_base_ptr)
{
    int* p = stack_ptr;
    for (; p >= stack_base_ptr; p++)
    {
        if (*p >= MUL && *p <= DIV)
            return 1;
    }
    return 0;
}

char priority(char op)
{
    switch (op)
    {
        case '+': 
        case '-': return 1;
        case '*': 
        case '/': return 2;
        default: return 0;
    }
}

char findlen(Token* tokens)
{
    char numtokens = 0;
    Token* p = tokens;
    for (; p->value != -1; p++)
        numtokens++;
    return numtokens;
}

Token* tokenize(char* infix_expr)
{
    static Token tokens_infix[MAX] = {0};
    Token* tokens = tokens_infix;

    for (int i = 0; i < MAX; i++) 
    {
        tokens_infix[i].Type = 0;
        tokens_infix[i].value = 0;
    }

    while (*infix_expr != '\n')
    {
        if (!(*infix_expr == '+' || *infix_expr == '-' || *infix_expr == '*' || *infix_expr == '/' || *infix_expr == '(' || 
            *infix_expr == ')' || (*infix_expr >= '0' && *infix_expr <= '9')))
        {
            infix_expr++;
            continue;
        }

        switch (*infix_expr)
        {
            case '+':
                tokens->Type = PLUS;
                tokens->value = '+';
                infix_expr++;
                tokens++;
                break;

            case '-':
                tokens->Type = MINUS;
                tokens->value = '-';
                infix_expr++;
                tokens++;
                break;

            case '*':
                tokens->Type = MUL;
                tokens->value = '*';
                infix_expr++;
                tokens++;
                break;

            case '/':
                tokens->Type = DIV;
                tokens->value = '/';
                infix_expr++;
                tokens++;
                break;

            case '(':
                tokens->Type = OPENPAR;
                tokens->value = '(';
                infix_expr++;
                tokens++;
                break;

            case ')':
                tokens->Type = CLOSPAR;
                tokens->value = ')';
                infix_expr++;
                tokens++;
                break;

            default:
                if (*infix_expr >= '0' && *infix_expr <= '9')
                {
                    tokens->Type = INTEGER;
                    tokens->value = 0;

                    while (*infix_expr >= '0' && *infix_expr <= '9')
                    {
                        tokens->value = (tokens->value * 10) + ((*infix_expr) - '0');
                        infix_expr++;
                    }
                    tokens++;
                }
                break;
        }
    }
    tokens->value = -1; // end
    char numtokens;
    if ((numtokens = findlen(tokens_infix)) == 0)
    {
        error = 1;
        return NULL;
    }
    return tokens_infix;
}

Token* parse(Token* tokens_infix_ptr)
{
    static Token tokens_postfix[MAX] = {0};
    Token* tokens_postfix_ptr = tokens_postfix;

    int* stack_base_ptr = stack_ptr;

    for (int i = 0; i < MAX; i++) 
    {
        tokens_postfix[i].Type = 0;
        tokens_postfix[i].value = 0;
    }

    while (tokens_infix_ptr->value != -1)
    {
        if (tokens_infix_ptr->Type == INTEGER)
        {
            tokens_postfix_ptr->Type = INTEGER;
            tokens_postfix_ptr->value = tokens_infix_ptr->value;
            tokens_infix_ptr++;
            tokens_postfix_ptr++;
            continue;
        }
        if (tokens_infix_ptr->Type >= MUL && tokens_infix_ptr->Type <= DIV && 
            tokens_infix_ptr->Type != OPENPAR && tokens_infix_ptr ->Type != CLOSPAR)
        {
            while (priority(*stack_ptr) != 0 && priority(*stack_ptr) >= priority(tokens_infix_ptr->Type))
            {
                tokens_postfix_ptr->Type = *stack_ptr;
                tokens_postfix_ptr->value = pop();
                tokens_postfix_ptr++;
            }
            push(tokens_infix_ptr->value);
            tokens_infix_ptr++;
            continue;
        }

        if (tokens_infix_ptr->Type == OPENPAR)
        {
            push(tokens_infix_ptr->value);
            tokens_infix_ptr++;
            continue;
        }

        if (tokens_infix_ptr->Type == CLOSPAR)
        {
            while (*stack_ptr != OPENPAR)
            {
                tokens_postfix_ptr->Type = *stack_ptr;
                tokens_postfix_ptr->value = pop();
                tokens_postfix_ptr++;
                if (stack_ptr == stack_base_ptr)
                {
                    error = 1;
                    return NULL;
                }
            }
            pop();
            tokens_infix_ptr++;
            continue;
        }
    }
    while (operatorsinstack(stack_base_ptr))
    {
        if (*stack_ptr == '(')
        {
            error = 1;
            return NULL;
        }
        tokens_postfix_ptr->value = pop();
        switch (tokens_postfix_ptr->value)
        {
            case '+': tokens_postfix_ptr->Type = PLUS; break;
            case '-': tokens_postfix_ptr->Type = MINUS; break;
            case '*': tokens_postfix_ptr->Type = MUL; break;
            case '/': tokens_postfix_ptr->Type = DIV; break;
        }
        tokens_postfix_ptr++;
    }
    tokens_postfix_ptr->value = -1;
    return tokens_postfix;
}

int evaluate(Token* tokens_postfix_ptr)
{
    int right = 0;
    int left = 0;
    int result = 0;
    while (tokens_postfix_ptr->value != -1)
    {
        switch (tokens_postfix_ptr->Type)
        {
            case INTEGER:
                push(tokens_postfix_ptr->value);
                break;
            case PLUS:
            {
                int right = pop();
                int left = pop();
                result = left + right;
                push(result);
                break;
            }
            case MINUS:
            {
                int right = pop();
                int left = pop();
                result = left - right;
                push(result);
                break;
            }
            case MUL:
            {
                int right = pop();
                int left = pop();
                result = left * right;
                push(result);
                break;
            }
            case DIV:
            {
                int right = pop();
                int left = pop();
                if (right == 0)
                {
                    error = 1;
                    return result;
                }
                result = left / right;
                push(result);
                break;
            }
        }
        tokens_postfix_ptr++;
    }
    result = pop();
    return result;
}

int main()
{
    char infix_expr[MAX] = {0};
    Token* tokens_infix;
    Token* tokens_postfix;
    int result;

    while (fgets(infix_expr, MAX, stdin) != NULL)
    {
        stack_ptr = stack;
        error = 0;
        if (infix_expr[0] == '\n')
        {
            printf("Empty expression\n");
            continue;
        }

        tokens_infix = tokenize(infix_expr);
        if (error)
        {
            printf("Illegal symbols in the expression\n");
            continue;
        }

        tokens_postfix = parse(tokens_infix);
        if (error)
        {
            printf("Missing parenthesis\n");
            continue;
        }

        result = evaluate(tokens_postfix);
        if (error)
        {
            printf("Division by zero\n");
            continue;
        }

        printf("%d\n", result);
    }

    return 0;
}
