#include "eight_input.h"
#define  _USE_MATH_DEFINES // "M_PI" のため
#include <math.h>

const double DEAD_VAL   = 8000.0;
const double DEAD_VAL_2 = 6000.0;

////////////////////////////////////////////////////////////////
// データ
////////////////////////////////////////////////////////////////
static const int tbl_x4[16] = {
    -1,  0,  2, -1,
     3, -1, -1, -1,
     1, -1, -1, -1,
    -1, -1, -1, -1,
};
static const int tbl_x8[16] = {
    -1,  0,  4, -1,
     6,  7,  5, -1,
     2,  1,  3, -1,
    -1, -1, -1, -1,
};

static const int tbl_x12[4][8] = {
	{  1,  2,  2, -1, -1, -1,  0,  0, },
	{  3,  3,  4,  5,  5, -1, -1, -1, },
	{ -1, -1,  6,  6,  7,  8,  8, -1, },
	{ 11, -1, -1, -1,  9,  9, 10, 11, },
};

static const int tbl_x20[4][8] = {
	{  2,  3,  4,  4, -1,  0,  0,  1, },
	{  5,  6,  7,  8,  9,  9, -1,  5, },
	{ -1, 10, 10, 11, 12, 13, 14, 14, },
	{ 19, 19, -1, 15, 15, 16, 17, 18, },
};

static const int tbl_ab2[4] = {
    -1,  0,  1, -1
};

static const int tbl_ab4[2][4] = {
    { -1,  0, -1,  2 },
    { -1, -1,  1,  3 },
};

static const int tbl_ab6[2][4] = {
    { -1,  0,  2,  1 },
    { -1,  5,  3,  4 },
};

static const int tbl_ab4p[2][4] = {
    { -1,  0,  1, -1 },
    { -1,  3,  2, -1 },
};

static const char tbl_chr_alp[2][8][4] = { {
	{ 'a', 'b', 'c', '1' }, { 'd', 'e', 'f', '2' }, 
	{ 'g', 'h', 'i', '3' }, { 'j', 'k', 'l', '4' }, 
	{ 'm', 'n', 'o', '5' }, { 'p', 'q', 'r', 's' }, 
	{ 't', 'u', 'v', ',' }, { 'w', 'x', 'y', 'z' }, 
} , {
	{ 'A', 'B', 'C', '6' }, { 'D', 'E', 'F', '7' }, 
	{ 'G', 'H', 'I', '8' }, { 'J', 'K', 'L', '9' }, 
	{ 'M', 'N', 'O', '0' }, { 'P', 'Q', 'R', 'S' }, 
	{ 'T', 'U', 'V', '.' }, { 'W', 'X', 'Y', 'Z' }, 
} , };

static const char tbl_chr_sym[2][4][4] = { {
	{ '\'',',', '(', ')' }, { '\"','.', '<', '>' }, 
	{ '+', '-', '|', '_' }, { '!', '?', '@', '`' }, 
} , {
	{ '^', ':', '{', '}' }, { '~', ';', '[', ']' }, 
	{ '*', '=', '/', '\\'}, { '#', '$', '%', '&' }, 
} , };

static const char tbl_chr_esc[2][4] = {
    { '\t', '\n', '\b', ' ' }, { '\0', '\0', '\0', '\0' },
};

// 配列回転用テーブル
// - rot90  : 左に90度回転
// - udlr   : 上下左右
// - aDcbFE : 
static const int tbl_x4_rot90[4] = {    
    0,  1,  2,  3
};
static const int tbl_x8_rot90[8] = {
    2,  3,  4,  5,  6,  7,  0,  1
};
static const int tbl_x4_udlr[4] = {
    0,  3,  1,  2
};
static const int tbl_ab4_rot90[4] = {
    0,  2,  1,  3
};
static const int tbl_ab6_adcbfe[6] = {
    0,  3,  2,  1,  5,  4,
};

// アナログ->デジタル変換用
static const int tbl_ang_x8[16] = {
    4,  5,  5,  6,  6,  7,  7,  0,
    0,  1,  1,  2,  2,  3,  3,  4,
};

// デジタル->ボタン変換用
static const int tbl_x8_btn[] = {
    1,  9,  8, 10,  2,  6,  4,  7,
};

