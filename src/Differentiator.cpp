#include "differ_header.h"
#include "RD_header.hpp"

int main(){

    printf("_DEBUG = %d, – ???\n", _DEBUG);

    Root root = {};
    int ret_val = 0;
    FILE* logfile = fopen("./logs/logfile.txt", "w");
    ret_val = RootCtor(&root, logfile);
    RETURN_CHECK(ret_val);

    const char my_str[128] = "(0.578+0.172)*(0.823+0.117)-1.711/(4.418+x)";

    free(root.init_node);

    root.init_node = GetG(my_str, logfile);

    OpTextDump(&root, logfile);

    FILE* dotfile = fopen("./dots/dotfile.dot", "w");
    OpGraphDump(&root, dotfile, logfile);
    fclose(dotfile);

    printf("Answer is %f\n", OpEval(&root, logfile));

    fclose(logfile);
    return 0;
}

