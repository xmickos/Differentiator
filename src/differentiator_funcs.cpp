#include "differ_header.h"
#include "RD_header.hpp"

int RootCtor(Root* root, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(root == nullptr, "Input root is nullptr!", logfile, -1);

    root->init_node = (Node*)calloc(1, sizeof(Node));
    VERIFICATION(root->init_node == nullptr, "Calloc for root->init_node failed!", logfile, -1);

    root->vars = (Variable*)calloc(MAX_VARS_COUNT, sizeof(Variable));
    VERIFICATION(root->init_node == nullptr, "Calloc for root->vars failed!", logfile, -1);

    root->vars_count = 0;

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
            node->data.type = 0;
            node->data.value = 0;
            break;
        default:
            fprintf(logfile, "[%s, %d] Bad %s call. Nullptr returned.\n", __FUNCTION__, __LINE__, __FUNCTION__);
            return nullptr;
    }


    fprintf(logfile, "[%s] Created node with data.type = %d == %c, data.value = %f and data_flag = %d\n", __FUNCTION__,
    node->data.type, node->data.type, node->data.value, node->data_flag);

    return node;
}

int OpGraphDump(const Root* root, FILE* dotfile, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(root == nullptr, "Input root is nullptr!", logfile, -1);
    VERIFICATION(dotfile == nullptr, "Input dotfile is nullptr!", logfile, -1);

    fprintf(dotfile, GRAPHVIZ_INIT);

    switch(root->init_node->data_flag){
        case VALUE:
            fprintf(logfile, "[%s, %d] Value!\n", __FUNCTION__, __LINE__);
            fprintf(dotfile, GRAPHVIZ_MKNODE_VALUE("0/0", "%f"), root->init_node->data.value);
        break;
        case OP:
            fprintf(logfile, "[%s, %d] Op!\n", __FUNCTION__, __LINE__);
            fprintf(dotfile, GRAPHVIZ_MKNODE_OP("0/0", "%c"), root->init_node->data.type);
        break;
        case VAR:
            fprintf(logfile, "[%s, %d] Var!\n", __FUNCTION__, __LINE__);
            fprintf(dotfile, GRAPHVIZ_MKNODE_OP("0/0", "x"));
        break;
        default:
            fprintf(logfile, "[%s, %d]: Wrong node data type.\nExiting...\n", __FUNCTION__, __LINE__);
            return -1;
    }



    OpPartialGraphDump(root->init_node, dotfile, 0, (char)0, logfile);

    fprintf(dotfile, GRAPHVIZ_END);

    return 0;
}