////////////////////////////////////////////////////////////////
// ヘルパー関数
////////////////////////////////////////////////////////////////
void sort_move_16(eight_input_button_sort_t* sort, 
    int A, int B, int sele, int strt, int u0, int d0, int l0, int r0, 
    int X, int Y, int C, int Z, int L1, int R1, int L2, int R2
//    int home, int pict, int res0, int res1, int l1, int r1, int u1, int d1,
//    int usr0, int usr1, int usr2, int usr3, int l2, int r2, int u2, int d2
) {
    sort->A    = A;    sort->B    = B;    sort->sele = sele; sort->strt = strt;
    sort->u0   = u0;   sort->d0   = d0;   sort->l0   = l0;   sort->r0   = r0;  
    sort->X    = X;    sort->Y    = Y;    sort->C    = C;    sort->Z    = Z;   
    sort->L1   = L1;   sort->R1   = R1;   sort->L2   = L2;   sort->R2   = R2;  
}

int eight_input_init(eight_input_t* clematis, eight_input_button_16_t* button_16, 
    int A, int B, int sele, int strt, int u0, int d0, int l0, int r0, 
    int X, int Y, int C, int Z, int L1, int R1, int L2, int R2)
{
    sort_move_16(
        &clematis->btn.sort, 
        A, B, sele, strt, u0, d0, l0, r0, 
        X, Y, C, Z, L1, R1, L2, R2);
    clematis->btn.button_16 = button_16;
	return 0;
}

int eight_input_finish(eight_input_t* clematis)
{
	return 0;
}

void eight_input_set_analog(
    eight_input_t* clematis, int n,
    double* ax, double* ay
){
    eight_input_stick_t*    st;
    eight_input_stick_ex_t* stex;
    switch (n) {
        case 0:
            st   = &clematis->st0;
            stex = &clematis->st0ex;
            break;
        case 1: 
            st   = &clematis->st1;
            stex = &clematis->st1ex;
            break;
    }
        
    if (ax) {
        st->px = ax;
    }
    if (ay) {
        st->py = ay;
    }
    // x y 両方有効で true とする.
    st->enable = (st->px && st->py) ? true : false;
    stex->_dead = DEAD_VAL;
    stex->is_dead = true;
}

bool get_bit(eight_input_button_16_t* button_16, int n){
    switch (n) {
        case  0: return button_16->b00;
        case  1: return button_16->b01;
        case  2: return button_16->b02;
        case  3: return button_16->b03;
        case  4: return button_16->b04;
        case  5: return button_16->b05;
        case  6: return button_16->b06;
        case  7: return button_16->b07;
        case  8: return button_16->b08;
        case  9: return button_16->b09;
        case 10: return button_16->b10;
        case 11: return button_16->b11;
        case 12: return button_16->b12;
        case 13: return button_16->b13;
        case 14: return button_16->b14;
        case 15: return button_16->b15;
        default: return false;
    }
}

int get_x4(uint32_t hold, int x4)
{
    uint32_t hold_udlr = (hold & 0x000000F0) >> 4;
    bool no_press   = (hold_udlr == 0);
    bool is_first   = (x4 == -1);

    // 1.押されていない,  2.初めて押す   3.押されている
    if (no_press) {
        return -1;
    } else if (is_first) {
        return tbl_x4[hold_udlr];
    } else {
        return x4;  // そのまま
    }
}

int get_x8(uint32_t hold)
{
    uint32_t hold_udlr  = (hold & 0x000000F0) >> 4;
    return tbl_x8[hold_udlr];
}

int get_x12(int x4, int x8)
{
    if (x4 >= 0) {
        return tbl_x12[x4][x8];
    } else {
        return -1;
    }
}

int get_x20(int x4, int x8)
{
    if (x4 >= 0) {
        return tbl_x20[x4][x8];
    } else {
        return -1;
    }
}

int get_x4p(int x4, int x4p)
{
    bool no_press = (x4 == -1);
    bool is_first = ((x4p != -2) && (x4 != x4p));
    // 1.押されていない,  2.初めて押す   3.押されている
    if (no_press) {
        return -1;
    } else if (is_first) {
        return x4;
    } else {
        return -2; // 前と同じ : -2
    }
}

void set_x8p(eight_input_state_t* _x8p, const eight_input_button_t* btn)
{
    _x8p->data = -1;
    // テスト中
    switch (_x8p->state) {
    case 0:
        if (btn->hold.udlr0) {
            _x8p->data = btn->hold.udlr0;
            _x8p->state = 1;
        }
        break;
    case 1:
        if (!btn->hold.udlr0) {
            _x8p->state = 0;
        }
        break;
    case 2:
        break;
    }
}

int get_ab2(uint32_t hold, int ab2)
{
    uint32_t hold_ab = (hold & 0x00000003) >> 0;
    bool no_press    = (hold_ab == 0);
    bool is_first    = (ab2 == -1);

    // 1.押されていない,  2.初めて押す   3.押されている
    if (no_press) {
        return -1;
    } else if (is_first) {
        return tbl_ab2[hold_ab];
    } else {
        return ab2;  // そのまま
    }
}
int get_ab4(int ab2, uint32_t hold)
{
    hold &= 0x00000003;
    if (ab2 >= 0) {
        return tbl_ab4[ab2][hold];
    } else {
        return -1;
    }
}

