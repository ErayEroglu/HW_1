#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#define HASH_SIZE 1000
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

typedef struct // Token structure to create tokens
{
    TokenType type;
    int number;
    char *id;
    char *name;
} Token;

typedef struct Node // Node structure, it will be used in creating parse trees
{
    TokenType op;
    int *value;
    char *name;
    struct Node *left;
    struct Node *right;
} Node;

typedef struct Variable // struct for a hashtable which will store data for variables
{
    int data;
    char *key;
} Variable;

// global variables and method declarations

Token *createToken(char *inp_s, int *token_number);
int evaluate(Node *nodeP);
Node *createNode(Token *token, Node *left, Node *right);
Node *constructNode(TokenType op, int *value, char *name, Node *left, Node *right);
Node *parseF(Token *ptoken_list, int *pos);
Node *parseT(Token *ptoken_list, int *pos);
Node *parseE(Token *ptoken_list, int *pos);
Node *parse(Token *ptoken_list, int *pos);
Variable *hashMap[HASH_SIZE];
int hashFunction(char *s);
bool printFlag = true;
bool errorFlag = false;
int num_tokens;

int main() // intleri longa çevir!!!!!!
{
    // a = a = a hata vermiyor
    // a )= 10 hata vermiyor 
    // 8 + 5 + hata vermiyor 

    Token *tokens = NULL;
    while (true)
    {
        printf("input : \n");
        int position = 0;
        int *ppos = &position;
        char expr[256];
        fgets(expr, 256, stdin);
        num_tokens = strlen(expr);
        Token *tokens = createToken(expr, &num_tokens);
        
        // printf("Tokens:\n");
        // for (int i = 0; i < num_tokens; i++)
        // {
        //     printf("Token %d: type=%d, id=%s, number=%d\n", i, tokens[i].type, tokens[i].id, tokens[i].number);
        // }

        Node *pnode = parse(tokens, ppos);
        if (!errorFlag)
        {
            int res = evaluate(pnode);
            if (printFlag)
            {
                printf("%d\n", res);
            }
        }
        else
        {
            printf("Error!\n");
        }

        printFlag = true;
        errorFlag = false;
    }
    free(tokens);

    return 45;
}

// helper methods

char peek(char *p) // looks the other char, but does not move the cursor
{
    p++;
    char c = *p;
    p--;
    return c;
}

int hashFunction(char *p)
{
    char *s = NULL;
    s = p;
    int hashval;
    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % HASH_SIZE;
}

Variable *search(char *pkey) // searches for the var name, if it exists returns the variable
{
    int key = hashFunction(pkey);
    while (hashMap[key] != NULL)
    {
        if (strcmp(hashMap[key]->key, pkey) == 0)
        {
            return hashMap[key];
        }
        key++;

        key %= HASH_SIZE;
    }
    return NULL;
}

Variable *createVar(char *key, int data)
{ // method to create variable
    Variable *var = malloc(sizeof(Variable));
    var->data = data;
    var->key = strdup(key);
    return var;
}

void insert(char *key, int data) // inserting function for hashmap
{
    Variable *var = createVar(key, data);
    int hash_pos = hashFunction(key);

    while (hashMap[hash_pos] != NULL)
    {
        hash_pos++;
        hash_pos %= HASH_SIZE;
    }
    hashMap[hash_pos] = var;
}

