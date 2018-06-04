/* Duskul version 0.1.3,  2018.05.29,   Takeshi Ogihara, (C) 2018 */
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "idtable.h"
#include "abort.h"

#define HASH_MAX       INT_MAX
#define TAB_SZ         128
#define STRPOOLSZ(x)   ((x) * 4)

struct _idtable {       // hash table
    int capacity;
    int entries;
    int emptyindex;     // Recently accessed empty entry. (-1 shows no entry)
                        // After search, new one will be inserted here.
    int strcapacity;
    int strindex;
    char *pool;         // Defined identifiers (var, func, or proc) are saved.
    item *table;
};

int hash(const char *str)
{
    const unsigned char *ustr = (unsigned char *)str;
    int x, ch;
    unsigned int v;
    if ((v = ustr[0]) == 0) return 0;
    if ((ch = ustr[1]) == 0) return v;
    v = (v * 148) ^ ch;
    for (x = 2; (ch = ustr[x]) != 0; x++) {
        v = ((v * 9) & HASH_MAX) ^ ch;
    }
    return (int)v;
}

idtable *idtableCreate(int capacity, bool dupflag)
{
    idtable *p = malloc(sizeof(idtable));
    p->capacity = (capacity == 0) ? TAB_SZ : capacity;
    p->entries = 0;
    p->emptyindex = -1; // none
    p->table = calloc(p->capacity, sizeof(item));
    if (dupflag) {
        p->strcapacity = STRPOOLSZ(p->capacity);
        p->strindex = 0;
        p->pool = malloc(p->strcapacity);
    }else {
        p->strcapacity = p->strindex = 0;
        p->pool = NULL;
    }
    return p;
}

void idtableFree(idtable *tabp) {
    if (tabp == NULL) return;
    free(tabp->table);
    if (tabp->pool) free(tabp->pool);
    free(tabp);
}

item *idtableSearch(idtable *tabp, const char *name, int hashv)
{
    int h = hashv % tabp->capacity;
    for ( ; ; ) {
        const char *str = tabp->table[h].a.name;
        if (str == NULL) {
            tabp->emptyindex = h;
            return NULL;                // not found
        }
        if (strcmp(str, name) == 0) {
            tabp->emptyindex = -1;
            break;                      // found successfully
        }
        h = (h + 17) % tabp->capacity;  // 17 is a prime number.
    }
    return &tabp->table[h];
}

static const char *duplicate_str(idtable *tabp, const char *str)
{
    char *p = tabp->pool;
    assert(p != NULL);
    int length = (int)strlen(str) + 1;
    if (tabp->strindex + length >= tabp->strcapacity)
        abortMessage("many ident"); // ERROR
    char *headp = &p[tabp->strindex];
    strcpy(headp, str);
    tabp->strindex += length;
    return headp;
}

item *idtableAdd(idtable *tabp, const char *name, int kind)
{
    if (tabp->entries >= tabp->capacity)
        abortMessage("many ident"); // ERROR
    item *ent = idtableSearch(tabp, name, hash(name));
    if (ent != NULL)
        return NULL; // the same name exists.
    assert(tabp->emptyindex >= 0);
    ent = &tabp->table[tabp->emptyindex];
    ent->a.name = (tabp->pool == NULL) ? name : duplicate_str(tabp, name);
    ent->token = tok_id;
    ent->kind = kind;
    tabp->entries++;
    return ent;
}
