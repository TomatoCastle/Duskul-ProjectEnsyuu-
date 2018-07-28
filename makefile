#CFLAGS = -std=c99 -Wall -DDEBUG -O0 -g
CFLAGS = -std=c99 -Wall -O2
##OBJ##
OBJ = abort.o errmessages.o evaluate.o execute.o \
    expression.o exptree.o functions.o getitem.o identifiers.o \
    idtable.o main.o newnode.o nextch.o printitem.o \
    simplestat.o statements.o strliteral.o symset.o

dusk: $(OBJ)
	$(CC) -o $@ $(OBJ)

clean:
	rm -rf *.o dusk graph.pdf *.dSYM

graph: makedot.sh
	./makedot.sh | dot -Tpdf -o graph.pdf
	open graph.pdf

make: make_template
	@ls *.c | sed 's/\.c/.o/' | awk -f mkobjs.awk > _tmp
	@sed '/^##OBJ##/r _tmp' make_template > makefile
	@cc -MM *.c >> makefile
	@rm _tmp

##DEPENDENCY##
abort.o: abort.c getitem.h token.h commontype.h abort_imp.h
errmessages.o: errmessages.c
evaluate.o: evaluate.c evaluate.h stnode_imp.h symset.h token.h \
  commontype.h stnode.h expression.h idtable.h getitem.h exp_imp.h \
  statements.h strliteral.h abort.h
execute.o: execute.c execute.h evaluate.h stnode_imp.h symset.h token.h \
  commontype.h stnode.h expression.h idtable.h getitem.h func_imp.h \
  exp_imp.h statements.h strliteral.h abort.h
expression.o: expression.c statements.h token.h commontype.h getitem.h \
  stnode.h symset.h idtable.h expression.h exp_imp.h abort.h
exptree.o: exptree.c token.h commontype.h exp_imp.h getitem.h \
  expression.h func_imp.h stnode.h statements.h symset.h idtable.h \
  abort.h
functions.o: functions.c getitem.h token.h commontype.h identifiers.h \
  idtable.h statements.h stnode.h symset.h stnode_imp.h expression.h \
  func_imp.h abort.h
getitem.o: getitem.c getitem_imp.h getitem.h token.h commontype.h \
  idtable.h identifiers.h abort.h
identifiers.o: identifiers.c identifiers.h idtable.h getitem.h token.h \
  commontype.h
idtable.o: idtable.c idtable.h getitem.h token.h commontype.h abort.h
main.o: main.c getitem.h token.h commontype.h symset.h identifiers.h \
  idtable.h statements.h stnode.h abort.h execute.h
newnode.o: newnode.c stnode_imp.h symset.h token.h commontype.h stnode.h \
  expression.h idtable.h getitem.h abort.h
nextch.o: nextch.c getitem_imp.h getitem.h token.h commontype.h \
  strliteral.h abort.h
printitem.o: printitem.c abort_imp.h identifiers.h idtable.h getitem.h \
  token.h commontype.h strliteral.h
simplestat.o: simplestat.c getitem.h token.h commontype.h statements.h \
  stnode.h symset.h idtable.h stnode_imp.h expression.h abort.h
statements.o: statements.c getitem.h token.h commontype.h statements.h \
  stnode.h symset.h idtable.h func_imp.h stnode_imp.h expression.h \
  abort.h
strliteral.o: strliteral.c
symset.o: symset.c symset.h token.h commontype.h abort.h