Token *createToken(char *inp_s, int *token_number) // creates token according to the given input string, one token each time
{                                                  // returns the list of tokens
    int found_tokens = 0;
    int length = strlen(inp_s);
    char *pcurrent_char = inp_s;
    Token *token_list = malloc(length * sizeof(Token)); // creates sufficient memory for tokens

    while (*pcurrent_char != '\0') // iterates until reaching the end of input
    {
        switch (*pcurrent_char) // creates tokens according to current char
        {
        case ' ':
            pcurrent_char++;
            break;
        case '\t':
            pcurrent_char++;
            break;
        case '\n':
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
        case ',':
            token_list[found_tokens].type = COMMA;
            token_list[found_tokens].id = "COMMA";
            found_tokens++;
            pcurrent_char++;
            break;
        default:
            if (isdigit(*pcurrent_char))
            {
                int num = atoi(pcurrent_char);
                pcurrent_char++;
                while (isdigit(*pcurrent_char)) // if it is a number, it might be consisted of more than one digit
                {                               // so it will be iterated until reaching a non-digit char
                    pcurrent_char++;
                }
                token_list[found_tokens].type = CONST;
                token_list[found_tokens].id = "CONST";
                token_list[found_tokens].number = num;
                found_tokens++;
            }
            else if (isalpha(*pcurrent_char)) // if it is a letter, it might be consisted of more than one letter
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
                if (strcmp(char_name, "xor") == 0) // if it is a special function name, creates the special token
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
                    token_list[found_tokens].type = VAR; // if it is not a function name, than it is a variable
                    token_list[found_tokens].id = "VAR";
                    token_list[found_tokens].name = strdup(char_name);
                    token_list[found_tokens].number = 0;
                }
                found_tokens++;
            }
            break;
        }
        
    }
    *token_number = found_tokens;

    return token_list; // returns the list of tokens
}

// Node *constructNode(TokenType op, int *value, char *name, Node *left, Node *right)
// {
//     Node *node;
//     node->op = op;
//     node->value = value;
//     node->name = name;
//     node->left = left;
//     node->right = right;
//     return node;
// }

// Node *createNode(Token *token, Node *left, Node *right) // calls node constructor and creates node
// {
//     TokenType op = (token->type);
//     int *value = &(token->number);
//     char *name = (token->name);
//     return constructNode(op, value, name, left, right);
// }

Node *constructNode(TokenType op, int *value, char *name, Node *left, Node *right)
{
    Node *node = malloc(sizeof(Node));
    node->op = op;
    node->value = malloc(sizeof(int));
    *(node->value) = *value;
    node->name = strdup(name);

    node->left = left;

    node->right = right;

    return node;
}

Node *createNode(Token *token, Node *left, Node *right)
{

    TokenType op = token->type;
    int *value = malloc(sizeof(int));
    *value = token->number;
    if (op == VAR)
    {
        char *name = strdup(token->name);
        return constructNode(op, value, name, left, right);
    }
    Node *node = constructNode(op, value, "", left, right);
    return node;
}

// parsing functions

Node *parse(Token *ptoken_list, int *pos)
{
    Node *temp = parseE(ptoken_list, pos);

    // error check
    if (temp == NULL)
    {
        errorFlag = true;
        return NULL;
    }
    if (ptoken_list[*pos].type == EQUAL)
    {
        if (temp->op != VAR || ptoken_list[0].type == L_PAREN)
        {
            errorFlag = true;
            return NULL;
        }

        Token *op_token = &(ptoken_list[*pos]);
        (*pos)++;
        Node *temp2 = parseE(ptoken_list, pos);
        temp = createNode(op_token, temp, temp2);
    }
    // if (&ptoken_list[*pos] != NULL) {
    //     errorFlag = true;
    //     return NULL;
    // }
    if(*pos < num_tokens){
        errorFlag = true;
        return NULL;
    }


    return temp;
}

Node *parseE(Token *ptoken_list, int *pos) // parses expression into terms
{
    Node *parsing_term = parseT(ptoken_list, pos);

    // error check
    if (parsing_term == NULL)
    {
        errorFlag = true;
        return NULL;
    }

    while (ptoken_list[*pos].type == ADDITION || ptoken_list[*pos].type == SUBTRACTION)
    {
        Token *op_token = &(ptoken_list[*pos]);
        (*pos)++;
        if (*pos == num_tokens){
            //printf("%d", *pos);
            errorFlag = true;
            return NULL;
        }
        Node *parsing_term2 = parseT(ptoken_list, pos);
        
        if (parsing_term2 == NULL)
        {
            // error check
            errorFlag = true;
            return NULL;
        }
        parsing_term = createNode(op_token, parsing_term, parsing_term2);
    }

    return parsing_term;
}

