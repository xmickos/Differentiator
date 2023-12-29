#include "differ_header.h"
#include "RD_header.hpp"


// int p = 0;
// const char* s = NULL;
// unsigned int vars_count = 0;


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

VI  итерация: одна переменная 'x'
    G ::= E'\0'
    E ::= T{[+-]T}*
    T ::= P{[*\]P}*
    P ::= '('E')' | N | 'x'
    N ::= ['0'-'9']+

    (2+5)*(2-5)'\0'
*/


Root* GetG(const char* str, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, nullptr);

    RD_data *data = nullptr;

    data = (RD_data*)calloc(1, sizeof(RD_data));
    data->s = str;
    data->p = 0;
    data->vars_info.vars_count = 0;
    data->vars_info.vars = (Variable*)calloc(MAX_VARS_COUNT, sizeof(Variable));
    VERIFICATION(data->vars_info.vars == nullptr, "data->vars calloc failed!", logfile, nullptr;);


    Root *output = (Root*)calloc(1, sizeof(Root));
    VERIFICATION(output == nullptr, "Vars_summary calloc failed!", logfile, nullptr;);

    DEBUG_CALL(GetE, logfile);
    Node* node = GetE(data, logfile);

    if(data->s[data->p] == '\0'){
        // output->vars_info.vars_count = data->vars_info.vars_count;
        output->init_node = node;
        // output->vars_info.vars = data->vars_info.vars;
        output->vars_info = OpGraphVarsRefresh(output, logfile);
        return output;
    }

    DEBUG_CALL(syntax_error, logfile);
    printf("\033[1;31mSyntax error\033[0m at p = %d: %s\n", data->p, data->s + data->p);
    fprintf(logfile, "Syntax error at p = %d: s = %s\n", data->p, data->s + data->p);

    exit(-1);       // ?..
}

Node* GetE(RD_data *data, FILE* logfile){
    DEBUG_ECHO(logfile);

    DEBUG_CALL(GetT, logfile);
    Node *node_left = GetT(data, logfile);
    Node *node = nullptr;

    if(data->s[data->p] == SUMM || data->s[data->p] == SUBTRACTION){

        DEBUG_CALL(PartialGetE, logfile);
        node = PartialGetE(data, logfile);

        node->left = node_left;

        return node;
    }else{
        return node_left;
    }

}

Node* PartialGetE(RD_data *data, FILE* logfile){
    DEBUG_ECHO(logfile);

    Node *subroot = OpNew(OP, data->s[data->p], logfile);

    data->p++;

    DEBUG_CALL(GetT, logfile);
    Node *node_right = GetT(data, logfile);
    Node *subsubroot = nullptr;

    if(data->s[data->p] == SUMM || data->s[data->p] == SUBTRACTION){

        DEBUG_CALL(PartialGetE, logfile);
        subsubroot = PartialGetE(data, logfile);

        subroot->right = subsubroot;
        subsubroot->left = node_right;
    }else{
        subroot->right = node_right;
    }

    return subroot;
}


Node* GetT(RD_data *data, FILE* logfile){
    DEBUG_ECHO(logfile);

    DEBUG_CALL(GetP, logfile);
    Node *node_left = GetP(data, logfile);
    Node* node = nullptr;

    if(data->s[data->p] == MULTIPLICATION || data->s[data->p] == DIVISION){

        DEBUG_CALL(PartialGetT, logfile);
        node = PartialGetT(data, logfile);

        node->left = node_left;

        return node;
    }else{
        return node_left;
    }


}

Node* PartialGetT(RD_data *data, FILE* logfile){
    DEBUG_ECHO(logfile);

    Node *subroot = OpNew(OP, data->s[data->p], logfile);

    data->p++;

    DEBUG_CALL(GetP, logfile);
    Node *node_right = GetP(data, logfile);
    Node *subsubroot = nullptr;

    fprintf(logfile, "[%s, %d]: GetP returned %p\n", __FUNCTION__, __LINE__, node_right);

    if(node_right->data_flag == VALUE){
        fprintf(logfile, "I am %s, got node with val = %f from GetP\n", __FUNCTION__, node_right->data.value);
    }


    if(data->s[data->p] == MULTIPLICATION || data->s[data->p] == DIVISION){

        DEBUG_CALL(PartialGetT, logfile);
        subsubroot = PartialGetT(data, logfile);

        subroot->right = subsubroot;
        subsubroot->left = node_right;
    }else{
        subroot->right = node_right;
    }

    return subroot;
}



