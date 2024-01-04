#include "differ_header.h"
#include "RD_header.hpp"
// #include "gtest/gtest.h"

int main(int argc, const char* argv[]){

if(DEBUG_){
    for(int i = 0; i < argc; i++){
        printf("%s\n", argv[i]);
    }
}

    if(argc < 2){
        printf("Not enough input parameters. Use -help for help.\n");
        return 0;
    }else{
        if(!strcmp(argv[1], "-help")){
            printf("\t\tDifferentiator %d.%d \n", Differentiator_VERSION_MAJOR, Differentiator_VERSION_MINOR);
            printf("  * This program can work in three modes: calculating the value of the expression at a given point, "
            "calculating the value of the derivative of the expression at a given point, \n    plotting the function and debugging. First "
            "argument always must be an expression. \n");
            printf("  -c –> Calculate value of expression written before. Value of x should be wrote after flag \"-c\"\n"
            "    Example: \"x*x+2*x+5 -c 2.5\" tells programm to calculate value of "
            "expression x^2 + 2*x + 5 at x = 2.5.\n"
            "  -d -> Calculate value of expression' derivative at given point. Value of x should be wrote after flag \"-d\".\n");
            printf("  -p -> Draw plot of given func.\n    Example: \"x*x+2*x+5 -d 2.5\".\n");

        return 0;
        }

        FILE* logfile = fopen("../logs/logfile.txt", "w");
        VERIFICATION_LOGFILE(logfile, -1);

        if(strlen(argv[1]) > MAX_EXPRESSION_LENGTH){
            printf("Expression is too large. Please try to simplify it before inputting.\n");
            return 0;
        }else{
            Root* root = RootCtor(logfile);

            char my_str[MAX_EXPRESSION_LENGTH] = "\0";
            strcpy(my_str, argv[1]);

            Root *rd_output = GetG(my_str, logfile);

            root->init_node = rd_output->init_node;
            root->vars_info->vars_count = rd_output->vars_info->vars_count;
            root->vars_info->vars = rd_output->vars_info->vars;

            Root *df_dx = nullptr;

            switch(argv[2][1]){
                case 'c':
                    printf("Answer for %s is %f\n", my_str, OpEval(root, logfile));
                break;
                case 'd':
                    df_dx = OpDerivative(root, "x", logfile);

                    printf("Answer for d (%s) / dx is %f\n", my_str, OpEval(df_dx, logfile));
                break;
                case 'p':

                break;
                default:
                    printf("Wrong second argument.\n");
                break;

                return 0;
            }
        }

    }

//     FILE* logfile = fopen("./logs/logfile.txt", "w");
//     VERIFICATION_LOGFILE(logfile, -1);
//
//     Root* root = RootCtor(logfile);
//     VERIFICATION(root == nullptr, "Bad return value for RootCtor", logfile, -1);
//
//     const char my_str[128] = "1+x+x*x/2+x*x*x/6+x*x*x*x/24+1/x";
//
//     Root *rd_output = GetG(my_str, logfile);
//
//     root->init_node = rd_output->init_node;
//     root->vars_info->vars_count = rd_output->vars_info->vars_count;
//     root->vars_info->vars = rd_output->vars_info->vars;
//
//     Root *df_dx = OpDerivative(root, "x", logfile);
//
//     printf("Answer for d (%s) / dx is %f\n", my_str, OpEval(df_dx, logfile));
//
//     fclose(logfile);
    return 0;
}

