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
        (((2000.00)-(1008.25))/((3.00)+(4.00)))

    VI  итерация: одна переменная 'x'
        G ::= E'\0'
        E ::= T{[+-]T}*
        T ::= P{[*\]P}*
        P ::= '('E')' | N | 'x'
        N ::= ['0'-'9']+

*/

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
