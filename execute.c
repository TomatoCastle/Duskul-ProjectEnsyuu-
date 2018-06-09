/* Duskul version 0.1.4,  2018.06.08,   Takeshi Ogihara, (C) 2018 */

#include <stdio.h>
#include <stdlib.h>
#include "execute.h"
#include "evaluate.h"
#include "func_imp.h"
#include "exp_imp.h"
#include "statements.h"
#include "stnode_imp.h"
#include "strliteral.h"
#include "abort.h"

#define  STACK_SIZE     (1024 * 4)

/* extern */
long *stack, *globals;
// typical registers
int sp;             // stack pointer
int localbase = 0;  // index of 1st local var
static long rtnvalue;   // returned value

typedef enum { ex_normal, ex_break, ex_return } ex_condition;

// prototypes
static ex_condition execStatements(const stnode *stptr);

static void execAssign(const assignnode *asp)
{
    int idx = asp->offset;
    long *target = asp->global ? &globals[idx] : &stack[localbase - idx];
    evaluate(asp->expr);
    *target = stack[sp++];
}

static void execReturn(const assignnode *asp)
{
    if (asp->global) {
        evaluate(asp->expr);
        rtnvalue = stack[sp++];
    }else
        rtnvalue = 0;
}

static ex_condition execIf(const ifnode *ifptr, int count)
{
    int i;
    for (i = 0; i < count; i++) {
        const expnode *ex = ifptr->clause[i].expr;
        if (ex == NULL) break; // else-clause
        evaluate(ex);
        if (stack[sp++] != 0) break;
    }
    if (i < count)
        return execStatements(ifptr->clause[i].body);
    return ex_normal;
}

static ex_condition execWhile(const whilenode *whp)
{
    ex_condition r = ex_normal;
    do {
        evaluate(whp->expr);
        if (stack[sp++] == 0) break;
        r = execStatements(whp->body);
    }while (r == ex_normal);
    return (r == ex_return) ? ex_return : ex_normal;
}

static ex_condition execFor(const fornode *frp)
{
    ex_condition r = ex_normal;
    long upper, step = 1;
    int idx = frp->offset;
    long *target = frp->global ? &globals[idx] : &stack[localbase - idx];
    evaluate(frp->exps[0]);
    *target = stack[sp++];
    evaluate(frp->exps[1]);
    upper = stack[sp++];
    if (frp->exps[2]) {
        evaluate(frp->exps[2]);
        step = stack[sp++];
        if (step == 0)
            return ex_normal;
    }
    while (1) {
        if (step > 0 && *target > upper) break;
        if (step < 0 && *target < upper) break;
        r = execStatements(frp->body);
        if (r == ex_return) return ex_return;
        if (r == ex_break) break;
        *target += step;
    }
    return ex_normal;
}

static ex_condition execStatements(const stnode *stptr)
{
    const stnode *stp = stptr;
    ex_condition r;
    while (stp) {
        switch (stp->kind) {
            case node_assign:
                execAssign((const assignnode *)stp); break;
            case node_call:
                execCall((const argnode *)stp, stp->count); break;
            case node_print:
                execPrint((const argnode *)stp, stp->count);
                break;
            case node_input:
                execInput((const argnode *)stp, stp->count); break;
            case node_return:
                execReturn((const assignnode *)stp);
                return ex_return;
            case node_if:
                r = execIf((const ifnode *)stp, stp->count);
                if (r != ex_normal) return r;
                break;
            case node_while:
                r = execWhile((const whilenode *)stp);
                if (r != ex_normal) return r;
                break;
            case node_for:
                r = execFor((const fornode *)stp);
                if (r != ex_normal) return r;
                break;
            case node_break:
            default:
                return ex_break;
        }
        stp = stp->next;
    }
    return ex_normal;
}

void subroutine(int index)
{
    funcinfo *finf = functionsTable[index];
    int localbase_save = localbase;
    localbase = sp + finf->params - 1;
    int locals = finf->localvars + CONTROL_INFO_SIZE;
    if (sp - locals < STACK_LOW)
        abortMessage("stack overflow");
    while (locals--)
        stack[--sp] = 0;    // local vars

    (void)execStatements(finf->body);

    if (finf->rtntype) { // function
        sp = localbase;
        stack[sp] = rtnvalue;
    }else
        sp = localbase + 1;
    localbase = localbase_save;
}

int executeProgram(int mainidx)
{
    stack = malloc(sizeof(long) * STACK_SIZE);
    globals = malloc(sizeof(long) * numberOfStaticVars);
    sp = STACK_SIZE;
    funcinfo *finf = functionsTable[mainidx];
    for (int i = 0; i < finf->params; i++)
        stack[--sp] = 0;      // dummy arguments
    subroutine(mainidx);
    int rtncode = (sp < STACK_SIZE) ? (int)(stack[sp]) : 0;
    free(stack);
    free(globals);
    return rtncode;
}