int get_ab6(int ab2, uint32_t hold)
{
    hold &= 0x00000003;
    if (ab2 >= 0) {
        return tbl_ab6[ab2][hold];
    } else {
        return -1;
    }
}

void set_ab4p(eight_input_button_ex_t* ex, const eight_input_button_t* btn)
{
    int hold = btn->hold.AB;
    int push = btn->push.AB;
    int rlse = btn->rlse.AB;

    bool is_double_pressed = (hold == 3);
    bool is_all_rereased   = (hold == 0);
    int  use = (ex->s_ab4p) ?  push : // 1
                               rlse ; // 0
    switch (ex->s_ab4p) {       case 0:
            if (is_double_pressed) {
                ex->s_ab4p = 1;
                use = push;     // rlse -> push へ更新
            }
            break;
        case 1:
            if (is_all_rereased) {
                ex->s_ab4p = 0;
            }
            break;
    }
    ex->ab4p = tbl_ab4p[ex->s_ab4p][use];
}


// 文字取得
char get_chr_alp(int x8, int ab4p)
{
    if ((x8 <= -1) || (ab4p <= -1)) {
        return 0;
    } else {
        return tbl_chr_alp[0][x8][ab4p];
    }
}

char get_chr_sym(int ab6, int x4p) 
{
    if ((ab6 <= -1) || (x4p <= -1)) {
        return 0;
    } else {
        return tbl_chr_sym[0][ab6][x4p];
    }
}

char get_chr_esc(int ab6, int x4p)
{
    if ((ab6 <= -1) || (x4p <= -1)) {
        return 0;
    } else {
        return tbl_chr_esc[ab6][x4p];
    }
}

char get_chr(int x8, int ab4p, int ab6, int x4p, eight_input_cmn_t* cmn)
{
    // 配列の回転
    int  x8_rot     = (x8   >= 0) ? tbl_x8_rot90[x8]    : -1;
    int  ab4p_rot   = (ab4p >= 0) ? tbl_ab4_rot90[ab4p] : -1;
    int  ab6_adcbfe = (ab6  >= 0) ? tbl_ab6_adcbfe[ab6] : -1;
    int  x4p_udlr   = (x4p  >= 0) ? tbl_x4_udlr[x4p]    : -1;
    
    // 条件
    //bool is_alp     = (x8  >= 0) && (ab4p >=  0) && (x4p  <= -1);
    //bool is_no_alp  = (ab6 >= 0) && (ab4p <= -1) && (x4p  >=  0);
    bool is_alp    = ((cmn->all_first & 0xF0) && !(cmn->all_first & ~0xF0));
    bool is_no_alp = ((cmn->all_first & 0x03) && !(cmn->all_first & ~0x03));
    bool is_sym    = (2 <= ab6_adcbfe) && (ab6_adcbfe <= 5);
    bool is_esc    = (0 <= ab6_adcbfe) && (ab6_adcbfe <= 1);
    
    char c = '\0';
    if (is_alp) {
        c = get_chr_alp(x8_rot, ab4p_rot);
    } else if (is_no_alp) {
        if (is_sym) {
            c = get_chr_sym(ab6_adcbfe - 2, x4p_udlr);  // 2 - 5
        } else if (is_esc) {
            c = get_chr_esc(ab6_adcbfe, x4p_udlr);      // 0 - 1
        }
    }
    return c;
}

void set_cmn(eight_input_cmn_t* cmn, const eight_input_button_t* btn) {
    if (!btn->oldd.all && btn->push.all) {
        cmn->all_first = btn->push.all;
    } else
    if (!btn->hold.all) {
        cmn->all_first = 0;
    }
}

void set_analog(eight_input_stick_t* st) {
    // x と y
    for (int i = 0; i < 2; i++) {
        if (st->p[i]) {
            st->oldd.p[i] = st->hold.p[i];
            st->hold.p[i] = *st->p[i];
            st->chng.p[i] = st->hold.p[i] - st->oldd.p[i];
            int sgn = ((st->chng.p[i] >= 0) == (st->hold.p[i] >= 0));
            st->push.p[i] = st->chng.p[i] *  sgn;
            st->rlse.p[i] = st->chng.p[i] * !sgn;
        }
    }
}

