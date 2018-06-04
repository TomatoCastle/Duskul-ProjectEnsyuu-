/* Duskul version 0.1.3,  2018.05.31,   Takeshi Ogihara, (C) 2018 */
#include <stdlib.h>
#include <string.h>
#include "identifiers.h"

#define GLOBAL_ID_MAX  512

static const char *reserved[] = {
    "and", "break", "call", "declare", "do",
    "else", "elsif", "end", "for", "func",
    "if", "input", "not", "or", "print",
    "println", "proc", "return", "step", "then",
    "to", "var", "while",
    NULL
};
// Note that the order of strings must be the same as the order of
// the enumerated symbols; sym_and ... sym_while.

/* extern */
idtable *idtableCurrent = NULL;
static idtable *resv_table;
static idtable *idTables[3];    // idTables[0] is always the Global ID-Table.
static int idtable_index = 0;

const char *reservedWord(int sym) {
    if (sym >= reserved_word_0 && sym < all_normal_symbols)
        return reserved[sym - reserved_word_0];
    return ""; // ERROR
}

void idtablesInitialize(void)
{
    int cap = 2 * sizeof(reserved) / sizeof(const char *);
    resv_table = idtableCreate(cap, false);
    const char *w;
    int sym = reserved_word_0;
    for (int x = 0; (w = reserved[x]) != NULL; x++) {
        item *ent = idtableAdd(resv_table, w, id_resvd);
        ent->token = sym++;
    }

    idTables[0] = idtableCreate(GLOBAL_ID_MAX, true);
    idtablePop(); // simply initialize ID-tables
}

/// Add a Local ID-table; therefore, Global, Local, and Reserved are set.
idtable *idtablePush(void)
{
    idTables[1] = idtableCurrent = idtableCreate(0, true);
    idtable_index = 2;
    idTables[idtable_index] = resv_table;
    return idtableCurrent;
}

/// Remove the Local ID-table; Global and Reserved are available.
void idtablePop(void)
{
    if (idtable_index > 1) idtableFree(idTables[1]);
    idtable_index = 1;
    idTables[idtable_index] = resv_table;
    idtableCurrent = idTables[0];
}

item *identifierSearch(const char *name, bool currentOnly)
{
    int hashv = hash(name);
    int limit = currentOnly ? (idtable_index - 1) : 0;
    for (int idx = idtable_index; idx >= limit; idx--) {
        item *ent = idtableSearch(idTables[idx], name, hashv);
        if (ent != NULL)
            return ent;
    }
    return NULL;
}
