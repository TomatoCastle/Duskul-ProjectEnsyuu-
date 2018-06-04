/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
#if !defined(__EXPRESSION_H__)
#define __EXPRESSION_H__

struct _expnode;
typedef struct _expnode  expnode;

typedef struct {
    unsigned char  global; // var is global ?
    unsigned short offset; // offset of var list
} varinfo;

/* expression.c */
expnode *expression(void);
expnode *strExpression(void);
void expressionList(expnode *xlist[], int args);

#endif /* !__EXPRESSION_H__ */
