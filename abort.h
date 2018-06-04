/* Duskul version 0.1.2,  2018.05.13,   Takeshi Ogihara, (C) 2018 */
#if !defined(__ABORT_H__)
#define __ABORT_H__

struct _item;

void abortMessage(const char *msg);
void abortMessageWithToken(const char *msg, const struct _item *s);
void abortMessageWithString(const char *msg, const char *str);

#if defined(DEBUG)
void _assert(int cond, const char *fn, int line);
#  define   assert(c)   _assert((c), __FILE__, __LINE__)
#else
#  define   assert(c)   // ignored
#endif

#endif /* !__ABORT_H__ */
