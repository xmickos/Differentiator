#include "differ_header.h"
#include "RD_header.hpp"


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
            fprintf(logfile, "%f <-> %p\n", node->data.value, node);
        break;
        case OP:
            fprintf(logfile, "%c <-> %p\n", node->data.type, node);
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

int OpGraphDump(const Root* root, FILE* dotfile, FILE* logfile){
    VERIFICATION_LOGFILE(logfile, -1);
    VERIFICATION(root == nullptr, "Input root is nullptr!", logfile, -1);
    VERIFICATION(dotfile == nullptr, "Input dotfile is nullptr!", logfile, -1);

    fprintf(dotfile, GRAPHVIZ_INIT);

    switch(root->init_node->data_flag){
        case VALUE:
            fprintf(logfile, "[%s, %d] Value!\n", __FUNCTION__, __LINE__);
            fprintf(dotfile, GRAPHVIZ_MKNODE_VALUE("0/0", "%f"), root->init_node->data.value, root->init_node);
        break;
        case OP:
            fprintf(logfile, "[%s, %d] Op!\n", __FUNCTION__, __LINE__);
            fprintf(dotfile, GRAPHVIZ_MKNODE_OP("0/0", "%c"),
            root->init_node->data.type,
            root->init_node,
            nullptr,
            0.0,
            "\0"
            );
        break;
        case VAR:
            fprintf(logfile, "[%s, %d] Var!\n", __FUNCTION__, __LINE__);
            fprintf(dotfile, GRAPHVIZ_MKNODE_OP("0/0", "x"),
            root->init_node,
            root->init_node->data.name,
            root->init_node->data.value,
            root->init_node->data.name
            );
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
                fprintf(logfile, "[%s, %d] Value! %.4f -> %d/%d, old ip & depth: %d/%d\n",
                    __FUNCTION__,
                    __LINE__,
                    node->left->data.value,
                    new_ip, new_depth,
                    ip,
                    depth
                );
                fprintf(dotfile, GRAPHVIZ_MKNODE_VALUE("%d/%d", "%.4f"),
                    ip,
                    new_depth,
                    node->left->data.value,
                    node->left
                );
            break;
            case OP:
                fprintf(logfile, "[%s, %d] Op!\n", __FUNCTION__, __LINE__);
                fprintf(dotfile, GRAPHVIZ_MKNODE_OP("%d/%d", "%c"),
                    ip,
                    new_depth,
                    node->left->data.type,
                    node->left,
                    nullptr,
                    0.0,
                    "\0"
                );
            break;
            case VAR:
                fprintf(logfile, "[%s, %d] Var!\n", __FUNCTION__, __LINE__);
                fprintf(dotfile, GRAPHVIZ_MKNODE_OP("%d/%d", "x"),
                    ip,
                    new_depth,
                    node->left,
                    node->left->data.name,
                    node->left->data.value,
                    node->left->data.name
                );
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
                fprintf(dotfile, GRAPHVIZ_MKNODE_VALUE("%d/%d", "%.4f"),
                    new_ip,
                    new_depth,
                    node->right->data.value,
                    node->right
                );
            break;
            case OP:
                fprintf(logfile, "[%s, %d] Op!\n", __FUNCTION__, __LINE__);
                fprintf(dotfile, GRAPHVIZ_MKNODE_OP("%d/%d", "%c"),
                    new_ip,
                    new_depth,
                    node->right->data.type,
                    node->right,
                    nullptr,
                    0.0,
                    "\0"
                );
            break;
            case VAR:
                fprintf(logfile, "[%s, %d] Var!\n", __FUNCTION__, __LINE__);
                fprintf(dotfile, GRAPHVIZ_MKNODE_OP("%d/%d", "x"),
                    new_ip,
                    new_depth,
                    node->right,
                    node->right->data.name,
                    node->right->data.value,
                    node->right->data.name
                );
            break;
            default:
                fprintf(logfile, "[%s, %d]: Wrong node data type.\nExiting...\n", __FUNCTION__, __LINE__);
                return -1;
        }

        fprintf(dotfile, GRAPHVIZ_CONNECT_NODE("\"%d/%d\"", "\"%d/%d\""), ip, depth, new_ip, new_depth);
    }

    // printf("Going down with left dataflag/value = %d/%d and right dataflag/value = %d/%d\n",
    // node->left->data_flag, node->left->data.value, node->right->data_flag, node->right->data.value);
    OpPartialGraphDump(node->left, dotfile, ip, new_depth, logfile);
    OpPartialGraphDump(node->right, dotfile, new_ip, new_depth, logfile);


    return 0;
}
