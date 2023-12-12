#include "differ_header.h"
#include "RD_header.hpp"

int main(){

    Root root = {};
    int ret_val = 0;
    FILE* logfile = fopen("logfile.txt", "w");
    ret_val = RootCtor(&root, logfile);
    RETURN_CHECK(ret_val);

    root.init_node = OpNew(OP, DIVISION, logfile);
    root.init_node->left = OpNew(OP, SUBTRACTION, logfile);
    root.init_node->right = OpNew(OP, SUMM, logfile);
    root.init_node->left->left = OpNew(VALUE, 2000, logfile);
    root.init_node->left->right = OpNew(VALUE, 1000, logfile);
    root.init_node->right = OpNew(OP, SUMM, logfile);
    root.init_node->right->left = OpNew(VALUE, 3, logfile);
    root.init_node->right->right = OpNew(VALUE, 4, logfile);

    FILE* dotfile = fopen("dotfile.dot", "w");
    OpGraphDump(&root, dotfile, logfile);
    fclose(dotfile);

    OpTextDump(&root, logfile);

    double c = 0;

    c = OpEval(&root, logfile);

    printf("Answer is %f\n", c);

    OpTree2Text(&root, logfile, logfile);

    const char my_str[128] = "(((2000.00)-(1008.25))/((3.00)+(4.00)))";
    printf("Answer is %f\n", GetG(my_str, logfile));

    fclose(logfile);
    return 0;
}

