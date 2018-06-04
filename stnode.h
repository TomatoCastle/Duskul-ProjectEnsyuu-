/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
#if !defined(__STNODE_H__)
#define __STNODE_H__

#include "commontype.h"

typedef enum {
    node_assign, node_call, node_print, node_input,
    node_break, node_return, node_if, node_while, node_for,
} nodetype;

typedef struct _stnode {
    struct _stnode *next;
    unsigned char  kind;    // nodetype
    unsigned short count;   // the number of args / vars
} stnode;

#endif /* !__STNODE_H__ */
