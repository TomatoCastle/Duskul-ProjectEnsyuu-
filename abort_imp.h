/* Duskul version 0.1.3,  2018.05.31,   Takeshi Ogihara, (C) 2018 */
#if !defined(__ABORT_IMP_H__)
#define __ABORT_IMP_H__

struct _item;

const char *shorten(const char *s, int limit);
void itemToString(char *buffer, const struct _item *p);
#if defined(DEBUG)
void printItem(const struct _item *p);
#endif

const char *getErrorMessage(const char *key);

#endif /* !__ABORT_IMP_H__ */
