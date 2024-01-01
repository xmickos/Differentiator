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

Node* OpNew(unsigned int data_flag, double value, FILE* logfile){

    Node* node = (Node*)calloc(1, sizeof(Node));
    VERIFICATION(node == nullptr, "Calloc for root->init_node failed!", logfile, nullptr);

    node->left  = nullptr;
    node->right = nullptr;

    switch(data_flag){
        case VALUE:
            node->data.value = value;
            node->data_flag  = VALUE;
            break;
        case OP:
            node->data.type = (char)value;
            node->data_flag = OP;
            break;
        case VAR:
            node->data_flag = VAR;
            node->data.value = 0;
            node->data.name = (char*)calloc(MAX_VAR_NAME_LENGTH, sizeof(char));
            break;
        default:
            fprintf(logfile, "[%s, %d] Bad %s call. Nullptr returned.\n", __FUNCTION__, __LINE__, __FUNCTION__);
            return nullptr;
    }


    fprintf(logfile, "[%s] Created node with data.type = %d == %c, data.value = %f and data_flag = %d\n", __FUNCTION__,
    node->data.type, node->data.type, node->data.value, node->data_flag);

    return node;
}

double OpEval(const Root* root, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(root == nullptr, "Input root is nullptr!\n", logfile, -1);

    Vars_summary * const &varinf = root->vars_info;

    if(root->vars_info->vars_count > 0){
        printf("Your graph has %d variable(s). Please enter their values one-by-one:\n", varinf->vars_count);

        fprintf(logfile, "[%s, %d] Curr vars:\n", __FUNCTION__, __LINE__);
        for(unsigned int i = 0; i < varinf->vars_count; i++){
            fprintf(logfile, "%s <-> %p\n", varinf->vars[i].name, varinf->vars[i].node_p);
        }

        for(unsigned int i = 0; i < varinf->vars_count; i++){
            printf("%s = ", varinf->vars[i].name);
            if( scanf("%lf", &(varinf->vars[i].value)) == 0){
                printf("Wrong value entered. Try again.\n");
                while(!getchar());
                i--;
                continue;
            }
            printf("\n");
            varinf->vars[i].node_p->data.value = varinf->vars[i].value;

        }
        GraphVarAssign(root, logfile);

    }else{
        printf("No variables found!\n");
    }

    printf("Written vars values:\n");
    for(unsigned int i = 0; i < varinf->vars_count; i++){
        printf("%s = %f\n", varinf->vars[i].name, varinf->vars[i].value);
    }

    return OpPartialEval(root->init_node, logfile);
}

double OpPartialEval(const Node* node, FILE* logfile){

    if(node == nullptr){
        #ifdef DEBUG
            fprintf(logfile, "Current node is: nullptr\n");
        #endif
        return 0.;
    }

    double tmp = 0.;

    switch(node->data_flag){
        case VALUE:
            return node->data.value;
        break;
        case OP:
            fprintf(logfile, "[%s, %d] Curr node: data.type = %d == %c, data.value = %f\n",
            __FUNCTION__,
            __LINE__,
            node->data.type,
            node->data.type,
            node->data.value);

            switch(node->data.type){
                case SUMM:
                    return OpPartialEval(node->left, logfile) + OpPartialEval(node->right, logfile);
                case SUBTRACTION:
                    return OpPartialEval(node->left, logfile) - OpPartialEval(node->right, logfile);
                case MULTIPLICATION:
                    return OpPartialEval(node->left, logfile) * OpPartialEval(node->right, logfile);
                case DIVISION:
                    tmp = OpPartialEval(node->right, logfile);
                    VERIFICATION(IsEqual(tmp, 0), "Division by zero!", logfile, -1);
                    // printf("[OpEval] ");
                    return OpPartialEval(node->left, logfile) / tmp;
                default:
                    fprintf(logfile, "node.type is %d == %c\n", node->data.type, node->data.type);
                    VERIFICATION(true, "Bad node type.", logfile, -1.);
            }
        break;
        case VAR:
            fprintf(logfile, "[%s, %d] Case VAR: returning %f for x\n",
                __FUNCTION__,
                __LINE__,
                node->data.value
            );
            return node->data.value;
        break;
        default:
            VERIFICATION(true, "Bad node data flag.", logfile, -1.);
    }

    return 0.;
}

