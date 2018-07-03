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

global_var_assing_tmp* root = NULL; //nodeの先頭部分の場所を表すポインタ
global_var_assing_tmp** varasstmp = &root;//現在対象としているglobalのあれ。

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
int var_list(int offset, int global, stnode* nodp, stnode*** statmp, symset_t* assign_set)
{
    item s;
    int vars = offset;
    
    if(global == 1){
        *varasstmp = malloc(sizeof(global_var_assing_tmp));
        (*varasstmp)->next = NULL;
        (*varasstmp)->index=0;
    }

    do {
        s = getItemLocal();
        symset_t connma_and_assign_set = symsetCreate((token_t[]){sym_comma,sym_var});
        if(global == 0){
        symsetUnion(&connma_and_assign_set, *assign_set);
        }
        if (s.token != tok_id) abortMessage("no id");
        if (s.kind != id_undefined) abortMessage("id conflict");
        
        item *ent = s.a.entptr;
        ent->kind = global ? id_static_v : id_local_v;
        ent->offset = vars++;
        //if(s.token == sym_eq)
       // item bfs = s; //前の奴を残しておく
        s = getItem();
        if(s.token == sym_eq && global == 0){ //ローカル変数である場合 代入
            ungetItem(s);
            nodp = assignStatement(*ent, connma_and_assign_set);
            /* *statmp = nodp;
            statmp = &nodp->next;*/
             **statmp = nodp;
             *statmp = &nodp->next;
            s = getItem();
        }
        
        if(s.token == sym_eq && global == 1){//グローバル変数である場合である場合。
            int minus_flg=0;
            s = getItem();
            if(s.token == sym_plus){
                s = getItem();
            }else if(s.token == sym_minus){
                minus_flg = 1;
                s = getItem();
            }
            if(s.token ==  tok_num){
                if(minus_flg == 1) (*varasstmp)->value = -1 * s.a.value;
                else (*varasstmp)->value = s.a.value;
                (*varasstmp)->index=vars-1;
                (*varasstmp)->next = malloc(sizeof(global_var_assing_tmp));
                *varasstmp = (*varasstmp)->next;
                (*varasstmp)->next = NULL;
                (*varasstmp)->index=0;
            } else {
                abortMessageWithToken("wrong exp", &s);
            }
            s = getItem();
        }
       
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
    // todo 変更する！！！
    int vars = porig;
    
    /*item s = getItem();
    while (s.token == sym_var) {
        vars = var_list(vars, false);
        s = getItem();
    }*/
    valueIsReturned = isfunc;
    fip->body = fcodeblock(end_set, valueIsReturned,&vars,1);
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
            vars = var_list(vars, true,0,0,0);
        else if (s.token == sym_decl)
            funcDeclare();
        else {
            abortMessage("func or var");
        }
    }
    numberOfStaticVars = vars;
    globals = malloc(sizeof(long) * numberOfStaticVars);//ここで、グローバル変数の容量確保。
    //ここでなぜか失敗している。NULLが必ず入ってる...なぜだ...
    varasstmp = &root;
    while (root != NULL && (*varasstmp)->next != NULL){
        globals[(*varasstmp)->index] = (*varasstmp)->value;
        *varasstmp = (*varasstmp)->next;
    }
    
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
