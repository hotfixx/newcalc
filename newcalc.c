#include <stdio.h>
#include <string.h>

#define MAX 256

enum Type {INTEGER=1, PLUS=43, MINUS=45, MUL=42, DIV=47, OPENPAR=40, CLOSPAR=41};

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

// char findlen(Token* tokens)
// {
//     char numtokens = 0;
//     Token* p = tokens;
//     for (; p->value != -1; p++)
//         numtokens++;
//     return numtokens;
// }

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
            *infix_expr == ')' || *infix_expr >= '0' && *infix_expr <= '9'))
        {
            infix_expr++;
            continue;
        }

        if (*infix_expr == '+')
        {
            tokens->Type = PLUS;
            tokens->value = '+';
            infix_expr++;
            tokens++;
        }

        if (*infix_expr == '-')
        {
            tokens->Type = MINUS;
            tokens->value = '-';
            infix_expr++;
            tokens++;
        }

        if (*infix_expr == '*')
        {
            tokens->Type = MUL;
            tokens->value = '*';
            infix_expr++;
            tokens++;
        }

        if (*infix_expr == '/')
        {
            tokens->Type = DIV;
            tokens->value = '/';
            infix_expr++;
            tokens++;
        }

        if (*infix_expr == '(')
        {
            tokens->Type = OPENPAR;
            tokens->value = '(';
            infix_expr++;
            tokens++;
        }

        if (*infix_expr == ')')
        {
            tokens->Type = CLOSPAR;
            tokens->value = ')';
            infix_expr++;
            tokens++;
        }

        while (*infix_expr >= '0' && *infix_expr <= '9')
        {
            if (tokens->Type != INTEGER)
            {
                tokens->Type = INTEGER;
                tokens->value = 0;
            }

            tokens->value = (tokens->value * 10) + ((*infix_expr) - '0');
            infix_expr++;

            if (!(*infix_expr >= '0' && *infix_expr <= '9'))
                tokens++;
        }
    }
    tokens->value = -1; // end
    return tokens_infix;
}

Token* parse(Token* tokens_infix_ptr)
{
    static Token tokens_postfix[MAX] = {0};
    Token* tokens_postfix_ptr = tokens_postfix;

    int* stack_base_ptr = stack_ptr;

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
                    return NULL;
            }
            pop();
            tokens_infix_ptr++;
            continue;
        }
    }
    while (operatorsinstack(stack_base_ptr))
    {
        if (*stack_ptr == '(')
            return NULL;
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
    int result = 0;
    while (tokens_postfix_ptr->value != -1)
    {
        if (tokens_postfix_ptr->Type == INTEGER)
            push(tokens_postfix_ptr->value);

        if (tokens_postfix_ptr->Type == PLUS)
        {
            int right = pop();
            int left = pop();
            result = left + right;
            push(result);
        }
        if (tokens_postfix_ptr->Type == MINUS)
        {
            int right = pop();
            int left = pop();
            result = left - right;
            push(result);
        }
        if (tokens_postfix_ptr->Type == MUL)
        {
            int right = pop();
            int left = pop();
            result = left * right;
            push(result);
        }
        if (tokens_postfix_ptr->Type == DIV)
        {
            int right = pop();
            int left = pop();
            result = left / right;
            push(result);
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
        tokens_infix = tokenize(infix_expr);
        tokens_postfix = parse(tokens_infix);
        if (tokens_postfix == NULL)
        {
            printf("Missing parenthesis\n");
            break;
        }
        result = evaluate(tokens_postfix);
        printf("%d\n", result);
    }

    return 0;
}