int OpTree2Text(const Root* root, FILE* outputfile, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(outputfile == nullptr, "Input outputfile is nullptr!", logfile, -1);
    VERIFICATION(root == nullptr, "Input root is nullptr!", logfile, -1);

    OpPartialTree2Text(root->init_node, outputfile, logfile);

    return 0;
}

int OpPartialTree2Text(const Node* node, FILE* outputfile, FILE* logfile){

    if(node == nullptr){
        #ifndef NONILL
            fprintf(outputfile, NILLCHAR);
        #endif

        return 0;
    }

    if(node->data_flag == OP) fprintf(outputfile, "(");

    OpPartialTree2Text(node->left, logfile, logfile);

    switch(node->data_flag){
        case VALUE:
            fprintf(outputfile, "%.4f", node->data.value);
        break;
        case OP:
            fprintf(outputfile, "%c", node->data.type);
        break;
        case VAR:
            fprintf(outputfile, "x");
        break;
        default:
            fprintf(outputfile, "[%s, %d]: Wrong node data type.\nExiting...\n", __FUNCTION__, __LINE__);
            return -1;
    }

    OpPartialTree2Text(node->right, logfile, logfile);

    if(node->data_flag == OP) fprintf(outputfile, ")");

    return 0;
}

Root* OpDerivative(Root* root, const char *target_var, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, nullptr);
    VERIFICATION(root == nullptr, "Input root is nullptr!", logfile, nullptr);

    root->init_node = OpPartialDerivative(root->init_node, target_var, logfile);

//     FILE* dotfile = fopen("./dots/dotfile.dot", "w");
//
//     OpGraphDump(root, dotfile, logfile);
//
//     fclose(dotfile);

    printf("\t\t[%s]:\n", __FUNCTION__);

    Node *&init = root->init_node;
    Node *&left = root->init_node->left;
    Node *&right = root->init_node->right;

    printf("Self: %p, left: %p, right: %p, data_flag: %d, type: %c, value: %f\n",
            init,
            init->left,
            init->right,
            init->data_flag,
            init->data.type,
            init->data.value);
    printf("Self: %p, left: %p, right: %p, data_flag: %d, type: %c, value: %f\n",
            left,
            left->left,
            left->right,
            left->data_flag,
            left->data.type,
            left->data.value);
    printf("Self: %p, left: %p, right: %p, data_flag: %d, type: %c, value: %f\n\n",
            right,
            right->left,
            right->right,
            right->data_flag,
            right->data.type,
            right->data.value);

    // printf("\t\t\t[OpVarsFree] & [OpGraphVarsRefresh]\n");

    OpGraphSimplify(root, logfile);

    FILE* dotfile = fopen("./dots/dotfile.dot", "w");

    OpGraphDump(root, dotfile, logfile);

    fclose(dotfile);

    // OpVarsFree(root, logfile);

    root->vars_info = OpGraphVarsRefresh(root, logfile);

    OpTextDump(root, logfile);

    return root;
}

