/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
#if !defined(__EXP_IMP_H__)
#define __EXP_IMP_H__

#include "getitem.h"
#include "expression.h"

#define ExpTree     tok_ILL // mark of 'oprExpnode'

// Note: expnode, oprExpnode, and argExpnode have the common part:
// kind and prefix.

struct _expnode {
    token_t  kind;      // tok_id:variable, tok_num:int, tok_str: string-literal
    token_t  prefix;    // 0, sym_plus, sym_minus, sym_not
    union {
        long    intvalue;   // if kind == tok_num, or tok_str
        varinfo varinf;     // if kind == tok_id
    } v;
};

typedef struct _oprexp {
    token_t  kind;          // operators: sym_ast, sym_plus, ...
    token_t  prefix;        // always ExpTree
    expnode  *operand[2];
} oprExpnode;

typedef struct _argexp {
    token_t kind;       // sym_func
    token_t prefix;     // 0, sym_plus, sym_minus, sym_not
    short   index;      // index of functionsTable
    short   count;      // number of arguments
    expnode *args[1];
} argExpnode;

expnode *newExpnode(int prefix, int kind);
expnode *newOprnode(int kind, expnode *o1, expnode *o2);
argExpnode *newArgnode(int prefix, int funcindex, int argnum);
expnode *term(void);

#endif /* !__EXP_IMP_H__ */
