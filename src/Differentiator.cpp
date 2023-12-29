#include "differ_header.h"
#include "RD_header.hpp"

int main(){

    printf("_DEBUG = %d, – ???\n", _DEBUG);

    Root root = {};
    int ret_val = 0;
    FILE* logfile = fopen("./logs/logfile.txt", "w");
    ret_val = RootCtor(&root, logfile);
    RETURN_CHECK(ret_val);

    // const char my_str[128] = "(0.578+0.172)*(0.823+0.117)-1.711/(4.418+x)";
    const char my_str[128] = "x*2/3";
    free(root.init_node);

    Root *rd_output = GetG(my_str, logfile);

    root.init_node = rd_output->init_node;
    root.vars_info->vars_count = rd_output->vars_info->vars_count;
    root.vars_info->vars = rd_output->vars_info->vars;

    OpTextDump(&root, logfile);

    FILE* dotfile = fopen("./dots/dotfile.dot", "w");
    // OpGraphDump(&root, dotfile, logfile);

    printf("Answer for %s is %f\n", my_str, OpEval(&root, logfile));

    // OpTree2Text(&root, stdout, logfile);        // fix

    Root *df_dx = OpDerivative(&root, "x", logfile);

    OpGraphDump(df_dx, dotfile, logfile);
    OpTextDump(df_dx, logfile);
    printf("Answer for d (%s) / dx is %f\n", my_str, OpEval(df_dx, logfile));

    fclose(dotfile);
    fclose(logfile);
    return 0;
}

