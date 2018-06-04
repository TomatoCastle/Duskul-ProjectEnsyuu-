/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
#if !defined(__STATEMENTS_H__)
#define __STATEMENTS_H__

#include "token.h"
#include "getitem.h"
#include "stnode.h"
#include "symset.h"
#include "idtable.h"

#define PARAM_MAX    16

struct _funcinfo;
typedef struct _funcinfo  funcinfo;

/* functions.c */
extern funcinfo *functionsTable[];
extern int currentFuncIndex;
extern int numberOfFunctions;
extern int numberOfStaticVars;

int parseProgram(void);

/* statements.c */
void statInitialize(void);

#endif /* !__STATEMENTS_H__ */
