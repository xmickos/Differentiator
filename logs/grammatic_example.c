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