Node* OpPartialDerivative(Node *node, const char *target_var, FILE* logfile){

    Node *left_ = node->left;
    Node *right_ = node->right;
    Node *df_summ, *df_sub, *df_mul, *df_div, *left_copy, *right_copy;

    df_summ = nullptr;
    df_sub = nullptr;
    df_mul = nullptr;
    df_div = nullptr;
    left_copy = nullptr;
    right_copy = nullptr;

    printf("\nself: %p, left_: %p, right_: %p, type: %d, data_flag: %d, data.value: %f\n",
        node,
        left_,
        right_,
        node->data.type,
        node->data_flag,
        node->data.value
    );
    if(node->right != nullptr) printf("right.data_flag = %d\n", node->right->data_flag);

    switch(node->data_flag){
        case VALUE:
        printf("case VALUE\n");
            return OpNew(VALUE, 0.0, logfile);
        break;
        case OP:
            switch(node->data.type){
                case SUMM:
                printf("case SUMM\n");
                    df_summ = OpNew(OP, SUMM, logfile);
                    df_summ->left = OpPartialDerivative(left_, target_var, logfile);
                    df_summ->right = OpPartialDerivative(right_, target_var, logfile);

                    return df_summ;
                break;
                case SUBTRACTION:
                printf("case SUBTRACTION\n");
                    df_sub = OpNew(OP, SUBTRACTION, logfile);
                    df_sub->left = OpPartialDerivative(left_, target_var, logfile);
                    df_sub->right = OpPartialDerivative(right_, target_var, logfile);

                    return df_sub;
                break;
                case MULTIPLICATION:
                    df_mul = OpNew(OP, SUMM, logfile);
                    left_copy = OpNodeCopy(left_, logfile);
                    right_copy = OpNodeCopy(right_, logfile);

                    df_mul->left = OpNew(OP, MULTIPLICATION, logfile);
                    df_mul->left->left = OpPartialDerivative(left_copy, target_var, logfile);
                    df_mul->left->right = OpNodeCopy(right_copy, logfile);

                    df_mul->right = OpNew(OP, MULTIPLICATION, logfile);
                    df_mul->right->left = OpNodeCopy(left_copy, logfile);
                    df_mul->right->right = OpPartialDerivative(right_copy, target_var, logfile);

                    OpTreeFree(left_copy, logfile);
                    OpTreeFree(right_copy, logfile);
                    return df_mul;
                break;
                case DIVISION:
                printf("case DIVISION\n");
                    df_div = OpNew(OP, DIVISION, logfile);
                    left_copy = OpNodeCopy(left_, logfile);
                    right_copy = OpNodeCopy(right_, logfile);


                    df_div->left = OpNew(OP, SUBTRACTION, logfile);

                    df_div->left->left = OpNew(OP, MULTIPLICATION, logfile);
                    df_div->left->left->left = OpPartialDerivative(left_copy, target_var, logfile);
                    df_div->left->left->right = OpNodeCopy(right_copy, logfile);

                    df_div->left->right = OpNew(OP, MULTIPLICATION, logfile);
                    df_div->left->right->left = OpNodeCopy(left_copy, logfile);
                    df_div->left->right->right = OpPartialDerivative(right_copy, target_var, logfile);

                    df_div->right = OpNew(OP, MULTIPLICATION, logfile);
                    df_div->right->left = OpNodeCopy(right_, logfile);
                    df_div->right->right = OpNodeCopy(right_, logfile);

                    OpTreeFree(left_copy, logfile);
                    OpTreeFree(right_copy, logfile);
                    return df_div;
                break;
            }
            free(left_);
            free(right_);
        break;
        case VAR:
        printf("case VAR\n");
            return OpNew(VALUE, 1.0, logfile);
        break;
        default:
            fprintf(logfile, "[%s, %d]: Wrong node data type.\nExiting...\n", __FUNCTION__, __LINE__);
            return nullptr;
    }

    return nullptr;
}

Vars_summary* OpGraphVarsRefresh(const Root* root, FILE* logfile){      // must be removed
    VERIFICATION_LOGFILE(logfile, nullptr);

    Vars_summary *graph_refresh_output = (Vars_summary*)calloc(1, sizeof(Vars_summary));
    VERIFICATION(graph_refresh_output == nullptr, "failed to allocate memory for graph_refresh_output!", logfile, nullptr);

    graph_refresh_output->vars = (Variable*)calloc(MAX_VARS_COUNT, sizeof(Variable));
    VERIFICATION(graph_refresh_output->vars == nullptr, "failed to allocate memory for graph_refresh_output->vars!", logfile, nullptr);

    graph_refresh_output->vars_count = 0;

    OpPartialGraphVarsRefresh(root->init_node, graph_refresh_output, logfile);
//
//     char *p = (char*)root;
//     const double x_samples[3] = {((double)p) % 0xf, (((double)p) / 0xf) % 0xff, (((double)p) % 0xff) % 0xfff};
//     // const double y_samples[3] = {OpEval()}; // bad idea, unfortunately...

    return graph_refresh_output;
}

