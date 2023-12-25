#include "differ_header.h"
#include "RD_header.hpp"

int RootCtor(Root* root, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(root == nullptr, "Input root is nullptr!", logfile, -1);

    root->init_node = (Node*)calloc(1, sizeof(Node));
    VERIFICATION(root->init_node == nullptr, "Calloc for root->init_node failed!", logfile, -1);

    return 0;
}

Node* OpNew(unsigned int data_flag, int value, FILE* logfile){

    /*
        Я не придумал ничего лучше, чем сделать тип value int'ом, чтобы оно могло спокойно конвертироваться и в double,
        и в char в обоих случаях.
    */

    Node* node = (Node*)calloc(1, sizeof(Node));
    VERIFICATION(node == nullptr, "Calloc for root->init_node failed!", logfile, nullptr);

    node->left  = nullptr;
    node->right = nullptr;

    if(data_flag == VALUE){
        node->data.type  = NONE;
        node->data.value = (double)value;
        node->data_flag  = VALUE;
    }else{
        node->data.type = (char)value;
     /* node->data_flag = OP; */
    }

    return node;
}

int OpGraphDump(Root* root, FILE* dotfile, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(root == nullptr, "Input root is nullptr!", logfile, -1);
    VERIFICATION(dotfile == nullptr, "Input dotfile is nullptr!", logfile, -1);

    fprintf(dotfile, GRAPHVIZ_INIT);

    if(root->init_node->data_flag != VALUE){
        fprintf(dotfile, GRAPHVIZ_MKNODE_OP("0/0", "%c"), root->init_node->data.type);
    }else{
        fprintf(dotfile, GRAPHVIZ_MKNODE_VALUE("0/0", "%f"), root->init_node->data.value);
    }

    OpPartialGraphDump(root->init_node, dotfile, 0, (char)0, logfile);

    fprintf(dotfile, GRAPHVIZ_END);

    return 0;
}

int OpPartialGraphDump(Node* node, FILE* dotfile, unsigned char ip, unsigned char depth, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(dotfile == nullptr, "Input dotfile is nullptr!", logfile, -1);

    unsigned char new_ip = 0, new_depth = depth + (unsigned char)1;

    if(node == nullptr) return 0;

    if(node->left != nullptr){

        if(node->left->data_flag != VALUE){
            fprintf(logfile, "Created OP node with name \"%d/%d\" and op %c, connected it to ", ip, new_depth, node->left->data.type);
            fprintf(dotfile, GRAPHVIZ_MKNODE_OP("%d/%d", "%c"), ip, new_depth, node->left->data.type);
        }else{
            fprintf(logfile, "Created VALUE node with name \"%d/%d\" and value %f, connected it to ", ip, new_depth, node->left->data.value);
            fprintf(dotfile, GRAPHVIZ_MKNODE_VALUE("%d/%d", "%.2f"), ip, new_depth, node->left->data.value);
        }

        fprintf(dotfile, GRAPHVIZ_CONNECT_NODE("\"%d/%d\"", "\"%d/%d\""), ip, depth, ip, new_depth);

        fprintf(logfile, "node \"%d/%d\" with type: %c\n", ip, depth, node->data.type);
    }

    if(node->right != nullptr){
        new_ip = ip | (unsigned char)(1 << (CHAR_BIT - depth - 1));

        if(node->right->data_flag != VALUE){
            fprintf(dotfile, GRAPHVIZ_MKNODE_OP("%d/%d", "%c"), new_ip, new_depth, node->right->data.type);
        }
        else{
            fprintf(dotfile, GRAPHVIZ_MKNODE_VALUE("%d/%d", "%.2f"), new_ip, new_depth, node->right->data.value);
        }

        fprintf(dotfile, GRAPHVIZ_CONNECT_NODE("\"%d/%d\"", "\"%d/%d\""), ip, depth, new_ip, new_depth);
    }

    OpPartialGraphDump(node->left, dotfile, ip, new_depth, logfile);
    OpPartialGraphDump(node->right, dotfile, new_ip, new_depth, logfile);


    return 0;
}

int OpTextDump(Root* root, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(root == nullptr, "Input root is nullptr", logfile, -1);

    OpPartialTextDump(root->init_node, 0, logfile);

    return 0;
}

int OpPartialTextDump(Node* node, size_t indent, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);

    if(node == nullptr) return 0;

    IndentMe(indent + 1u, logfile);
    if(node->data_flag == VALUE){
        fprintf(logfile, "%f\n", node->data.value);
    }else{
        fprintf(logfile, "%c\n", node->data.type);
    }

    IndentMe(indent + 1u, logfile);
    fprintf(logfile, "left:\t\n");
    OpPartialTextDump(node->left, indent + 1, logfile);

    IndentMe(indent + 1u, logfile);
    fprintf(logfile, "right:\t\n");
    OpPartialTextDump(node->right, indent + 1, logfile);

    return 0;
}

void IndentMe(size_t count, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, );
    for(size_t i = 0; i < count; i++) fprintf(logfile, "\t");
}

double OpEval(Root* root, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(root == nullptr, "Input root is nullptr!\n", logfile, -1);

    return OpPartialEval(root->init_node, logfile);
}

double OpPartialEval(Node* node, FILE* logfile){

    if(node == nullptr){
        #ifdef DEBUG
            fprintf(logfile, "Current node is: nullptr\n");
        #endif
        return 0;
    }

    double tmp = 0.;

    if(node->data.type != NONE){

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
                VERIFICATION(1, "Bad node type.", logfile, -1);
        }
    }else{
        return node->data.value;
    }

    return 0;
}

int OpTree2Text(Root* root, FILE* outputfile, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(outputfile == nullptr, "Input outputfile is nullptr!", logfile, -1);
    VERIFICATION(root == nullptr, "Input root is nullptr!", logfile, -1);

    OpPartialTree2Text(root->init_node, outputfile, logfile);

    return 0;
}

int OpPartialTree2Text(Node* node, FILE* outputfile, FILE* logfile){

    if(node == nullptr){
        #ifndef NONILL
            fprintf(outputfile, NILLCHAR);
        #endif

        return 0;
    }

    fprintf(outputfile, "(");

    OpPartialTree2Text(node->left, logfile, logfile);

    if(node->data_flag == VALUE){
        fprintf(outputfile, "%.2f", node->data.value);
    }else{
        fprintf(outputfile, "%c", node->data.type);
    }

    OpPartialTree2Text(node->right, logfile, logfile);

    fprintf(outputfile, ")");


    return 0;
}