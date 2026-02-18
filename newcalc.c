#include <stdio.h>
#include <string.h>

#define MAX 256

enum ErrorCode {ERR_DIVBYZERO=-3, ERR_MISPAREN, ERR_BADEXPR};

enum Type {INTEGER=1, OPERATOR, OPENPAR='(', CLOSPAR=')', TOKEN_END=-1};

typedef struct
{
    enum Type tokentype;
    int value;
} Token;

int stack[MAX] = {0};
int* stack_ptr = stack;
int* stack_base_ptr = stack;

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

char operatorsinstack()
{
    int* p = stack_ptr;
    for (; p != stack_base_ptr; p--)
    {
        if (*p == '+' || *p == '-' || *p == '*' || *p == '/') 
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

char findlen(Token* tokens_ptr)
{
    char numtokens = 0;
    Token* p = tokens_ptr;
    for (; p->tokentype != TOKEN_END; p++)
        numtokens++;
    return numtokens;
}

int tokenize(char* infix_expr, Token* tokens_infix_ptr)
{
    Token* tokens_infix_base_ptr = tokens_infix_ptr;

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
            case '-':
            case '*':
            case '/':
                tokens_infix_ptr->tokentype = OPERATOR;
                tokens_infix_ptr->value = *infix_expr;
                infix_expr++;
                tokens_infix_ptr++;
                break;
            case '(':
                tokens_infix_ptr->tokentype = OPENPAR;
                tokens_infix_ptr->value = *infix_expr;
                infix_expr++;
                tokens_infix_ptr++;
                break;
            case ')':
                tokens_infix_ptr->tokentype = CLOSPAR;
                tokens_infix_ptr->value = *infix_expr;
                infix_expr++;
                tokens_infix_ptr++;
                break;

            default:
                if (*infix_expr >= '0' && *infix_expr <= '9')
                {
                    tokens_infix_ptr->tokentype = INTEGER;
                    tokens_infix_ptr->value = 0;

                    while (*infix_expr >= '0' && *infix_expr <= '9')
                    {
                        tokens_infix_ptr->value = (tokens_infix_ptr->value * 10) + ((*infix_expr) - '0');
                        infix_expr++;
                    }
                    tokens_infix_ptr++;
                }
                break;
        }
    }
    tokens_infix_ptr->tokentype = TOKEN_END; // end

    char numtokens;
    if ((numtokens = findlen(tokens_infix_base_ptr)) == 0)
    {
        printf("Illegal symbols in the expression\n");
        return ERR_BADEXPR;
    }
    return 0;
}
// Shunting yard algorithm
int parse(Token* tokens_infix_ptr, Token* tokens_postfix_ptr)
{
    while (tokens_infix_ptr->tokentype != TOKEN_END)
    {
        if (tokens_infix_ptr->tokentype == INTEGER)
        {
            tokens_postfix_ptr->tokentype = INTEGER;
            tokens_postfix_ptr->value = tokens_infix_ptr->value;
            tokens_infix_ptr++;
            tokens_postfix_ptr++;
            continue;
        }
        if (tokens_infix_ptr->tokentype == OPERATOR)
        {
            while (priority(*stack_ptr) != 0 && priority(*stack_ptr) >= priority(tokens_infix_ptr->value))
            {
                tokens_postfix_ptr->tokentype = OPERATOR;
                tokens_postfix_ptr->value = pop();
                tokens_postfix_ptr++;
            }
            push(tokens_infix_ptr->value);
            tokens_infix_ptr++;
            continue;
        }

        if (tokens_infix_ptr->tokentype == OPENPAR)
        {
            push(tokens_infix_ptr->value);
            tokens_infix_ptr++;
            continue;
        }

        if (tokens_infix_ptr->tokentype == CLOSPAR)
        {
            while (*stack_ptr != OPENPAR)
            {
                tokens_postfix_ptr->tokentype = OPERATOR;
                tokens_postfix_ptr->value = pop();
                tokens_postfix_ptr++;
                if (stack_ptr == stack_base_ptr)
                {
                    printf("Missing parenthesis\n");
                    return ERR_MISPAREN;
                }
            }
            pop();
            tokens_infix_ptr++;
            continue;
        }
    }
    while (operatorsinstack())
    {
        if (*stack_ptr == OPENPAR)
        {
            printf("Missing parenthesis\n");
            return ERR_MISPAREN;
        }
        tokens_postfix_ptr->value = pop();
        switch (tokens_postfix_ptr->value)
        {
            case '+':
            case '-':
            case '*':
            case '/': tokens_postfix_ptr->tokentype = OPERATOR; break;
        }
        tokens_postfix_ptr++;
    }
    tokens_postfix_ptr->tokentype = TOKEN_END;
    return 0;
}

int evaluate(Token* tokens_postfix_ptr, int* result)
{
    while (tokens_postfix_ptr->tokentype != TOKEN_END)
    {
        switch (tokens_postfix_ptr->tokentype)
        {
            case INTEGER:
                push(tokens_postfix_ptr->value);
                break;
            
            case OPERATOR:
            {
                int right = pop();
                int left = pop();
        
                switch (tokens_postfix_ptr->value)
                {
                    case '+': *result = left + right; break;
                    case '-': *result = left - right; break;
                    case '*': *result = left * right; break;
                    case '/':
                        if (right == 0)
                        {
                            printf("Division by zero\n");
                            return ERR_DIVBYZERO;
                        }
                        *result = left / right;
                        break;
                }
                push(*result);
                break;
            }
        }
        tokens_postfix_ptr++;
    }
    *result = pop();
    return 0;
}

static inline void clear_infix_expr(char* infix_expr_ptr)
{
    for (; *infix_expr_ptr != '\0'; infix_expr_ptr++) *infix_expr_ptr = 0;
}

int main()
{
    char infix_expr[MAX] = {0};

    while (fgets(infix_expr, MAX, stdin) != NULL)
    {
        Token tokens_infix[MAX] = {0};
        Token tokens_postfix[MAX] = {0};
        int result;

        if (infix_expr[0] == '\n')
        {
            printf("Empty expression\n");
            continue;
        }

        if (tokenize(infix_expr, tokens_infix)) continue;

        if (parse(tokens_infix, tokens_postfix)) continue;

        if (evaluate(tokens_postfix, &result)) continue;

        printf("%d\n", result);

        clear_infix_expr(infix_expr);
        stack_ptr = stack;
    }

    return 0;
}
