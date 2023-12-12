#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_SIZE 128

#define VERIFY_LOGFILE(logfile) do{ if(logfile == nullptr){ printf("\033[1;31mWe've lost logfile!\033[0m\n "); return -1;}} while(0)

#define DEBUG_ECHO(logfile) do{ fprintf(logfile, "I am %s, p = %d, next symbols is %c\n", __FUNCTION__, p, s[p]); }while(0)
#define DEBUG_CALL(next_func, logfile) do{ fprintf(logfile, "I am %s, calling " #next_func "\n", __FUNCTION__);}while(0)

struct Lexema{
    int type = 0;
    int val = 0;
    char name[128] = {};
};

int syntax_error(FILE* logfile);

double GetG(const char* str, FILE* logfile);

double GetE(FILE* logfile);

double GetT(FILE* logfile);

double GetP(FILE* logfile);

double GetN(FILE* logfile);

inline double d_pow(double d, int k){

    while(k > 0){
        d = d * d;
        k--;
    }

    return d;
}


/*
    G ::= E'\0'
    E ::= T{[+-]T}*
    T ::= P{[*\]P}*
    P ::= '('E')' | N
    N ::= ['0'-'9']+ | ['0'-'9']+'.'['0'-'9']+ <– Не уверен, что в RBRF это записывается именно так, но здесь я пытался описать
    правила написания дробных чисел в десятичной записи. Дробных или целых, если быть точным.


*/