void set_analog_ex(eight_input_stick_ex_t* stex, const eight_input_stick_t* st){
    if (st->px && st->py) {
        stex->mag     = sqrt(pow(st->hold.x, 2) + st->hold.y * st->hold.y);
        switch (stex->_is_dead_state) {
        case 0:
            if ((stex->mag) > (DEAD_VAL)) {
                stex->_is_dead_state = 1;
                stex->is_dead = false;
            }
            break;
        case 1:
            if ((stex->mag) < (DEAD_VAL_2)) {
                stex->_is_dead_state = 0;
                stex->is_dead = true;
            }
            break;
        }
        stex->ang     = (stex->is_dead) ? -1:
                        atan2(st->hold.x, st->hold.y) / M_PI;

		int ang2 = (int)((stex->ang + 1.0) * 8.0);
        stex->x8      = (stex->is_dead) ? -1:
						tbl_ang_x8[ang2];
        stex->x8_btn  = (stex->is_dead) ? 0:
                        tbl_x8_btn[stex->x8];
    }
}

int eight_input_update(eight_input_t* clematis)
{
    // oldd に 代入
    clematis->btn.oldd.all = clematis->btn.hold.all;

    // hold にビットを代入(ソート設定に沿わせる)
    clematis->btn.hold.A    = get_bit(clematis->btn.button_16, clematis->btn.sort.A);
    clematis->btn.hold.B    = get_bit(clematis->btn.button_16, clematis->btn.sort.B);
    clematis->btn.hold.sele = get_bit(clematis->btn.button_16, clematis->btn.sort.sele);
    clematis->btn.hold.strt = get_bit(clematis->btn.button_16, clematis->btn.sort.strt);
    clematis->btn.hold.u0   = get_bit(clematis->btn.button_16, clematis->btn.sort.u0);
    clematis->btn.hold.d0   = get_bit(clematis->btn.button_16, clematis->btn.sort.d0);
    clematis->btn.hold.l0   = get_bit(clematis->btn.button_16, clematis->btn.sort.l0);
    clematis->btn.hold.r0   = get_bit(clematis->btn.button_16, clematis->btn.sort.r0);
    clematis->btn.hold.X    = get_bit(clematis->btn.button_16, clematis->btn.sort.X);
    clematis->btn.hold.Y    = get_bit(clematis->btn.button_16, clematis->btn.sort.Y);
    clematis->btn.hold.C    = get_bit(clematis->btn.button_16, clematis->btn.sort.C);
    clematis->btn.hold.Z    = get_bit(clematis->btn.button_16, clematis->btn.sort.Z);
    clematis->btn.hold.L1   = get_bit(clematis->btn.button_16, clematis->btn.sort.L1);
    clematis->btn.hold.R1   = get_bit(clematis->btn.button_16, clematis->btn.sort.R1);
    clematis->btn.hold.L2   = get_bit(clematis->btn.button_16, clematis->btn.sort.L2);
    clematis->btn.hold.R2   = get_bit(clematis->btn.button_16, clematis->btn.sort.R2);
    // analogを追加してみる
    if (clematis->st0.enable) {
        clematis->btn.hold.udlr1 = clematis->st0ex.x8_btn;
    }

    // push と rlse, oldd
    uint32_t diff = clematis->btn.hold.all ^ clematis->btn.oldd.all;
    clematis->btn.push.all = diff & clematis->btn.hold.all;
    clematis->btn.rlse.all = diff & clematis->btn.oldd.all;
    set_cmn(&clematis->cmn, &clematis->btn);

    // 拡張ボタン情報 : ex
    clematis->ex.x4   = get_x4(clematis->btn.hold.all, clematis->ex.x4);
    clematis->ex.x8   = get_x8(clematis->btn.hold.all);
    clematis->ex.x12  = get_x12(clematis->ex.x4, clematis->ex.x8);
    clematis->ex.x20  = get_x20(clematis->ex.x4, clematis->ex.x8);
    clematis->ex.x4p  = get_x4p(clematis->ex.x4, clematis->ex.x4p);
    set_x8p(&clematis->ex._x8p, &clematis->btn);  // 方針変更・・・。随時、このスタイルにしていく。

    clematis->ex.ab2  = get_ab2(clematis->btn.hold.all, clematis->ex.ab2);
    clematis->ex.ab4  = get_ab4(clematis->ex.ab2, clematis->btn.hold.all);
    clematis->ex.ab6  = get_ab6(clematis->ex.ab2, clematis->btn.hold.all);
    set_ab4p(&clematis->ex, &clematis->btn);
	
    clematis->ex2.chr = get_chr(clematis->ex.x8, clematis->ex.ab4p, clematis->ex.ab6, clematis->ex.x4p, &clematis->cmn);

    // アナログ
    set_analog(&clematis->st0);
    set_analog_ex(&clematis->st0ex, &clematis->st0);
    return 0;
}