int OpPartialGraphVarsRefresh(Node* node, Vars_summary* output, FILE* logfile){     // must be removed

    fprintf(stdout, "we need to go deeper!\n");

    if(node == nullptr) return 0;


    if(node->data_flag == VALUE){
        return 0;
    }

    if(node->data_flag == VAR && !is_known_variable(output, "x", logfile)){
        output->vars[output->vars_count].name = (char*)calloc(MAX_VAR_NAME_LENGTH, sizeof(char));
        node->data.name = (char*)calloc(MAX_VAR_NAME_LENGTH, sizeof(char));
        VERIFICATION(output->vars[output->vars_count].name == nullptr, "var name allocation failed!", logfile, -1);
        node->data.name = strcpy(node->data.name, "x");
        output->vars[output->vars_count].name = strcpy(output->vars[output->vars_count].name, "x");

        output->vars_count++;
        output->vars->node_p = node;
        fprintf(stdout, "[%s, %d] Found! p = %p, name: %s\n", __FUNCTION__, __LINE__, node, output->vars[output->vars_count - 1].name);
    } // fix for many units of one var

    if(node->data_flag == VAR){
        node->data.name = strcpy(node->data.name, "x");
    }

    OpPartialGraphVarsRefresh(node->left, output, logfile);
    OpPartialGraphVarsRefresh(node->right, output, logfile);

    return 0;
}

