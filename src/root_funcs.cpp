#include "differ_header.h"
#include "RD_header.hpp"

int RootCtor(Root* root, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(root == nullptr, "Input root is nullptr!", logfile, -1);

    root->init_node = (Node*)calloc(1, sizeof(Node));
    VERIFICATION(root->init_node == nullptr, "Calloc for root->init_node failed!", logfile, -1);

    root->vars_info = (Vars_summary*)calloc(1, sizeof(Vars_summary));
    VERIFICATION(root->vars_info == nullptr, "Calloc for root->vars_info failed!", logfile, -1);

    root->vars_info->vars = (Variable*)calloc(MAX_VARS_COUNT, sizeof(Variable));
    VERIFICATION(root->init_node == nullptr, "Calloc for root->vars failed!", logfile, -1);

    root->vars_info->vars_count = 0;

    return 0;
}

int OpTree2Text(const Root* root, FILE* outputfile, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(outputfile == nullptr, "Input outputfile is nullptr!", logfile, -1);
    VERIFICATION(root == nullptr, "Input root is nullptr!", logfile, -1);

    OpPartialTree2Text(root->init_node, outputfile, logfile);

    return 0;
}

Root* OpDerivative(Root* root, const char *target_var, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, nullptr);
    VERIFICATION(root == nullptr, "Input root is nullptr!", logfile, nullptr);

    root->init_node = OpPartialDerivative(root->init_node, target_var, logfile);

    printf("\t\t[%s]:\n", __FUNCTION__);

    // Node *&init = root->init_node;
    // Node *&left = root->init_node->left;
    // Node *&right = root->init_node->right;

    // printf("Self: %p, left: %p, right: %p, data_flag: %d, type: %c, value: %f\n",
    //         init,
    //         init->left,
    //         init->right,
    //         init->data_flag,
    //         init->data.type,
    //         init->data.value);
    // printf("Self: %p, left: %p, right: %p, data_flag: %d, type: %c, value: %f\n",
    //         left,
    //         left->left,
    //         left->right,
    //         left->data_flag,
    //         left->data.type,
    //         left->data.value);
    // printf("Self: %p, left: %p, right: %p, data_flag: %d, type: %c, value: %f\n\n",
    //         right,
    //         right->left,
    //         right->right,
    //         right->data_flag,
    //         right->data.type,
    //         right->data.value);

    // printf("\t\t\t[OpVarsFree] & [OpGraphVarsRefresh]\n");

    OpGraphSimplify(root, logfile);

    // OpVarsFree(root, logfile);

    root->vars_info = OpGraphVarsRefresh(root, logfile);

    FILE* my_dotfile = fopen("./dots/dotfile.dot", "a+");

    printf("\033[0;33mGraph Dump #1\033[0m\n");

    OpGraphDump(root, my_dotfile, logfile);

    fclose(my_dotfile);

    OpTextDump(root, logfile);

    return root;
}

Vars_summary* OpGraphVarsRefresh(const Root* root, FILE* logfile){      // must be removed
    /*
        Эта функция нужна для определения переменных, которые
        участвуют в графе вычислений, и определения их адресов
        в памяти.

    */

    VERIFICATION_LOGFILE(logfile, nullptr);

    Vars_summary *graph_refresh_output = (Vars_summary*)calloc(1, sizeof(Vars_summary));
    VERIFICATION(graph_refresh_output == nullptr, "failed to allocate memory for graph_refresh_output!", logfile, nullptr);

    graph_refresh_output->vars = (Variable*)calloc(MAX_VARS_COUNT, sizeof(Variable));
    VERIFICATION(graph_refresh_output->vars == nullptr, "failed to allocate memory for graph_refresh_output->vars!", logfile, nullptr);

    graph_refresh_output->vars_count = 0;

    OpPartialGraphVarsRefresh(root->init_node, graph_refresh_output, logfile);

//     char *p = (char*)root;
//     const double x_samples[3] = {((double)p) % 0xf, (((double)p) / 0xf) % 0xff, (((double)p) % 0xff) % 0xfff};
//     const double y_samples[3] = {OpEval()}; // bad idea, unfortunately...

    return graph_refresh_output;
}

int OpGraphSimplify(Root* root, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(root == nullptr, "Input root is nullptr!", logfile, -1);

    int i = 0;

    do{
        printf("\033[0;34miteration #%d\033[0m\n", i);
        i++;
    }while(OpPartialGraphSimplify(root->init_node, logfile));


    return 0;
}

int GraphVarAssign(const Root *root, FILE *logfile){
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(root == nullptr, "Input root is nullptr!", logfile, -1);

    for(unsigned int i = 0; i < root->vars_info->vars_count; i++){
        printf("[%s, %d] Working for %s\n", __FUNCTION__, __LINE__, root->vars_info->vars[i].name);
        PartialGraphVarAssign(root->init_node, root->vars_info->vars[i].name, root->vars_info->vars[i].value, logfile);
    }

    return 0;

}
