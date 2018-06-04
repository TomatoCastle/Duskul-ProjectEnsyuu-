/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
#if !defined(__STNODE_IMP_H__)
#define __STNODE_IMP_H__

#include "symset.h"
#include "stnode.h"
#include "expression.h"
#include "idtable.h"

typedef struct _assign {        // assign and return
    struct _stnode _padding;    // caution!
    unsigned char  global;
    unsigned short offset;
    // assign: global=[var is global?], offset=[offset of var]
    // return: global=[with expression?], offset=[--not used--]
    expnode        *expr;
} assignnode;

struct ifclause {
    expnode        *expr;
    struct _stnode *body;
};

typedef struct _ifnode {        // if-statement
    struct _stnode _padding;    // caution!
    struct ifclause  clause[1];
} ifnode;

typedef struct _whilenode {     // while-statement
    struct _stnode _padding;    // caution!
    expnode        *expr;
    struct _stnode *body;
} whilenode;

typedef struct _fornode {
    struct _stnode _padding;    // caution!
    unsigned char  global;      // var is global ?
    unsigned short offset;      // offset of var
    expnode        *exps[3];
    struct _stnode *body;       // code block
} fornode;

typedef struct _argnode {
    struct _stnode _padding;        // caution!
    unsigned short offset;          // index of proc / println ?
    union {
        expnode        *xlist[1];   // call, print, println
        varinfo        vlist[1];    // input
    } p; // parameters
} argnode;

/* newnode.c */
stnode *newNode(nodetype kind);
stnode *newNodeExpand(nodetype kind, int count);

/* statements.c */
extern int currentBreakNest;
extern bool valueIsReturned;
extern symset_t end_set;

stnode *fetchStatement(item ahead);
stnode *codeblock(symset_t termset, bool rtnflag);

/* simplestat.c */
stnode *assignStatement(item ahead, symset_t terminator);
stnode *returnStatement(symset_t terminator);
stnode *inputStatement(void);
stnode *forStatement(void);

#endif /* !__STNODE_IMP_H__ */