inline int OpVarsFree(Root* root, FILE* logfile){           // must be removed
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(root == nullptr, "Input root is nullptr!\n", logfile, -1);

    for(unsigned int i = 0; i < root->vars_info->vars_count; i++){
        free(root->vars_info->vars[i].name);
    }

    root->vars_info->vars_count = 0;

    free(root->vars_info);

    return 0;
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

bool OpPartialGraphSimplify(Node *node, FILE* logfile){
    bool ret_val = false;

    if(node == nullptr || node->data_flag == VALUE) return false;

    printf("---Node---\np: %p, l: %p, r: %p, data_flag: %d, type: %c, value: %f\n",
            node,
            node->left,
            node->right,
            node->data_flag,
            node->data.type,
            node->data.value
    );

    if(node->data_flag == OP){

        printf("Entered with: p = %p, data_flag = %d, left = %p, right = %p, value = %f, type: %c, ",
                node,
                node->data_flag,
                node->left,
                node->right,
                node->data.value,
                node->data.type
        );

        printf("right data flag: %d\n", node->right->data_flag);

        if(node->left->data_flag == VALUE && node->right->data_flag == VALUE){
            ret_val = true;

            switch(node->data.type){
                case SUMM:
                    node->data.value = node->left->data.value + node->right->data.value;
                break;
                case SUBTRACTION:
                    node->data.value = node->left->data.value - node->right->data.value;
                break;
                case MULTIPLICATION:
                    node->data.value = node->left->data.value * node->right->data.value;
                break;
                case DIVISION:
                    if(!IsEqual(node->right->data.value, 0.)){
                        node->data.value = node->left->data.value / node->right->data.value;
                    }else{
                        printf("[%s, %d]Error: Division by zero in graph. Exiting...\n", __FUNCTION__, __LINE__);
                        return false;
                    }
                break;
            }
            printf("\033[0;33m%d\033[0m    –   now left node \033[0;33m%p\033[0m with data_flag %d and value %f is free!\n",
            __LINE__ + 4,
            node->left,
            node->left->data_flag,
            node->left->data.value
            );
            free(node->left);
            printf("\033[0;31m%d\033[0m    –   now right node \033[0;31m%p\033[0m with data_flag %d and value %f is free!\n",
            __LINE__ + 4,
            node->right,
            node->right->data_flag,
            node->right->data.value
            );
            free(node->right);
            node->left = nullptr;
            node->right = nullptr;
            node->data_flag = VALUE;

        }else{
            printf("Exiting with(1): p = %p, data_flag = %d, left = %p, right = %p, value = %f\n",
                    node,
                    node->data_flag,
                    node->left,
                    node->right,
                    node->data.value
                    );
            bool val1, val2;
            printf("Going left into %p, data_flag: %d, type: %c, value: %f\n",
                node->left,
                node->left->data_flag,
                node->left->data.type,
                node->left->data.value
            );
            val1 = OpPartialGraphSimplify(node->left, logfile);
            printf("left returned: %d\nGoing right:\n", val1);
            printf("Going right into %p, data_flag: %d, type: %c, value: %f\n",
                node->right,
                node->right->data_flag,
                node->right->data.type,
                node->right->data.value
            );
            val2 = OpPartialGraphSimplify(node->right, logfile);
            printf("right returned: %d\n", val2);
            return val1 || val2;
        }
    }else{
        printf("Exiting with(2): p = %p, data_flag = %d, left = %p, right = %p, value = %f\n",
                node, node->data_flag, node->left, node->right, node->data.value);
        return false;
    }

    printf("Exiting with(3): p = %p, data_flag = %d, left = %p, right = %p, value = %f\n",
            node, node->data_flag, node->left, node->right, node->data.value);
    return ret_val;
}

inline Node* OpNodeCopy(const Node *node, FILE *logfile){
    VERIFICATION_LOGFILE(logfile, nullptr);
    VERIFICATION(node == nullptr, "Input node is nullptr!", logfile, nullptr);

    Node* output = nullptr;

    switch(node->data_flag){
        case VALUE:
            output = OpNew(node->data_flag, node->data.value, logfile);
            break;
        case OP:
            output = OpNew(node->data_flag, node->data.type, logfile);
            output->left = OpNodeCopy(node->left, logfile);
            output->right = OpNodeCopy(node->right, logfile);
            break;
        case VAR:
            output = OpNew(node->data_flag, node->data.type, logfile);
            output->data.name = strcpy(output->data.name, node->data.name);
            printf("[%s, %d] Copying for %p, name_p: %p = %s from node.dataname = %s\n",
            __FUNCTION__,
            __LINE__,
            output,
            &(output->data.name),
            output->data.name,
            node->data.name
            );
            break;
        default:
            fprintf(logfile, "[%s, %d]: Wrong node data type.\nExiting...\n", __FUNCTION__, __LINE__);
            return nullptr;
    }

    return output;
}

inline int OpTreeFree(Node *node, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);

    if(node != nullptr){
        if(node->left != nullptr){
            OpTreeFree(node->left, logfile);
        }
        if(node->right != nullptr){
            OpTreeFree(node->right, logfile);
        }
        if(node->data_flag == VAR){
            free(node->data.name);
        }
        free(node);
    }

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

int PartialGraphVarAssign(Node *node, const char *name, const double value, FILE* logfile){

    if(node != nullptr) printf("[%s, %d]: node = %p\n", __FUNCTION__, __LINE__, node);

    if(node->data_flag == VAR){
        printf("[%s, %d] %p <-> name:", __FUNCTION__, __LINE__, node);
        printf("%s\n", node->data.name);
        if(!strcmp(node->data.name, name)){
            printf("find! %p\n", node);
            node->data.value = value;
        }
        return 0;
    }

    if(node->data_flag == OP){
        PartialGraphVarAssign(node->left, name, value, logfile);
        PartialGraphVarAssign(node->right, name, value, logfile);
    }


    return 0;
}
