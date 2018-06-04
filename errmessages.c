/* Duskul version 0.1.3,  2018.05.31,   Takeshi Ogihara, (C) 2018 */
#include <string.h>

static const char *table[] = {
    "no source",      "ソースプログラムを指定して下さい",
    "unexp EOF",      "予期しないEOF",
    "func or var",  "'func', 'proc', 'var' または 'declare' があるべきです",
    "no id",          "識別子がありません",
    "no equal",       "'=' がありません",
    "no then",        "'then' がありません",
    "no do",          "'do' がありません",
    "no end",         "'end' がありません",
    "no left paren",  "'\x28' がありません",
    "no right paren", "'\x29' がありません",
    "no to",          "'to' がありません",
    "no func",        "'func' がありません",
    "illegal string", "文字列が不正です",
    "illegal statement", "文が不正です",
    "illegal tail",   "文末に不正な文字があります",
    "wrong exp",      "式として正しくない要素が現れました",
    "wrong proto",    "サブルーチンの宣言が一致しません",
    "not func name",  "関数の名前ではありません",
    "not proc name",  "手続きの名前ではありません",
    "w-def func",     "同名のサブルーチンがすでに定義されています",
    "w-def param",    "同名の引数があります",
    "id conflict",    "同じ名前で定義されたものがあります",
    "undef id",       "変数が未定義です",
    "many if",        "if構文の連続が長すぎます",
    "many func",      "サブルーチンの数が多すぎます",
    "many param",     "仮引数の個数が多すぎます",
    "long string",    "文字列が長すぎます",
    "long ident",     "識別子が長すぎます",
    "many ident",     "識別子が多すぎます",
    "wrong arg num",  "引数の個数が間違っています",
    "assign param",   "仮引数に代入はできません",
    "assign func",    "サブルーチン名に代入はできません",
    "illegal break",  "ここでbreak文は利用できません",
    "return needed",  "returnで値を返す必要があります",
    "no main",        "main関数がありません",
    "no func body",   "サブルーチンの本体がありません",
    "stack overflow", "スタックがあふれました",
    "arith exception", "算術演算でエラーが発生しました",
    "ill. opcode",    "不正な命令です",
    "input eof",      "入力がクローズされています",
    NULL, NULL,
};

const char *getErrorMessage(const char *key)
{
    for (int i = 0; table[i]; i += 2) {
        if (strcmp(key, table[i]) == 0)
            return table[i+1];
    }
    return key;
}
