/* Duskul version 0.1.3,  2018.05.31,   Takeshi Ogihara, (C) 2018 */
#include <stdlib.h>
#include "getitem_imp.h"
#include "idtable.h"
#include "identifiers.h"
#include "abort.h"

#define IDENTMAX   128

static item unget_store;
static int unget_flag = false;

void ungetItem(item s)//おそらく最後に実行するもので、２回実行したらエラーとなる関数
{
    assert(unget_flag == false);
    unget_store = s;
    unget_flag = true;
}
//ここで、変数が宣言されているか確認する。ｇ
/// if currentOnly: not search the global table for the identifier.
///     New identifier may be added to the current table.
/// else: the global table is also used.  No identifier is added.
static item get_identifier(int ch, bool currentOnly, TIN *tip)
{
    int index = 0;
    char buf[IDENTMAX];//文字列
    int cc, attr;

    buf[index++] = ch;
    cc = nextch(tip);//tip情報を元に、つぎの文字をとってくる
    while ((attr = chAttribute(cc)) == ca_alpha || attr == ca_digit) {//トークンとなる文字の区切りを探してる
        if (index >= IDENTMAX - 1) {
            buf[IDENTMAX - 1] = 0;
            abortMessageWithString("long ident", buf);
        }
        buf[index++] = cc;//バッファーに貯め込む
        cc = nextch(tip);//次の文字を確認する
    }
    if (cc != EOF) undoch(cc, tip);
    buf[index] = 0;
    /**/
    item *ent = identifierSearch(buf, currentOnly);//おそらく、文字列を確認して、識別子(トークンの種類)が見つかればitemにそいつを入れる。
    if (ent == NULL) { // unknown id
        item s;
        if (!currentOnly) // Error
            abortMessageWithString("undef id", buf);
        //新しい変数を作る処理
        s.token = tok_id;
        s.kind = id_undefined;
        s.a.entptr = idtableAdd(idtableCurrent, buf, id_new);
        return s;
    }
    return *ent;
}

/// Get a token from the source program.
/// if currentOnly: not search the global table for the identifier.
item fgetItem(TIN *tip, bool currentOnly) {
    
    if (unget_flag) {
        unget_flag = false;
        return unget_store;
    }
    
    int ch = nextch(tip);
    if(ch == '/'){
        ch = nextch(tip);
        if(ch == '/'){
            while(ch != EOF && ch != '\n'){
                ch = nextch(tip);
            }
        }
        else{
            abortMessage("illegal statment");
        }
    }
    undoch(ch,tip);

    item s;
    chattr_t attr;
    do {
        ch = nextch(tip);
        attr = chAttribute(ch);
    }while (attr == ca_blank);
    switch (attr) {
        case ca_sym://予約文字
            return getsymbol(ch, tip);
        case ca_quot://コーテーションのことで文字列のとき
            s.token = tok_str;
            s.a.value = get_string(tip);
            return s;
        case ca_digit://整数値の時
            s.token = tok_num;
            s.a.value = getnumber(ch, tip);
            return s;
        case ca_alpha: return get_identifier(ch, currentOnly, tip);//変数名のときかな？
        default:
            break;
    }
    if (isEOF(tip))
        abortMessage("unexp EOF");
    s.token = tok_ILL;
    return s;
}

bool fgetEOF(TIN *tip) {//EOFを取得できたどうか
    int ch;
    do {
        ch = nextch(tip);
        if (ch == EOF) return true;
    }while (chAttribute(ch) == ca_blank);
    undoch(ch, tip);
    return false;
}
