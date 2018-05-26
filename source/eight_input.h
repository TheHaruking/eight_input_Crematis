//---------------------------------------------------------------------------------
#ifndef _eight_input_h_
#define _eight_input_h_
//---------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//---------------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>

/*
    ボタン入力活用ライブラリ
       ～ くれまちす ～     
*/

// 当ライブラリのボタン配列
typedef struct {
    union { uint32_t all;
        struct {
            union { uint8_t byte0;
                struct { bool A   :1, B   :1, sele:1, strt:1,
                              u0  :1, d0  :1, l0  :1, r0  :1;
            }; };
            union { uint8_t byte1;
                struct { bool X   :1, Y   :1, C   :1, Z   :1,
                              L1  :1, R1  :1, L2  :1, R2  :1;
            }; };
            union { uint8_t byte2;
                struct { bool home:1, func:1, res0:1, res1:1,
                              usr0:1, usr1:1, usr2:1, usr3:1;
            }; };
            union { uint8_t byte3;
                struct { bool u1  :1, d1  :1, l1  :1, r1  :1, 
                              u2  :1, d2  :1, l2  :1, r2  :1;
            }; };
        };
    };
} eight_input_button_t;

// ボタンビット化構造体
typedef struct {
    union { uint16_t all;
        struct {
            union { uint8_t byte0;
                struct { bool b00  :1,b01  :1,b02  :1,b03  :1,
                              b04  :1,b05  :1,b06  :1,b07  :1;
            }; };
            union { uint8_t byte1;
                struct { bool b08  :1,b09  :1,b10  :1,b11  :1,
                              b12  :1,b13  :1,b14  :1,b15  :1;
            }; };
        };
    };
} eight_input_button_16_t;
/*
  後で :
    eight_input_button_8_t;
    eight_input_button_32_t;
*/

// ボタン配列情報
typedef struct {
    int  A     , B     , sele  , strt  ,
         u0    , d0    , l0    , r0    ;
    int  X     , Y     , C     , Z     ,
         L1    , R1    , L2    , R2    ;
    int  home  , func  , res0  , res1  ,
         usr0  , usr1  , usr2  , usr3  ;
    int  u1    , d1    , l1    , r1    ,
         u2    , d2    , l2    , r2    ;
} eight_input_button_sort_t;

// ボタン情報(拡張)
typedef struct {
    int  x4,     x8,     x12,    x20,    
         x4p,    x8p,    x12p,   x20p,
         ab2,    ab4,    ab6,    
         ab4p,   ab6p,   
         s_x4p,  s_x8p,  s_x12p, s_ab4p, s_ab6p;
} eight_input_button_ex_t;

// ボタン情報(拡張2 : 文字関連)
typedef struct {
    unsigned char chr;  // これは、'0' が無入力扱い (他は "-1")
} eight_input_button_ex2_t;

// メイン構造体
typedef struct {
    // ボタン情報(基本)
    eight_input_button_t      hold;     // 押している
    eight_input_button_t      push;     // 押したとき
    eight_input_button_t      rlse;     // 離したとき
    eight_input_button_t      oldd;     // 押している(前回)
    // ボタン配列情報
    eight_input_button_sort_t sort;
    // 元ボタン入力アドレス
    eight_input_button_16_t*  button_16;
    // ボタン情報(拡張)
    eight_input_button_ex_t   ex;
    eight_input_button_ex2_t  ex2;
} eight_input_t;

int eight_input_init(eight_input_t* clematis, eight_input_button_16_t* btn,
    int A, int B, int sele, int strt, int u0, int d0, int l0, int r0, 
    int X, int Y, int C, int Z, int L1, int R1, int L2, int R2);
// int eight_input_finish(eight_input_t* clematis);
int eight_input_update(eight_input_t* clematis);

// void eight_input_regatod(eight_input_t* clematis);

//---------------------------------------------------------------------------------
#ifdef __cplusplus
    }      // extern "C"
#endif
//---------------------------------------------------------------------------------
#endif // _eight_input_h_
//---------------------------------------------------------------------------------

/*

  十字キー系列
    ■ x4
    後で

    ■ x8
    後で

    ■ x12
    後で

    ■ x20
    後で

  AB系列
    ■ ab2
    後で

    ■ ab4
    後で

    ■ ab6
    A B の押続け方によって、6通りの出力を行います。
    ・最初に押されたボタンにより、[012] [345] のグループに分かれます。
    ・最初に押されたボタンは、全てのボタンを離すまで有効です。

          | ＢＡ    ＢＡ
        0 : ○● → ○●
        1 : ○● → ●●
        2 : ○● → ●○

        3 : ●○ → ●○
        4 : ●○ → ●●
        5 : ●○ → ○●

    ■ ab4p
    ab4 に似ていますが、こちらは [押した瞬間] です。
*/