#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_SIZE 128

#define DEBUG_ECHO(logfile) do{ fprintf(logfile, "I am %s, p = %d, next symbols is %c\n", __FUNCTION__, data->p, data->s[data->p]); }while(0)
#define DEBUG_CALL(next_func, logfile) do{ fprintf(logfile, "I am %s, calling " #next_func "\n", __FUNCTION__);}while(0)

struct Lexema{
    int type = 0;
    int val = 0;
    char name[128] = {};
};

struct RD_output{
    Node* node = nullptr;
    unsigned int vars_count = 0;
};

struct RD_data{
    int p = 0;
    const char* s = NULL;
    unsigned int vars_count = 0;
    char **vars = nullptr;
};


Node* syntax_error(RD_data *data, FILE* logfile);

RD_output* GetG(const char* str, FILE* logfile);

Node* GetE(RD_data *data, FILE* logfile);

Node* GetT(RD_data *data, FILE* logfile);

Node* GetP(RD_data *data, FILE* logfile);

Node* GetN(RD_data *data, FILE* logfile);

Node* PartialGetE(RD_data *data, FILE* logfile);

Node* PartialGetT(RD_data *data, FILE* logfile);

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
