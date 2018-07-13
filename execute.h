/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
#ifndef __EXECUTE_H__
#define __EXECUTE_H__

struct _expnode;

int executeProgram(int mainidx, int argc, char **argv, int ac);
void evaluate(const struct _expnode *expptr);

#endif /* __EXECUTE_H__ */
