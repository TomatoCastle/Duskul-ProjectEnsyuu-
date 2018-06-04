/* Duskul version 0.1.3,  2018.05.29,   Takeshi Ogihara, (C) 2018 */
#if !defined(__GETITEM_H__)
#define __GETITEM_H__

#include <stdio.h>
#include "token.h"

typedef enum {
    id_resvd,      // reserved word
    id_static_v, id_local_v, id_param, id_func, id_proc,
    id_undefined, id_new
    // id_new is only used to temporarily keep a table item.
} idkind;

struct _item {
    token_t        token;
    unsigned char  kind;    // idkind or ASCII char
    short          offset;  // variables, index or funcs/procs,
    union { // associated data
        long       value;   // int value, index of string-literal
        const char *name;   // identifier of var, param, func, proc, undefined
        struct _item *entptr;  // if kind == id_undefined, used in fgetItem()
    } a;
};
typedef struct _item    item;

struct textinput; /* an incomplete type */
typedef struct textinput TIN;

extern TIN *stdtxin;

TIN *getTextBuffer(FILE *fp);
void freeTextBuffer(TIN *tip);
void printTextBuffer(FILE *fp, TIN *tip);
void ungetItem(item s);
item fgetItem(TIN *tip, bool currentOnly);
#define getItemLocal()  fgetItem(stdtxin, true)
#define getItem()       fgetItem(stdtxin, false)
bool fgetEOF(TIN *tip);
#define getEOF()        fgetEOF(stdtxin)
#endif /* !__GETITEM_H__ */
