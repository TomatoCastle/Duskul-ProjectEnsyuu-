/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
#if !defined(__FUNC_IMP_H__)
#define __FUNC_IMP_H__

#include "stnode.h"

#define CONTROL_INFO_SIZE   0   // 実引数と局所変数の間に置かれる制御用ワードの数

struct _funcinfo {
    const char     *ident;
    stnode         *body;
    unsigned char  withbody;    // not prototype
    unsigned char  rtntype;     // 0:proc, 1:func
    short          params;      // number of parameters
    short          localvars;   // number of local variables
};
extern long *globals;


//グローバル変数宣言と同時代入用の構造体
typedef struct _global_var_assing_tmp {
    long value;//値
    long index;//globalvarsの添字
    struct _global_var_assing_tmp* next;
}global_var_assing_tmp;


#endif /* !__FUNC_IMP_H__ */
