/* Duskul version 0.1.3,  2018.05.29,   Takeshi Ogihara, (C) 2018 */
#include <stdlib.h>
#include <string.h>
#include "getitem.h"
#include "identifiers.h"
#include "statements.h"
#include "stnode_imp.h"
#include "func_imp.h"
#include "abort.h"

#define FUNC_TABLE_CAPACITY   32

/* extern */
funcinfo *functionsTable[FUNC_TABLE_CAPACITY];
    // 関数、手続きの情報を格納する配列。構文木の根でもある。
int currentFuncIndex = 0;
int numberOfFunctions = 0;
int numberOfStaticVars = 0;

// 仮引数列: '(' が読まれてから呼び出される。最後の ')' は読む。
static int parameter_list(void)
{
    item s = getItemLocal();
    if (s.token == sym_rpar) // no parameters
        return 0;

    int prm = 0;
    for ( ; ; ) {
        if (s.token != tok_id) abortMessage("no id");
        if (s.kind != id_undefined) abortMessage("w-def param");
        item *ent = s.a.entptr;
        ent->kind = id_param;
        ent->offset = prm++;
        if (prm >= PARAM_MAX) abortMessage("many param");
        s = getItemLocal();
        if (s.token != sym_comma)
            break;
        s = getItemLocal();
    }
    if (s.token != sym_rpar) abortMessage("no right paren");
    return prm;
}

// 変数宣言: 'var' が読まれてから呼び出される。',' が出現しなくなったら終わり。
static int var_list(int offset, int global)
{
    item s;
    int vars = offset;
    do {
        s = getItemLocal();
        if (s.token != tok_id) abortMessage("no id");
        if (s.kind != id_undefined) abortMessage("id conflict");
        item *ent = s.a.entptr;
        ent->kind = global ? id_static_v : id_local_v;
        ent->offset = vars++;
        s = getItem();
    }while (s.token == sym_comma);
    ungetItem(s);
    return vars;
}

// 'func' or 'proc' が読まれてから呼び出される
// funcinfoをfunctionsTable[]に登録してその添字を返す
// 前方参照があった場合、すでに登録された分の添字を返す
static int func_header(bool isfunc, bool withbody)
{
    int funcindex = numberOfFunctions;
    bool hasproto = false;
    int func_proc = isfunc ? id_func : id_proc;
    item s = getItemLocal();
    if (s.token != tok_id)
        abortMessageWithToken("no id", &s);
    funcinfo *finf = NULL;
    if (s.kind == id_undefined) { // new function
        item *idp = s.a.entptr;
        idp->kind = func_proc;
        idp->offset = funcindex = numberOfFunctions++;
        if (numberOfFunctions >= FUNC_TABLE_CAPACITY)
            abortMessage("many func");
        functionsTable[funcindex] = finf = malloc(sizeof(funcinfo));
        finf->ident = idp->a.name;
        finf->withbody = withbody;
        finf->rtntype = isfunc;
    }else if (s.kind == id_func || s.kind == id_proc) {
        hasproto = true;
        if (s.kind != func_proc)
            abortMessageWithToken("wrong proto", &s);
        funcindex = s.offset;
        finf = functionsTable[funcindex];
        if (finf->withbody && withbody)
            abortMessageWithToken("w-def func", &s);
        if (withbody)
            finf->withbody = true;
    }else {
        abortMessageWithToken("not func name", &s);
    }

    item t = getItem();
    if (t.token != sym_lpar)
        abortMessageWithToken("no left paren", &t);
    idtablePush();
    int prms = parameter_list();
    if (hasproto) {
        if (finf->params != prms) abortMessageWithToken("wrong proto", &s);
    }else finf->params = prms;
    return funcindex;
}

/// Definition of a subroutine.
/// This is called after a token of 'func' or 'proc' is read.
static void funcDefine(bool isfunc)
{
    int fidx = func_header(isfunc, true); // idtablePush() is called
    funcinfo *fip = functionsTable[fidx];
    currentFuncIndex = fidx;
    currentBreakNest = 0;
    int porig = fip->params + CONTROL_INFO_SIZE;
    int vars = porig;
    item s = getItem();
    while (s.token == sym_var) {
        vars = var_list(vars, false);
        s = getItem();
    }
    ungetItem(s);
    valueIsReturned = isfunc;
    fip->body = codeblock(end_set, valueIsReturned);
    fip->rtntype = isfunc;
    fip->localvars = vars - porig;  // number of local variables
    (void)getItem();
    idtablePop();
}

/// Forward declaration: This is called after a token of 'declare' is read.
static void funcDeclare(void)
{
    item s = getItem();
    bool isfunc = false;
    if (s.token == sym_func) isfunc = true;
    else if (s.token != sym_proc)
        abortMessage("no func");
    int fidx = func_header(isfunc, false); // idtablePush() is called
    funcinfo *fip = functionsTable[fidx];
    fip->rtntype = isfunc;
    idtablePop();
}

/// Parse the top level definitions of subroutines and vars.
int parseProgram(void)
{
    int vars = 0;
    while (!getEOF()) {
        item s = getItem();
        if (s.token == sym_func || s.token == sym_proc) {
            bool isfunc = BOOL(s.token == sym_func);
            funcDefine(isfunc);
        }else if (s.token == sym_var)
            vars = var_list(vars, true);
        else if (s.token == sym_decl)
            funcDeclare();
        else {
            abortMessage("func or var");
        }
    }
    numberOfStaticVars = vars;

    int mainindex = -1;
    for (int i = 0; i < numberOfFunctions; i++) {
        const char *name = functionsTable[i]->ident;
        if (!functionsTable[i]->withbody)
            abortMessageWithString("no func body", name);
        if (strcmp(name, "main") == 0)
            mainindex = i;
    }
    return mainindex;
}
