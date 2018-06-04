/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
#if !defined(__IDTABLE_H__)
#define __IDTABLE_H__

#include "getitem.h"

struct _idtable;
typedef struct _idtable  idtable;

int hash(const char *str);

idtable *idtableCreate(int capacity, bool dupflag);
void idtableFree(idtable *tabp);
item *idtableSearch(idtable *tabp, const char *name, int hashv);
item *idtableAdd(idtable *tabp, const char *name, int kind);
  // If the name exists in the table, the pointer to the info is returned.

#endif /* !__IDTABLE_H__ */
