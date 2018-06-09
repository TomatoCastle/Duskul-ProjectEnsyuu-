/* Duskul version 0.1.4,  2018.06.07,   Takeshi Ogihara, (C) 2018 */
#include <stdio.h>
#include "getitem.h"
#include "symset.h"
#include "identifiers.h"
#include "statements.h"
#include "abort.h"
#include "execute.h"
#define  VERSION    "0.1.4"

/// Call several functions for initialization at once
static void initialize(void)
{
    idtablesInitialize();
    statInitialize();
    symInitialize();
}

int main(int argc, char **argv)
{
    int code = 0;
    bool runflag = true;
    int ac = 1;
    while (ac < argc) {
        if (argv[ac][0] == '-') {
            switch(argv[ac][1]) {
            case 'p':
                runflag = false; break;
            case 'h':
            default:
                fprintf(stderr, "Duskul version " VERSION "\n");
                fprintf(stderr, "Usage: %s [options] input\n", argv[0]);
                fprintf(stderr,
                    "Options:  -p    Parsing only\n"
                    "          -h    this help\n"
                );
                return 1;
            }
        }else
            break;
        ac++;
    }
    FILE *fp = NULL;
    if (ac >= argc)
        abortMessage("no source");
    if ((fp = fopen(argv[ac], "r")) == NULL) {
        fprintf(stderr, "Error: can't open: %s\n", argv[ac]);
        return 1;
    }
    stdtxin = getTextBuffer(fp);
    initialize();
    int mainindex = parseProgram();
    freeTextBuffer(stdtxin);
    stdtxin = NULL;
    if (!runflag)
        return code;

    if (mainindex < 0)
        abortMessage("no main");
    code = executeProgram(mainindex);
    return code;
}
