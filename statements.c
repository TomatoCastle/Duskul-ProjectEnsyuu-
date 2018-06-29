/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
#include <stdio.h>
#include <stdlib.h>
#include "getitem.h"
#include "statements.h"
#include "func_imp.h"
#include "stnode_imp.h"
#include "expression.h"
#include "abort.h"

/* extern */
int currentBreakNest = 0;
bool valueIsReturned = false;
symset_t end_set;

static symset_t afterthen_set, stat_set, rtn_set;

void statInitialize(void)
{//starend,rtn,aterthenの役割を果たすシンボルを集合にしている。
    // C99 style literal
    stat_set = symsetCreate((token_t[]){
        tok_id, sym_call, sym_if, sym_while, sym_for, sym_print,
        sym_println, sym_input, tok_EOD });
    end_set = symsetCreate((token_t[]){ sym_end, tok_EOD });
    rtn_set = symsetCreate((token_t[]){ sym_break, sym_return, tok_EOD });
    afterthen_set = symsetCreate((token_t[]){ sym_else, sym_elsif, tok_EOD });
    symsetUnion(&afterthen_set, end_set);
}

#define MAX_IF_SEQ      64

// if文："if"を読んでから呼び出される
static stnode *ifStatement(void)
{
    struct ifclause buffer[MAX_IF_SEQ];
    item s;
    int index = 0;
    do {
        buffer[index].expr = expression();//式を入れている。
        s = getItem();
        if (s.token != sym_then)
            abortMessageWithToken("no then", &s);
        buffer[index].body = codeblock(afterthen_set, false);//bodyの中にif文の中のnodeが入る。
        if (++index >= MAX_IF_SEQ - 1)
            abortMessageWithString("many if", "elsif");
        s = getItem();
    }while (s.token == sym_elsif);
    if (s.token == sym_else) {
        buffer[index].expr = NULL;
        buffer[index++].body = codeblock(end_set, false);
        (void)getItem();
    }
    stnode *stp = newNodeExpand(node_if, index);
    ifnode *ifp = (ifnode *)stp;
    for (int x = 0; x < index; x++)
        ifp->clause[x] = buffer[x];
    return stp;
}
//whileぶんの構文解析を行う部分。
static stnode *whileStatement(void)
{
    stnode *stp = newNode(node_while);
    whilenode *whp = (whilenode *)stp;
    whp->expr = expression();
    item s = getItem();
    if (s.token != sym_do)
        abortMessageWithToken("no do", &s);
    currentBreakNest++;
    whp->body = codeblock(end_set, false);
    (void)getItem();
    currentBreakNest--;
    return stp;
}
//call文の解析を行う部分。mian関数の追加はここをさんこうにしてc言語のmain関数に、ぽいって投げるか、functionぶんにぽいってなゲル化すると、効果的な気がする。
static stnode *callStatement(void)
{
    item pr = getItem();
    if (pr.token != tok_id || pr.kind != id_proc)
        abortMessageWithToken("not proc name", &pr);
    int num = functionsTable[pr.offset]->params;
    stnode *stp = newNodeExpand(node_call, num);
    stp->count = num;
    argnode *anp = (argnode *)stp;
    anp->offset = pr.offset;
    expressionList(anp->p.xlist, num);
    return stp;
}
//print文の解析を行う部分。
static stnode *printStatement(int sym)
{
    expnode *buffer[PARAM_MAX];
    item s = getItem();
    if (s.token != sym_lpar)
        abortMessageWithToken("no left paren", &s);
    int num = 0;
    s = getItem();
    if (s.token != sym_rpar) {
        ungetItem(s);
        do {
            if (num >= PARAM_MAX)
                abortMessageWithString("wrong arg num", "print");
            buffer[num++] = strExpression();
            s = getItem();
        }while(s.token == sym_comma);
        if (s.token != sym_rpar)
            abortMessageWithToken("no right paren", &s);
    }
    stnode *stp = newNodeExpand(node_print, num);
    stp->count = num;
    argnode *anp = (argnode *)stp;
    anp->offset = BOOL(sym == sym_println); // need new-line
    if (num > 0) {
        expnode **xlist = anp->p.xlist;
        for (int i = 0; i < num; i++)
            xlist[i] = buffer[i];
    }
    return stp;
}

stnode *fetchStatement(item ahead)
{
    switch (ahead.token) {
        case sym_call:
            return callStatement();
        case sym_print:
        case sym_println:
            return printStatement(ahead.token);
        case sym_input:
            return inputStatement();
        case sym_if:
            return ifStatement();
        case sym_while:
            return whileStatement();
        case sym_for:
            return forStatement();
        default:
            break; // error
    }
    assert(false); // error
    return NULL;
}

//コードの中のブロックを解析して、適切なノードで返す関数。　ほとんどの関数でbodyの中に乖離地が入れられている。 文列：終わりを表すトークンの集合を第１引数に与える。終わりのトークンは ungetItem() される。
stnode* fcodeblock(symset_t termset, bool rtnflag,int* vars,int isFuncBody)
{
    stnode *root = NULL; //nodeの先頭部分の場所を表すポインタ
    stnode **statmp = &root;//現在対象としているノードを入れるポインタ　初期値は先頭root
    stnode *nodp = NULL;
    symset_t exit_set = termset;        // sym_end or sym_elsif, etc.
    symsetUnion(&exit_set, rtn_set);    // += sym_return, sym_break
    symset_t assign_set = exit_set;
    symsetUnion(&assign_set, stat_set); // += while, for, if, call, etc.
    
    //todo ここで値の容量確保を行う
    if (isFuncBody == 1){
        item s = getItem();
        while (s.token == sym_var) {
            *vars = var_list(*vars, false);
            s = getItem();
        }
        ungetItem(s);
    }
    /* */
    
    item s = getItem();
    while (symsetHas(stat_set, s.token)) {
        nodp = (s.token == tok_id)
             ? assignStatement(s, assign_set) : fetchStatement(s);
        *statmp = nodp;
        statmp = &nodp->next;
        s = getItem();
    }
    if (!symsetHas(exit_set, s.token))  // terminator of statement-list ?
        abortMessageWithToken("illegal statement", &s);
    bool has_rtn = false;
    if (s.token == sym_return || s.token == sym_break) {
        if (s.token == sym_break) {
            //breakであった場合
            if (currentBreakNest == 0)//コードの深さがない。(抜ける対象がない。)
                abortMessage("illegal break");
            nodp = newNode(node_break);
        }else { // sym_returnであったばあい
            nodp = returnStatement(termset);
            has_rtn = true;
        }
        *statmp = nodp;
        s = getItem();
    }
    if (!symsetHas(termset, s.token)) {
        const char *msg = symsetHas(termset, sym_end)
                          ? "no end" : "illegal statement";
        abortMessageWithToken(msg, &s);
    }
    ungetItem(s);
    if (rtnflag && !has_rtn)
        abortMessage("return needed");
    return root;
}
