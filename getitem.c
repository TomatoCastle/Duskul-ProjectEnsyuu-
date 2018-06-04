/* Duskul version 0.1.3,  2018.05.31,   Takeshi Ogihara, (C) 2018 */
#include <stdlib.h>
#include "getitem_imp.h"
#include "idtable.h"
#include "identifiers.h"
#include "abort.h"

#define IDENTMAX   128

static item unget_store;
static int unget_flag = false;

void ungetItem(item s)
{
    assert(unget_flag == false);
    unget_store = s;
    unget_flag = true;
}

/// if currentOnly: not search the global table for the identifier.
///     New identifier may be added to the current table.
/// else: the global table is also used.  No identifier is added.
static item get_identifier(int ch, bool currentOnly, TIN *tip)
{
    int index = 0;
    char buf[IDENTMAX];
    int cc, attr;

    buf[index++] = ch;
    cc = nextch(tip);
    while ((attr = chAttribute(cc)) == ca_alpha || attr == ca_digit) {
        if (index >= IDENTMAX - 1) {
            buf[IDENTMAX - 1] = 0;
            abortMessageWithString("long ident", buf);
        }
        buf[index++] = cc;
        cc = nextch(tip);
    }
    if (cc != EOF) undoch(cc, tip);
    buf[index] = 0;

    item *ent = identifierSearch(buf, currentOnly);
    if (ent == NULL) { // unknown id
        item s;
        if (!currentOnly) // Error
            abortMessageWithString("undef id", buf);
        s.token = tok_id;
        s.kind = id_undefined;
        s.a.entptr = idtableAdd(idtableCurrent, buf, id_new);
        return s;
    }
    return *ent;
}

/// Get a token from the source program.
/// if currentOnly: not search the global table for the identifier.
item fgetItem(TIN *tip, bool currentOnly) {
    if (unget_flag) {
        unget_flag = false;
        return unget_store;
    }

    item s;
    int ch;
    chattr_t attr;
    do {
        ch = nextch(tip);
        attr = chAttribute(ch);
    }while (attr == ca_blank);
    switch (attr) {
        case ca_sym:
            return getsymbol(ch, tip);
        case ca_quot:
            s.token = tok_str;
            s.a.value = get_string(tip);
            return s;
        case ca_digit:
            s.token = tok_num;
            s.a.value = getnumber(ch, tip);
            return s;
        case ca_alpha: return get_identifier(ch, currentOnly, tip);
        default:
            break;
    }
    if (isEOF(tip))
        abortMessage("unexp EOF");
    s.token = tok_ILL;
    return s;
}

bool fgetEOF(TIN *tip) {
    int ch;
    do {
        ch = nextch(tip);
        if (ch == EOF) return true;
    }while (chAttribute(ch) == ca_blank);
    undoch(ch, tip);
    return false;
}