Node* GetP(RD_data *data, FILE* logfile){
    DEBUG_ECHO(logfile);

    Node* node = nullptr;

    if(data->s[data->p] == 'x'){
        fprintf(logfile, "Variable case.\n");
        node = OpNew(VAR, 0, logfile);
        // data->vars_info.vars[data->vars_info.vars_count].node_p = node;
        data->p++;
        // if(!is_known_variable(&(data->vars_info), "x", logfile)){          // must be replaced with current var name
            // data->vars[data->vars_count].name = (char*)calloc(MAX_VAR_NAME_LENGTH, sizeof(char));
            // data->vars_info.vars[data->vars_info.vars_count].name = (char*)calloc(MAX_VAR_NAME_LENGTH, sizeof(char));
            // VERIFICATION(data->vars_info.vars[data->vars_info.vars_count].name == nullptr, "data->vars[data->vars_count] calloc failed!", logfile, nullptr);

            // data->vars[data->vars_count].name = strcpy(data->vars[data->vars_count].name, "x");
            // data->vars_info.vars[data->vars_info.vars_count].name = strcpy(data->vars_info.vars[data->vars_info.vars_count].name, "x");
            // printf("allocated: %p == %s\n", data->vars[data->vars_count].name, data->vars[data->vars_count].name);
            // printf("allocated: %p == %s\n", data->vars_info.vars[data->vars_info.vars_count].name);

            // data->vars_count++;
            // data->vars_info.vars_count++;
        // }
        return node;
    }

    if(data->s[data->p] == '('){
        data->p++;
        DEBUG_ECHO(logfile);
        DEBUG_CALL(GetE, logfile);
        node = GetE(data, logfile);
        VERIFICATION(node == nullptr, "GetE failed!", logfile, nullptr);
        fprintf(logfile, "I am %s, got from GetE node with val = %f, data.type = %c and data_flag = %d\n", __FUNCTION__,
                node->data.value, node->data.type, node->data_flag);
    }else{
        fprintf(logfile, "Default case.\n");
        DEBUG_CALL(GetN, logfile);
        Node *debug_variable = GetN(data, logfile);
        fprintf(logfile, "I am %s, got from GetN node with val = %f\n", __FUNCTION__, debug_variable->data.value);
        return debug_variable;
    }

    fprintf(logfile, "[%s, %d]: curr s[%d] is %c\n", __FUNCTION__, __LINE__, data->p, data->s[data->p]);

    if(data->s[data->p] == ')'){
        data->p++;
        fprintf(logfile, "[%s, %d]: Exiting with \')\'\n", __FUNCTION__, __LINE__);
        return node;
    }

    return nullptr;
}

Node* GetN(RD_data *data, FILE* logfile){
    DEBUG_ECHO(logfile);

    int val = 0;

    if(data->s[data->p] - '0' > 9 || data->s[data->p] - '0' < 0){
        syntax_error(data, logfile);            //Хотя бы одна цифра в начале должна быть,
    }                                           //иначе syntax_error; Тогда пустые строки запрещены

    while(data->s[data->p] - '0' <= 9 && data->s[data->p] - '0' >= 0){
        val = 10*val + data->s[data->p] - '0';
        data->p++;
    }

    double d_val = (double)val;
    int k = 1;

    DEBUG_ECHO(logfile);
    if(data->s[data->p] == '.'){
        data->p++;

        while(data->s[data->p] - '0' <= 9 && data->s[data->p] - '0' >= 0){
            DEBUG_ECHO(logfile);
            d_val = d_val + (double)(data->s[data->p] - '0') / d_pow(10.0, k);
            data->p++;
            k++;
        }
    }

    Node* node = OpNew(VALUE, d_val, logfile);          // Любое число - всегда лист дерева - никакой привязки вниз быть
    VERIFICATION(node == nullptr, "OpNew failed!", logfile, nullptr);                                        // не может.
    fprintf(logfile, "I am %s, successfully created node with val = %f\n", __FUNCTION__, node->data.value);

    return node;
}

Node* syntax_error(RD_data *data, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, nullptr);

    printf("\033[1;31mSyntax error\033[0m at p = %d: %s\n", data->p, data->s + data->p);
    fprintf(logfile, "Syntax error at p = %d: s = %s\n", data->p, data->s + data->p);

    exit(-1);       // ?..

    return nullptr;
}

bool is_known_variable(Vars_summary *vars_info, const char* target_name, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, false);
    VERIFICATION(vars_info == nullptr, "vars_info input is nullptr!", logfile, false);

    for(unsigned int i = 0; i < vars_info->vars_count; i++){
        if(!strcmp(vars_info->vars[i].name, target_name)){
            return true;
        }
    }

    return false;
}
