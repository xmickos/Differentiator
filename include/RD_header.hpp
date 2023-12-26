#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_SIZE 128

#define DEBUG_ECHO(logfile) do{ fprintf(logfile, "I am %s, p = %d, next symbols is %c\n", __FUNCTION__, p, s[p]); }while(0)
#define DEBUG_CALL(next_func, logfile) do{ fprintf(logfile, "I am %s, calling " #next_func "\n", __FUNCTION__);}while(0)

struct Lexema{
    int type = 0;
    int val = 0;
    char name[128] = {};
};

typedef struct page_t * (*slow_getpage_t)();


Node* syntax_error(FILE* logfile);

Node* GetG(const char* str, FILE* logfile);

Node* GetE(FILE* logfile);

Node* GetT(FILE* logfile);

Node* GetP(FILE* logfile);

Node* GetN(FILE* logfile);

Node* PartialGetE(FILE* logfile);

Node* PartialGetT(FILE* logfile);

inline double d_pow(double d, int k){

    if(k == 0) return 1.;
    if(k == 1) return d;

    double val = d;
    while(k > 1){
        val = val * d;
        k--;
    }

    return val;
}


/*
    G ::= E'\0'
    E ::= T{[+-]T}*
    T ::= P{[*\]P}*
    P ::= '('E')' | N
    N ::= ['0'-'9']+ | ['0'-'9']+'.'['0'-'9']+ <– Не уверен, что в RBRF это записывается именно так, но здесь я пытался описать
    правила написания дробных чисел в десятичной записи. Дробных или целых, если быть точным.


*/
