/* Duskul version 0.1.2,  2018.05.13,   Takeshi Ogihara, (C) 2018 */

#include "getitem.h"

typedef enum {
    ca_nul = 0, ca_blank = 1, ca_sym, ca_quot, ca_esc,
    ca_digit, ca_alpha,
    ca_instr = 0x10,
} chattr_t;

struct textinput {
    FILE *fp;
    unsigned char *buf;
    int lines, index, capacity;
    int ungetch;
};
#define isEOF(t)    feof((t)->fp)   // TIN *t;

chattr_t chAttribute(int x);
int nextch(TIN *tip);
void undoch(int ch, TIN *tip);
long getnumber(int ch, TIN *tip);
item getsymbol(int ch, TIN *tip);
int get_string(TIN *tip);
