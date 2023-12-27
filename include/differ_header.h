#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>

#define EPS 1e-2

#define DEBUG

#define NILLCHAR "_"

#define NONILL

#define MAX_VARS_COUNT 128

#define MAX_VAR_NAME_LENGTH 128

union NodeData{
    double value;
    char type;
};

enum types{

    SUMM            = '+',
    SUBTRACTION     = '-',
    MULTIPLICATION  = '*',
    DIVISION        = '/',
    NONE            = '\0',
    OP              = 0u,
    VALUE           = 1u,
    VAR             = 2u

};

struct Node{
    Node* left = nullptr;
    Node* right = nullptr;
    NodeData data = {};
    unsigned int data_flag : 2;
};

struct Variable{
    char *name = nullptr;
    double value = 0.;
    Node* node_p = nullptr;
};

struct Root{
    Node* init_node = nullptr;
    Variable *vars  = nullptr;
    unsigned int vars_count = 0;
};


#define VERIFICATION_LOGFILE(logfile, retval) if(logfile == nullptr){ printf("\033[1;31mError\033[0m: logfile is nullptr!\nExiting...\n"); return retval;}

#define ECHO(logfile) fprintf(logfile, "[%s, %d]\n", __FUNCTION__, __LINE__);                                          \
    printf("[%s, %d]\n", __FUNCTION__, __LINE__);

#define VERIFICATION(condition, message, logfile, ret_val)     if(condition){                                          \
        fprintf(logfile, "[%s, %d]Error:\t" message "\nExiting...\n", __FUNCTION__, __LINE__);                         \
        return ret_val;                                                                                                \
    }

#define RETURN_CHECK(ret_val) do{ if(ret_val){ printf("\033[1;31mError\033[0m: bad return value: %d\n", ret_val); \
        return ret_val; }}while(0)

#define GRAPHVIZ_INIT "digraph g {\nfontname=\"Helvetica,Arial,sans-serif\"\n\
node [fontname=\"Helvetica,Arial,sans-serif\"]\n\
edge [fontname=\"Helvetica,Arial,sans-serif\"]\n\
node [\n\tfontsize = \"16\"\n\tshape = \"ellipse\"\n];\n\n\
edge [ ];\n\n"

#define GRAPHVIZ_MKNODE_VALUE(name, data) #name "[\n\tlabel = \"{<f0> " data "}\"\n\tshape=record\n];\n\n"

#define GRAPHVIZ_MKNODE_OP(name, data) #name "[\n\tlabel = \"" data "\"\n\tshape=circle\n];\n\n"

#define GRAPHVIZ_MKNODE_VAR(name, data) #name "[\n\tlabel = \"" data "\"\n\tshape=diamond\n];\n\n"

#define GRAPHVIZ_CONNECT_NODE(first_id, second_id) first_id " -> " second_id "\n\n"

#define GRAPHVIZ_END "}\n"

int RootCtor(Root* root, FILE* logfile);

Node* OpNew(unsigned int data_flag, double value, FILE* logfile);

int OpPartialGraphDump(const Node* node, FILE* dotfile, unsigned char ip, unsigned char depth, FILE* logfile);

int OpGraphDump(const Root* root, FILE* dotfile, FILE* logfile);

int OpPartialTextDump(const Node* node, size_t indent, FILE* logfile);

inline void IndentMe(size_t count, FILE* logfile);

int OpTextDump(const Root* root, FILE* logfile);

double OpEval(const Root* root, FILE* logfile);

double OpPartialEval(const Node* node, FILE* logfile);

inline bool IsEqual(double a, double b){
    return fabs(a - b) < EPS;
}

int OpTree2Text(const Root* root, FILE* outputfile, FILE* logfile);

int OpPartialTree2Text(const Node* node, FILE* outputfile, FILE* logfile);

