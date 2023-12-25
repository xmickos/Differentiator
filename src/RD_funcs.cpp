#include "RD_header.hpp"
#include "differ_header.h"


int p = 0;
const char* s = NULL;


/*
I   итерация: односимвольные числа
    G ::= N'\0'
    N ::= ['0'-'9']         // пустые строчки разрешены ? Не помню

II  итерация: многосимвольные числа
    G ::= N'\0'
    N :: ['0'-'9']+

III итерация: сложение и вычитание
    G ::= E'\0'
    E ::= N{[+-]N}*
    N ::= ['0'-'9']+

IV  итерация: умножение и деление с приоритетами, без скобок
    G ::= E'\0'     -- grammatic
    E ::= T{[+-]T}* -- expression
    T ::= N{[*\]N}* -- term
    N ::= ['0'-'9'] -- numeral


V  итерация: умножение и деление со скобками
    G ::= E'\0'
    E ::= T{[+-]T}*
    T ::= P{[*\]P}*
    P ::= '('E')' | N
    N ::= ['0'-'9']+

    (2+5)*(2-5)'\0'
*/


double GetG(const char* str, FILE* logfile){
    VERIFY_LOGFILE(logfile);

    s = str;
    p = 0;

    DEBUG_CALL(GetE, logfile);
    double val = GetE(logfile);

    if(s[p] == '\0'){
        return val;
    }

    DEBUG_CALL(syntax_error, logfile);
    return syntax_error(logfile);
}

Node* GetE(FILE* logfile){
    DEBUG_ECHO(logfile);

    DEBUG_CALL(GetT, logfile);
    // double val = GetT(logfile),
    double val2 = 0;
    int op = 0;
    Node* node_left = GetT(logfile);
    Node* node = nullptr, node_right = nullptr, head_node = nullptr;

    while(s[p] == SUMM || s[p] == SUBTRACTION){
        DEBUG_ECHO(logfile);

        op = s[p];
        p++;
        node = OpNew(OP, s[p], logfile);

        /*

        Обработать случай с тремя и более слагаемыми

        */

        DEBUG_CALL(GetT, logfile);

        node_right = GetT(logfile);

        if(op != SUMM && op != SUBTRACTION){
            DEBUG_CALL(syntax_error, logfile);
            syntax_error(logfile);
        }

        node->left  = node_left;
        node->right = node_right;
        head_node->left = node; // !!! *nullptr!
        node = node_left;  //?..
    }

    return node;
}

Node* GetT(FILE* logfile){
    DEBUG_ECHO(logfile);

    DEBUG_CALL(GetP, logfile);
    double val = GetP(logfile), val2 = 0;
    int op = 0;
    Node* node_left = OpNew(VALUE, val, logfile);
    Node* node_right = nullptr, node = nullptr;

    while(s[p] == MULTIPLICATION || s[p] == DIVISION){
        DEBUG_ECHO(logfile);

        op = s[p];
        p++;

        DEBUG_CALL(GetP, logfile);
        val2 = GetP(logfile);

        node_right = OpNew(VALUE, val2, logfile);
        node = OpNew(OP, op, logfile);

        switch(op){
            case '*':
                val = val * val2;
                break;
            case '/':
                val = val / val2;
                break;
            default:
                DEBUG_CALL(syntax_error, logfile);
                syntax_error(logfile);
        }
    }

    node->left  = node_left;
    node->right = node_right;

    return node;
}

Node* GetP(FILE* logfile){
    DEBUG_ECHO(logfile);

    double val = 0;
    Node* node = nullptr;

    if(s[p] == '('){
        p++;
        node = GetE(logfile);
        VERIFICATION(node == nullptr, "GetE failed!", logfile, nullptr);
    }else{
        return GetN(logfile);
    }
    if(s[p] == ')'){
        p++;
        return node;
    }

    return nullptr;
}

Node* GetN(FILE* logfile){
    DEBUG_ECHO(logfile);

    int val = 0;

    if(s[p] - '0' > 9 || s[p] - '0' < 0){
        syntax_error(logfile);                  //Хотя бы одна цифра в начале должна быть,
    }                                           //иначе syntax_error; Тогда пустые строки запрещены

    while(s[p] - '0' <= 9 && s[p] - '0' >= 0){
        val = 10*val + s[p] - '0';
        p++;
    }

    double d_val = (double)val;
    int k = 0;

    DEBUG_ECHO(logfile);
    if(s[p] == '.'){
        p++;

        while(s[p] - '0' <= 9 && s[p] - '0' >= 0){
            DEBUG_ECHO(logfile);
            d_val = d_val + (double)(s[p] - '0') / d_pow(10.0, k);
            fprintf(logfile, "current d_val is %f\n", d_val);
            p++;
            k++;
        }
    }

    Node* node = OpNew(VALUE, d_val, logfile);
    VERIFICATION(node == nullptr, "OpNew failed!", logfile, nullptr);

    return node;
}

int syntax_error(FILE* logfile){
    VERIFY_LOGFILE(logfile);

    printf("\033[1;31mSyntax error\033[0m at p = %d: %s\n", p, s + p);
    fprintf(logfile, "Syntax error at p = %d: s = %s\n", p, s + p);

    exit(-1);       // ?..

    return 0;
}
