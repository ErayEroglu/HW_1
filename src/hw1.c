#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


// structure definitions

typedef enum
{ // Token types which will be needed in lexical analysis
    CONST,
    VAR,
    COMMA,
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    AND,
    OR,
    XOR,
    EQUAL,
    L_SHIFT,
    R_SHIFT,
    L_ROTATE,
    R_ROTATE,
    NOT,
    L_PAREN,
    R_PAREN,
    EOI,
} TokenType;

typedef struct  // Token structure to create tokens
{
    TokenType type;
    int number;
    char *id;
} Token;

typedef struct  // Node structure, it will be used in creating parse trees
{
    TokenType type;
    int val;
    char var;
    struct node *left;
    struct node *right;
} Node;

// global variables and method declarations

int inp_length;
Token *createToken(char *inp_s, int *token_number);

int main()
{
    char expr[256];
    printf("input : \n");
    fgets(expr,256,stdin);
    int num_tokens = strlen(expr);
    Token *tokens = createToken(expr, &num_tokens);

    printf("Tokens:\n");
    for (int i = 0; i < num_tokens; i++)
    {
        printf("Token %d: type=%d, id=%s, number=%d\n", i, tokens[i].type, tokens[i].id, tokens[i].number);
    }

    printf("\n");

    free(tokens);
    return 45;
}

// operations

int addition(int a, int b)
{
    return a + b;
}
int subtraction(int a, int b)
{
    return a - b;
}

int multiplication(int a, int b)
{
    return a * b;
}

int bitwiseAnd(int a, int b)
{
    return a & b;
}

int bitwiseOr(int a, int b)
{
    return a || b;
}

int bitwiseXor(int a, int b)
{
    return a ^ b;
}

int complement(int a)
{
    return ~a;
}

int leftShift(int a, int i)
{
    return a << i;
}

int rightShift(int a, int i)
{
    return a >> i;
}

int leftRotate(int a, int i)
{
    // TODO
}

int rightRotate(int a, int i)
{
    // TODO
}

// helper methods

// char nextOperation(char *ch)
// {
//     while (1)
//     {
//         printf("inner while \n");
//         if (*ch == ' ')
//             continue;
//         else if (*ch == '+' || *ch == '-' || *ch == '*' || *ch == '&' || *ch == '|')
//             return *ch;
//         else if (*ch == 'x')
//         {
//             if (*(ch++) == 'o' && *(ch++) == 'r')
//                 return *ch;
//         }
//         else
//             break;
//     }
// }

char peek(char *p) // looks the other char, but does not move the cursor
{
    p++;
    char c = *p;
    p--;
    return c;
}

// !!! her tokenizeın sonunda ekstra virgül ve intlerin değeri yanlış
Token *createToken(char *inp_s, int *token_number) // creates token according to the given input string, one token each time
{                                                  // returns the list of tokens
    int found_tokens = 0;
    int length = strlen(inp_s);
    char *pcurrent_char = inp_s;
    Token *token_list = malloc(length * sizeof(Token));  // creates sufficient memory for tokens

    while (*pcurrent_char != '\0')  // iterates until reaching the end of input
    {
        switch (*pcurrent_char)  // creates tokens according to current char
        {
        case ' ':
            pcurrent_char++;
            break;
        case '\t':
        case '\n':
        case ',':
            token_list[found_tokens].type = COMMA;
            token_list[found_tokens].id = "COMMA";
            found_tokens++;
            pcurrent_char++;
            break;
        case '+':
            token_list[found_tokens].type = ADDITION;
            token_list[found_tokens].id = "ADDITION";
            found_tokens++;
            pcurrent_char++;
            break;
        case '-':
            token_list[found_tokens].type = SUBTRACTION;
            token_list[found_tokens].id = "SUBTRACTION";
            found_tokens++;
            pcurrent_char++;
            break;
        case '*':
            token_list[found_tokens].type = MULTIPLICATION;
            token_list[found_tokens].id = "MULTIPLICATION";
            found_tokens++;
            pcurrent_char++;
            break;
        case '&':
            token_list[found_tokens].type = AND;
            token_list[found_tokens].id = "AND";
            found_tokens++;
            pcurrent_char++;
            break;
        case '|':
            token_list[found_tokens].type = OR;
            token_list[found_tokens].id = "OR";
            found_tokens++;
            pcurrent_char++;
            break;
        case '(':
            token_list[found_tokens].type = L_PAREN;
            token_list[found_tokens].id = "L_PAREN";
            found_tokens++;
            pcurrent_char++;
            break;
        case ')':
            token_list[found_tokens].type = R_PAREN;
            token_list[found_tokens].id = "R_PAREN";
            found_tokens++;
            pcurrent_char++;
            break;
        case '%':
            while (*pcurrent_char != '\0')
            {
                pcurrent_char++;
            };
            break;
        case '=':
            token_list[found_tokens].type = EQUAL;
            token_list[found_tokens].id = "EQUAL";
            found_tokens++;
            pcurrent_char++;
            break;
        default:
            if (isdigit(*pcurrent_char))  
            {
                int num;
                num = (*pcurrent_char) - '\0';
                pcurrent_char++;
                while (isdigit(*pcurrent_char)) // if it is a number, it might be consisted of more than one digit
                {                               // so it will be iterated until reaching a non-digit char
                    num = num * 10 + *(pcurrent_char) - '\0';
                    pcurrent_char++;
                }
                token_list[found_tokens].type = CONST;
                token_list[found_tokens].id = "CONST";
                token_list[found_tokens].number = num;
                found_tokens++;
            }
            else if (isalpha(*pcurrent_char))  // if it is a letter, it might be consisted of more than one letter
            {                                   
                char char_name[256];
                char_name[0] = *pcurrent_char;
                int index = 1;
                pcurrent_char++;
                while (isalpha(*pcurrent_char))
                {
                    char_name[index] = *pcurrent_char;
                    pcurrent_char++;
                    index++;
                }
                char_name[index] = '\0';
                if (strcmp(char_name, "xor") == 0)  // if it is a special function name, creates the special token
                {
                    token_list[found_tokens].type = XOR;
                    token_list[found_tokens].id = "XOR";
                }

                else if (strcmp(char_name, "ls") == 0)
                {
                    token_list[found_tokens].type = L_SHIFT;
                    token_list[found_tokens].id = "L_SHIFT";
                }

                else if (strcmp(char_name, "rs") == 0)
                {
                    token_list[found_tokens].type = R_SHIFT;
                    token_list[found_tokens].id = "R_SHIFT";
                }

                else if (strcmp(char_name, "lr") == 0)
                {
                    token_list[found_tokens].type = L_ROTATE;
                    token_list[found_tokens].id = "L_ROTATE";
                }

                else if (strcmp(char_name, "rr") == 0)
                {
                    token_list[found_tokens].type = R_ROTATE;
                    token_list[found_tokens].id = "R_ROTATE";
                }

                else if (strcmp(char_name, "not") == 0)
                {
                    token_list[found_tokens].type = NOT;
                    token_list[found_tokens].id = "NOT";
                }

                else
                {
                    token_list[found_tokens].type = VAR;  // if it is not a function name, than it is a variable
                    token_list[found_tokens].id = "VAR";
                }
                found_tokens++;
            }
            break;
        }
    }
    *token_number = found_tokens;
    return token_list;  // returns the list of tokens
}