Node *parseT(Token *ptoken_list, int *pos) // parses term into factors
{
    Node *parsing_factor = parseF(ptoken_list, pos);

    // error check
    if (parsing_factor == NULL)
    {
        errorFlag = true;
        return NULL;
    }
    
    while (ptoken_list[*pos].type == MULTIPLICATION)
    {
        Token *op_token = &(ptoken_list[*pos]);

        (*pos)++;
        if (*pos == num_tokens){
            errorFlag = true;
            return NULL;
        }
        Node *parsing_factor2 = parseF(ptoken_list, pos);
        parsing_factor = createNode(op_token, parsing_factor, parsing_factor2);
    }
    return parsing_factor;
}

Node *parseF(Token *ptoken_list, int *pos) // parsing factor method
{

    if (ptoken_list[*pos].type == CONST) // if the current token matches the type, creates node
    {

        Node *temp = createNode(&(ptoken_list[*pos]), NULL, NULL);
        (*pos)++;
        // if (ptoken_list[*pos].type == CONST)
        return temp;
    }
    else if (ptoken_list[*pos].type == VAR)
    {
        insert(ptoken_list[*pos].name, ptoken_list[*pos].number);
        Node *temp = createNode(&(ptoken_list[*pos]), NULL, NULL);
        (*pos)++;
        return temp;
    }
    else if (ptoken_list[*pos].type == L_PAREN) // if token is l paren, it must be an expression inside of it
    {
        (*pos)++; // it moves the next token
        Node *temp = parseE(ptoken_list, pos);
        // !!! We will probably need an error check here
        // TODO
        //
        if (temp == NULL || temp->op == R_PAREN)
        { // if there does't exist a statement, return null
            errorFlag = true;
            return NULL;
        }
        else if (ptoken_list[*pos].type == R_PAREN)
        {
            (*pos)++;
            return temp;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

// method to evaluate the tree
int evaluate(Node *nodeP)
{

    // start evaluating at root node, evaluate the tree recursively

    // binary operations
    if (nodeP->op == ADDITION)
    {
        return evaluate(nodeP->left) + evaluate(nodeP->right);
    }
    else if (nodeP->op == SUBTRACTION)
    {
        return evaluate(nodeP->left) - evaluate(nodeP->right);
    }
    else if (nodeP->op == MULTIPLICATION)
    {
        return evaluate(nodeP->left) * evaluate(nodeP->right);
    }

    else if (nodeP->op == CONST)
    {
        return *(nodeP->value);
    }
    else if (nodeP->op == VAR)
    {
        Variable *var = search(nodeP->name);
        if (var != NULL)
        {
            return var->data;
        }
        return 0;
    }

    else if (nodeP->op == EQUAL)
    {
        Node *pLeft;
        pLeft = nodeP->left;

        // burası muhtemelen silinecek
        // error condition
        // if (pLeft->op != VAR)
        // {
        //     // print error
        //     return -1;
        // }

        Variable *pVar = search(pLeft->name);

        pVar->data = evaluate(nodeP->right);
        printFlag = false;
        return pVar->data;
    }

    // bitwise binary operations
    else if (nodeP->op == AND)
    {
        return evaluate(nodeP->left) & evaluate(nodeP->right);
    }
    else if (nodeP->op == OR)
    {
        return evaluate(nodeP->left) | evaluate(nodeP->right);
    }
    // function with one parameter
    else if (nodeP->op == NOT)
    {
        // update after parse implementation
        return 0;
    }

    // functions with two parameters
    else
    {
        Node *pLeft = nodeP->left;
        Node *pRight = nodeP->right;
        bool syntaxError = false;
        if (pLeft->op != VAR || pLeft->op != CONST)
            switch (nodeP->op)
            {
            case XOR:
                return evaluate(nodeP->left) ^ evaluate(nodeP->right);
                break;
            case L_SHIFT:
                return evaluate(nodeP->left) << evaluate(nodeP->right);
            case R_SHIFT:
                return evaluate(nodeP->left) >> evaluate(nodeP->right);
            case L_ROTATE:
                return (evaluate(nodeP->left) << evaluate(nodeP->right)) | (evaluate(nodeP->left) >> (32 - evaluate(nodeP->right)));
            case R_ROTATE:
                return (evaluate(nodeP->left) >> evaluate(nodeP->right)) | (evaluate(nodeP->left) << (32 - evaluate(nodeP->right)));
            default:
                return -1;
                break;
            }
    }
}
