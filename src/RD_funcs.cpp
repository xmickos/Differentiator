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
    Node *node_left = GetT(logfile);
    Node *node = nullptr;

    if(s[p] == SUMM || s[p] == SUBTRACTION){

        DEBUG_CALL(PartialGetE, logfile);
        node = PartialGetE(logfile);

        node->left = node_left;

        return node;
    }else{
        return node_left;
    }

}

Node* PartialGetE(FILE* logfile){
    DEBUG_ECHO(logfile);

    Node *subroot = OpNew(OP, s[p], logfile);

    p++;

    DEBUG_CALL(GetT, logfile);
    Node *node_right = GetT(logfile);
    Node *subsubroot = nullptr;

    if(s[p] == SUMM || s[p] == SUBTRACTION){

        DEBUG_CALL(PartialGetE, logfile);
        subsubroot = PartialGetE(logfile);

        subroot->right = subsubroot;
        subsubroot->left = node_right;
    }else{
        subroot->right = node_right;
    }

    return subroot;
}

/*
Node* GetT(FILE* logfile){
    DEBUG_ECHO(logfile);

    int op = 0;

    // Node *node = GetP(logfile);
    // Node* node_left = OpNew(VALUE, val , logfile);

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

        node->left = node_left;

        return node;
    }else{
        return node_left;
    }


}

Node* PartialGetT(FILE* logfile){
    DEBUG_ECHO(logfile);

    Node *subroot = OpNew(OP, s[p], logfile);

    p++;

    DEBUG_CALL(GetP, logfile);
    Node *node_right = GetP(logfile);

    if(node_right->data_flag){
        fprintf(logfile, "I am %s, got node with val = %f from GetP\n", __FUNCTION__, node_right->data.value);
    }

    Node *subsubroot = nullptr;

    if(s[p] == MULTIPLICATION || s[p] == DIVISION){

        DEBUG_CALL(PartialGetT, logfile);
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

    Node* node = nullptr;

    if(s[p] == '('){
        p++;
        DEBUG_CALL(GetE, logfile);
        node = GetE(logfile);
        VERIFICATION(node == nullptr, "GetE failed!", logfile, nullptr);
        fprintf(logfile, "I am %s, got from GetN node with val = %f\n", __FUNCTION__, node->data.value);
    }else{
        DEBUG_CALL(GetN, logfile);
        Node *debug_variable = GetN(logfile);
        fprintf(logfile, "I am %s, got from GetN node with val = %f\n", __FUNCTION__, debug_variable->data.value);
        return debug_variable;
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
    int k = 1;

    DEBUG_ECHO(logfile);
    fprintf(logfile, "curr d_val before evaluating fractional part: %f\n", d_val);
    if(s[p] == '.'){
        p++;

        while(s[p] - '0' <= 9 && s[p] - '0' >= 0){
            DEBUG_ECHO(logfile);
            fprintf(logfile, "[%s, %d]: s[p] - \'0\' == %d\n", __FUNCTION__, __LINE__, s[p] - '0');
            d_val = d_val + (double)(s[p] - '0') / d_pow(10.0, k);
            fprintf(logfile, "current d_val is %f\n", d_val);
            p++;
            k++;
        }
    }

    Node* node = OpNew(VALUE, d_val, logfile);          // Любое число - всегда лист дерева - никакой привязки вниз быть
    VERIFICATION(node == nullptr, "OpNew failed!", logfile, nullptr);                                        // не может.
    fprintf(logfile, "I am %s, successfully created node with val = %f\n", __FUNCTION__, node->data.value);

    return node;
}

Node* syntax_error(FILE* logfile){
    VERIFICATION_LOGFILE(logfile, nullptr);

    printf("\033[1;31mSyntax error\033[0m at p = %d: %s\n", p, s + p);
    fprintf(logfile, "Syntax error at p = %d: s = %s\n", p, s + p);

    exit(-1);       // ?..

    return nullptr;
}
