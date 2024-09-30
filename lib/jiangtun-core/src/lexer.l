%option noyywrap
%option nounput
%option noinput
%option nounistd
%option reentrant
%{
#include <jiangtun.h>

#include "lex.yy.extern.h"

/* nounistd */
static int isatty(int fd) {
    (void)fd;
    return 1;
}

#define SET_PARTIAL(v)                                                         \
    do {                                                                       \
        jiangtun_yy_extra_t *extra = NULL;                                     \
        if ((extra = yyget_extra(yyscanner)) != NULL)                          \
            extra->partial = (v);                                              \
    } while (0)

#define SET_RETURN(v)                                                          \
    do {                                                                       \
        jiangtun_yy_extra_t *extra = NULL;                                     \
        if ((extra = yyget_extra(yyscanner)) != NULL)                          \
            extra->return_ = (v);                                              \
    } while (0)
%}

%%

    /*
     * NX Macro Controller
     */
^\xAB[\x00-\xFF]{0,9}   {
                            SET_PARTIAL(1);
                            yymore();
                        }
^\xAB[\x00-\xFF]{10}    {
                            SET_PARTIAL(0);
                            SET_RETURN(JIANGTUN_YY_MATCH_NXMC2);
                        }

    /*
     * Poke-Controller
     * 正規表現の簡略化のために、ボタン下位ビットで判定できる不正パターンを許容する
     */
^(0x) |
^(0x)?[0-9a-f]{1,4}\x20? |
    /* [048c] 末尾2bitが0b00 */
^(0x)?[0-9a-f]{1,4}\x20[0-8]\r? |
    /* [1235679abdef] 末尾2bitの少なくとも1つが0b1 */
^(0x)?[0-9a-f]{1,4}\x20[0-8]\x20? |
^(0x)?[0-9a-f]{1,4}\x20[0-8]\x20(0x) |
^(0x)?[0-9a-f]{1,4}\x20[0-8]\x20(0x)?[0-9a-f]{1,2}\x20? |
^(0x)?[0-9a-f]{1,4}\x20[0-8]\x20(0x)?[0-9a-f]{1,2}\x20(0x) |
    /* [12569ade] 末尾2bitのいずれかが0b1（ただしバグにより"3"はスティックが1つになる場合がある） */
^(0x)?[0-9a-f]{1,4}\x20[0-8]\x20(0x)?[0-9a-f]{1,2}\x20(0x)?[0-9a-f]{1,2}\r? |
    /* [37bf] 末尾2bitが0b11 */
^(0x)?[0-9a-f]{1,4}\x20[0-8]\x20(0x)?[0-9a-f]{1,2}\x20(0x)?[0-9a-f]{1,2}\x20? |
^(0x)?[0-9a-f]{1,4}\x20[0-8]\x20(0x)?[0-9a-f]{1,2}\x20(0x)?[0-9a-f]{1,2}\x20(0x) |
^(0x)?[0-9a-f]{1,4}\x20[0-8]\x20(0x)?[0-9a-f]{1,2}\x20(0x)?[0-9a-f]{1,2}\x20(0x)?[0-9a-f]{1,2}\x20? |
^(0x)?[0-9a-f]{1,4}\x20[0-8]\x20(0x)?[0-9a-f]{1,2}\x20(0x)?[0-9a-f]{1,2}\x20(0x)?[0-9a-f]{1,2}\x20(0x) |
^(0x)?[0-9a-f]{1,4}\x20[0-8]\x20(0x)?[0-9a-f]{1,2}\x20(0x)?[0-9a-f]{1,2}\x20(0x)?[0-9a-f]{1,2}\x20(0x)?[0-9a-f]{1,2}\r? |
^end? |
^end\r? {
            SET_PARTIAL(1);
            yymore();
        }
^(0x)?[0-9a-f]{1,4}\x20[0-8](\x20(0x)?[0-9a-f]{1,2}\x20(0x)?[0-9a-f]{1,2}(\x20(0x)?[0-9a-f]{1,2}\x20(0x)?[0-9a-f]{1,2})?)?\r?\n |
^end\r?\n   {
                SET_PARTIAL(0);
                SET_RETURN(JIANGTUN_YY_MATCH_POKECON);
            }

    /*
     * ORCA GC Controller
     */
^\x80\xFF*[\x00-\xFE]? |
^\x80\xFF*[\x00-\xFE]\xFF*  {
                                SET_PARTIAL(1);
                                yymore();
                            }
^(@|\x80\xFF*[\x00-\xFE]\xFF*[\x00-\xFE])   {
                                                SET_PARTIAL(0);
                                                SET_RETURN(JIANGTUN_YY_MATCH_ORCA);
                                            }

[\x00-\xFF] {
                SET_PARTIAL(0);
                SET_RETURN(JIANGTUN_YY_NO_MATCH);
            }

%%