int OpPartialGraphDump(const Node* node, FILE* dotfile, unsigned char ip, unsigned char depth, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(dotfile == nullptr, "Input dotfile is nullptr!", logfile, -1);

    unsigned char new_ip = 0, new_depth = depth + (unsigned char)1;

    if(node == nullptr) return 0;

    if(node->left != nullptr){

        switch(node->left->data_flag){
            case VALUE:
                fprintf(logfile, "[%s, %d] Value! %.4f -> %d/%d, old ip & depth: %d/%d\n", __FUNCTION__, __LINE__,
                node->left->data.value, new_ip, new_depth, ip, depth);
                fprintf(dotfile, GRAPHVIZ_MKNODE_VALUE("%d/%d", "%.4f"), ip, new_depth, node->left->data.value);
            break;
            case OP:
                fprintf(logfile, "[%s, %d] Op!\n", __FUNCTION__, __LINE__);
                fprintf(dotfile, GRAPHVIZ_MKNODE_OP("%d/%d", "%c"), ip, new_depth, node->left->data.type);
            break;
            case VAR:
                fprintf(logfile, "[%s, %d] Var!\n", __FUNCTION__, __LINE__);
                fprintf(dotfile, GRAPHVIZ_MKNODE_OP("%d/%d", "x"), ip, new_depth);
            break;
            default:
                fprintf(logfile, "[%s, %d]: Wrong node data type.\nExiting...\n", __FUNCTION__, __LINE__);
                return -1;
        }

        fprintf(dotfile, GRAPHVIZ_CONNECT_NODE("\"%d/%d\"", "\"%d/%d\""), ip, depth, ip, new_depth);

        fprintf(logfile, "node \"%d/%d\" with type: %c\n", ip, depth, node->data.type);
    }

    if(node->right != nullptr){
        new_ip = ip | (unsigned char)(1 << (CHAR_BIT - depth - 1));

        switch(node->right->data_flag){
            case VALUE:
                fprintf(logfile, "[%s, %d] Value!\n", __FUNCTION__, __LINE__);
                fprintf(dotfile, GRAPHVIZ_MKNODE_VALUE("%d/%d", "%.4f"), new_ip, new_depth, node->right->data.value);
            break;
            case OP:
                fprintf(logfile, "[%s, %d] Op!\n", __FUNCTION__, __LINE__);
                fprintf(dotfile, GRAPHVIZ_MKNODE_OP("%d/%d", "%c"), new_ip, new_depth, node->right->data.type);
            break;
            case VAR:
                fprintf(logfile, "[%s, %d] Var!\n", __FUNCTION__, __LINE__);
                fprintf(dotfile, GRAPHVIZ_MKNODE_VAR("%d/%d", "x"), new_ip, new_depth);
            break;
            default:
                fprintf(logfile, "[%s, %d]: Wrong node data type.\nExiting...\n", __FUNCTION__, __LINE__);
                return -1;
        }

        fprintf(dotfile, GRAPHVIZ_CONNECT_NODE("\"%d/%d\"", "\"%d/%d\""), ip, depth, new_ip, new_depth);
    }

    OpPartialGraphDump(node->left, dotfile, ip, new_depth, logfile);
    OpPartialGraphDump(node->right, dotfile, new_ip, new_depth, logfile);


    return 0;
}

int OpTextDump(const Root* root, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(root == nullptr, "Input root is nullptr", logfile, -1);

    OpPartialTextDump(root->init_node, 0, logfile);

    fprintf(logfile, "\n");

    return 0;
}

int OpPartialTextDump(const Node* node, size_t indent, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);

    if(node == nullptr) return 0;

    IndentMe(indent + 1u, logfile);

    switch(node->data_flag){
        case VALUE:
            fprintf(logfile, "%f\n", node->data.value);
        break;
        case OP:
            fprintf(logfile, "%c\n", node->data.type);
        break;
        case VAR:
            fprintf(logfile, "x\n");
        break;
        default:
            fprintf(logfile, "[%s, %d]: Wrong node data type.\nExiting...\n", __FUNCTION__, __LINE__);
            return -1;
    }

    IndentMe(indent + 1u, logfile);
    fprintf(logfile, "left:\t\n");
    OpPartialTextDump(node->left, indent + 1, logfile);

    IndentMe(indent + 1u, logfile);
    fprintf(logfile, "right:\t\n");
    OpPartialTextDump(node->right, indent + 1, logfile);

    return 0;
}

inline void IndentMe(size_t count, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, );
    for(size_t i = 0; i < count; i++) fprintf(logfile, "\t");
}

double OpEval(const Root* root, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(root == nullptr, "Input root is nullptr!\n", logfile, -1);

    if(root->vars_count > 0){
        printf("Your graph has %d variable(s). Please enter their values one-by-one:\n", root->vars_count);

        fprintf(logfile, "[%s, %d] Curr vars:\n", __FUNCTION__, __LINE__);
        for(unsigned int i = 0; i < root->vars_count; i++){
            fprintf(logfile, "%s\n", root->vars[i].name);
        }

        for(unsigned int i = 0; i < root->vars_count; i++){
            printf("%s = ", root->vars[i].name);
            if( scanf("%lf", &(root->vars[i].value)) == 0){
                i--;
            }
            printf("\n");
            root->vars[i].node_p->data.value = root->vars[i].value;
        }
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
            fprintf(logfile, "[%s, %d] Curr node: data.type = %d == %c, data.value = %f\n", __FUNCTION__, __LINE__, node->data.type, node->data.type, node->data.value);

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
                    return OpPartialEval(node->left, logfile) / tmp;
                default:
                    fprintf(logfile, "node.type is %d == %c\n", node->data.type, node->data.type);
                    VERIFICATION(true, "Bad node type.", logfile, -1.);
            }
        break;
        case VAR:
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
