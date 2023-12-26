#include "differ_header.h"
#include "RD_header.hpp"


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


Node* GetG(const char* str, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, nullptr);

    s = str;
    p = 0;

    DEBUG_CALL(GetE, logfile);
    Node* node = GetE(logfile);

    if(s[p] == '\0'){
        return node;
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
    Node* node = nullptr, *node_right = nullptr, *head_node = nullptr;

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

/*
Node* GetT(FILE* logfile){
    DEBUG_ECHO(logfile);

    int op = 0;

    // Node *node = GetP(logfile);
    // Node* node_left = OpNew(VALUE, /*val , logfile);

    DEBUG_CALL(GetP, logfile);
    Node *node_left = GetP(logfile);

    Node *node_right = nullptr, *node = nullptr, *val2 = nullptr;

    while(s[p] == MULTIPLICATION || s[p] == DIVISION){
        DEBUG_ECHO(logfile);

        op = s[p];
        p++;

        DEBUG_CALL(GetP, logfile);
        node_right = GetP(logfile);

        node = OpNew(OP, op, logfile);

        if(s[p] ==

        node->left  = node_left;
        node->right = node_right;

        // switch(op){
        //     case '*':
        //         val = val * val2;
        //         break;
        //     case '/':
        //         val = val / val2;
        //         break;
        //     default:
        //         DEBUG_CALL(syntax_error, logfile);
        //         syntax_error(logfile);
        // }
    }


    return node;
} */

Node* GetT(FILE* logfile){
    DEBUG_ECHO(logfile);

    DEBUG_CALL(GetP, logfile);
    Node *node_left = GetP(logfile);
    Node* node = nullptr;

    if(s[p] == MULTIPLICATION || s[p] == DIVISION){

        DEBUG_CALL(PartialGetT, logfile);
        node = PartialGetT(logfile);

    }

    node->left = node_left;

    return node;
}

Node* PartialGetT(FILE* logfile){
    DEBUG_ECHO(logfile);

    Node *subroot = OpNew(OP, s[p], logfile);

    p++;

    DEBUG_CALL(GetP, logfile);
    Node *node_right = GetP(logfile);
    Node *subsubroot = nullptr;

    if(s[p] == MULTIPLICATION || s[p] == DIVISION){

        subsubroot = PartialGetT(logfile);

        subroot->right = subsubroot;
        subsubroot->left = node_right;
    }else{
        subroot->right = node_right;
    }

    return subroot;
}



Node* GetP(FILE* logfile){
    DEBUG_ECHO(logfile);

    double val = 0;
    Node* node = nullptr;

    if(s[p] == '('){
        p++;
        DEBUG_CALL(GetE, logfile);
        node = GetE(logfile);
        VERIFICATION(node == nullptr, "GetE failed!", logfile, nullptr);
    }else{
        DEBUG_CALL(GetN, logfile);
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

    Node* node = OpNew(VALUE, d_val, logfile);          // Любое число - всегда лист дерева - никакой привязки вниз быть
    VERIFICATION(node == nullptr, "OpNew failed!", logfile, nullptr);                                        // не может.

    return node;
}

Node* syntax_error(FILE* logfile){
    VERIFY_LOGFILE(logfile);

    printf("\033[1;31mSyntax error\033[0m at p = %d: %s\n", p, s + p);
    fprintf(logfile, "Syntax error at p = %d: s = %s\n", p, s + p);

    exit(-1);       // ?..

    return nullptr;
}
