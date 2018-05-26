#include "eight_input.h"

// データ
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

// ヘルパー関数
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
        &clematis->sort, 
        A, B, sele, strt, u0, d0, l0, r0, 
        X, Y, C, Z, L1, R1, L2, R2);
    clematis->button_16 = button_16;
	return 0;
}

int eight_input_finish(eight_input_t* clematis)
{
	return 0;
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

int get_x8p(int x12, int x8p, int* s_x8p)
{
    int first;
    switch (*s_x8p) {
    case 0:
        
        break;
    case 1:
        if (x12 <= -1)
            return -1
        else
            return -2
    }
    return -1;
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

int get_ab4p(uint32_t hold, uint32_t push, uint32_t rlse, int* s_ab4p)
{
    hold &= 0x00000003;
    push &= 0x00000003;
    rlse &= 0x00000003;

    bool is_double_pressed = (hold == 3);
    bool is_all_rereased   = (hold == 0);
    int use = (*s_ab4p) ? push : // 1
                              rlse ; // 0
    switch (*s_ab4p) {
        case 0:
            if (is_double_pressed) {
                *s_ab4p = 1;
                use = push;     // rlse -> push へ更新
            }
            break;
        case 1:
            if (is_all_rereased) {
                *s_ab4p = 0;
            }
            break;
    }
    return tbl_ab4p[*s_ab4p][use];
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

char get_chr(int x8, int ab4p, int ab6, int x4p )
{
    // 配列の回転
    int  x8_rot     = (x8   >= 0) ? tbl_x8_rot90[x8]    : -1;
    int  ab4p_rot   = (ab4p >= 0) ? tbl_ab4_rot90[ab4p] : -1;
    int  ab6_adcbfe = (ab6  >= 0) ? tbl_ab6_adcbfe[ab6] : -1;
    int  x4p_udlr   = (x4p  >= 0) ? tbl_x4_udlr[x4p]    : -1;

    // 条件
    bool is_alp     = (x8  >= 0) && (ab4p >=  0) && (x4p  <= -1);
    bool is_no_alp  = (ab6 >= 0) && (ab4p <= -1) && (x4p  >=  0);
    bool is_sym     = (2 <= ab6_adcbfe) && (ab6_adcbfe <= 5);
    bool is_esc     = (0 <= ab6_adcbfe) && (ab6_adcbfe <= 1);
    
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

int eight_input_update(eight_input_t* clematis)
{
    // oldd に 代入
    clematis->oldd.all = clematis->hold.all;

    // hold にビットを代入(ソート設定に沿わせる)
    clematis->hold.A    = get_bit(clematis->button_16, clematis->sort.A);
    clematis->hold.B    = get_bit(clematis->button_16, clematis->sort.B);
    clematis->hold.sele = get_bit(clematis->button_16, clematis->sort.sele);
    clematis->hold.strt = get_bit(clematis->button_16, clematis->sort.strt);
    clematis->hold.u0   = get_bit(clematis->button_16, clematis->sort.u0);
    clematis->hold.d0   = get_bit(clematis->button_16, clematis->sort.d0);
    clematis->hold.l0   = get_bit(clematis->button_16, clematis->sort.l0);
    clematis->hold.r0   = get_bit(clematis->button_16, clematis->sort.r0);
    clematis->hold.X    = get_bit(clematis->button_16, clematis->sort.X);
    clematis->hold.Y    = get_bit(clematis->button_16, clematis->sort.Y);
    clematis->hold.C    = get_bit(clematis->button_16, clematis->sort.C);
    clematis->hold.Z    = get_bit(clematis->button_16, clematis->sort.Z);
    clematis->hold.L1   = get_bit(clematis->button_16, clematis->sort.L1);
    clematis->hold.R1   = get_bit(clematis->button_16, clematis->sort.R1);
    clematis->hold.L2   = get_bit(clematis->button_16, clematis->sort.L2);
    clematis->hold.R2   = get_bit(clematis->button_16, clematis->sort.R2);

    // push と rlse
    uint8_t diff = clematis->hold.all ^ clematis->oldd.all;
    clematis->push.all = diff & clematis->hold.all;
    clematis->rlse.all = diff & clematis->oldd.all;

    // 拡張ボタン情報 : ex
    clematis->ex.x4   = get_x4(clematis->hold.all, clematis->ex.x4);
    clematis->ex.x8   = get_x8(clematis->hold.all);
    clematis->ex.x12  = get_x12(clematis->ex.x4, clematis->ex.x8);
    clematis->ex.x20  = get_x20(clematis->ex.x4, clematis->ex.x8);
    clematis->ex.x4p  = get_x4p(clematis->ex.x4, clematis->ex.x4p);
    clematis->ex.x8p  = get_x8p(clematis->ex.x4, clematis->ex.x8, &clematis->ex.s_x8p);

    clematis->ex.ab2  = get_ab2(clematis->hold.all, clematis->ex.ab2);
    clematis->ex.ab4  = get_ab4(clematis->ex.ab2, clematis->hold.all);
    clematis->ex.ab6  = get_ab6(clematis->ex.ab2, clematis->hold.all);
    clematis->ex.ab4p = get_ab4p(clematis->hold.all, clematis->push.all, clematis->rlse.all, &clematis->ex.s_ab4p);
	
    clematis->ex2.chr = get_chr(clematis->ex.x8, clematis->ex.ab4p, clematis->ex.ab6, clematis->ex.x4p);

    return 0;
}