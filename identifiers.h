/* Duskul version 0.1.3,  2018.05.29,   Takeshi Ogihara, (C) 2018 */
#if !defined(__IDENTIFIERS_H__)
#define __IDENTIFIERS_H__

#include "idtable.h"

extern idtable *idtableCurrent;

const char *reservedWord(int sym);

void idtablesInitialize(void);
idtable *idtablePush(void);
void idtablePop(void);
item *identifierSearch(const char *name, bool currentOnly);

#endif /* !__IDENTIFIERS_H__ */
