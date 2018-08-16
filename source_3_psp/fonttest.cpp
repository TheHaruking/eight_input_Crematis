#include <stdio.h>
#include <wchar.h>
#include <string>
#include <pspsdk.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <oslib/intraFont/intraFont.h>

#include "common/callbacks.h"
#include "common/gu.h"
#include "eight_input.h"

using std::string;
using std::wstring;

PSP_MODULE_INFO("intraFontTest", PSP_MODULE_USER, 1, 1);

#define PGF_NUM 2
const char pgfs[PGF_NUM][32] = {
	"flash0:/font/ltn8.pgf",
	"flash0:/font/jpn0.pgf"
};
/*
0x01 : ｻﾝｾﾘﾌ | ｾﾘﾌ
0x02 : 標準  | ｲﾀﾘｯｸ
0x04 : 標準  | 太字
0x08 : 大    | 小
*/

class textData {
public:
    const static int MAX_STR = 128;
    wstring str;
    uint16_t str16[MAX_STR];
    textData() {
        str = L"";
    }

    textData(const wchar_t* c) {
        str = L"";
        str += c;
    }

    void addStr(wchar_t c) {
        str += c;
    }

    const wchar_t* data() {
        return str.data();
    }

    // wchar_t : 4バイト
    // そのままでは intraFontPrintUCS2 で 表示不可。
    // よって 2バイト へ 変換する
    const unsigned short* data_ucs2() {
        int i;
        str16[0] = 0;
        for (i = 0; i < MAX_STR && str[i]; i++) {
            str16[i] = (uint16_t)str[i];
        }
        str16[i] = 0;
        return (const unsigned short*)str16;
    }
};

// ★★★ main ★★★
int main() {
	SceCtrlData pad;
	callbacksSetup();

	// Colors
	enum colors {
		RED  =0xFF0000FF, GREEN=0xFF00FF00, BLUE =0xFFFF0000,
		GRAY =0xFF7F7F7F, LGRAY=0xFFBFBFBF, DGRAY=0xFF3F3F3F,		
		WHITE=0xFFFFFFFF, BLACK=0xFF000000,
	};

    // Init intraFont library
    intraFontInit();

    // Load fonts
	float size = 1.0f;

    intraFont* fonts[PGF_NUM];
	for (int i = 0; i < PGF_NUM; i++){
        fonts[i] = intraFontLoad(pgfs[i], INTRAFONT_STRING_UTF8);
		intraFontSetStyle(fonts[i], 1.0f, WHITE, DGRAY, INTRAFONT_WIDTH_FIX);
    }
    // 英字 : 60字 480px, 日本語 : 60字 540px
    // 16行 : 8x16(margin 1) 
    intraFontSetStyle(fonts[1], (480.0f / 540.0f), WHITE, DGRAY, INTRAFONT_WIDTH_FIX);
	intraFontSetAltFont(fonts[0], fonts[1]); // ltn8 <= jpn0
	intraFont* font = fonts[0];

	// Gu初期化
	guInit();
	// ボタン初期化
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    // くれまちす 初期化
    // 入力情報更新に必要なアドレスを最初に教えてやる必要がある。
	// 0.PSP_CTRL_SELECT    = 0x00000001,
    // 1.-
    // 2.-
	// 3.PSP_CTRL_START     = 0x00000008,
	// 4.PSP_CTRL_UP        = 0x00000010,
	// 5.PSP_CTRL_RIGHT     = 0x00000020,
	// 6.PSP_CTRL_DOWN      = 0x00000040,
	// 7.PSP_CTRL_LEFT      = 0x00000080,
	// 8.PSP_CTRL_LTRIGGER  = 0x00000100,
	// 9.PSP_CTRL_RTRIGGER  = 0x00000200,
    //10.-
    //11.-
	//12.PSP_CTRL_TRIANGLE  = 0x00001000,
	//13.PSP_CTRL_CIRCLE    = 0x00002000,
	//14.PSP_CTRL_CROSS     = 0x00004000,
	//15.PSP_CTRL_SQUARE    = 0x00008000,
	//16.PSP_CTRL_HOME      = 0x00010000,
    eight_input_t clematis = { 0 };
    eight_input_init( &clematis, 
            (eight_input_button_16_t*)&pad.Buttons, 
              13, 14,  0,  3,  4,  6,  7,  5, 
              12, 15, -1, -1,  8,  9, -1, -1);
	// 物:テキスト と テスト変数
    const int MAX_ROW = 17;
    textData text[MAX_ROW];
    uint32_t y = 0;

	// ★★ main loop ★★
	while(callbacksRunning()) {
        // ループ時 初期化処理
		guUpdate(GRAY);
		sceCtrlReadBufferPositive(&pad, 1);
        eight_input_update(&clematis);
        
        // 英語 : 横69文字
        if (clematis.btn.push.d0) {
            y++; y %= 17;
        }
        if (clematis.btn.push.u0) {
            y--; y %= 17;
        }
		if (clematis.btn.push.A) {
			text[y].addStr(L'A');
		}
        if (clematis.btn.push.B) {
            text[y].addStr(L'あ');
        }
        if (clematis.btn.push.Y) {
            wstring* str = &text[y].str;
            if (!str->empty()) {
                str->erase(str->begin() + str->length() - 1);
            }
        }
        // Draw various text
        float y = -2;
        for (int i = 0; i < MAX_ROW; i++) {
            intraFontPrintUCS2(font, 0, y+=16, text[i].data_ucs2());
        }
        // End drawing
		guFinish();
	}
	
	// Unload our fonts
    for (int i = 0; i < PGF_NUM; i++) {
        intraFontUnload(fonts[i]);
    }

	// Shutdown font library
	intraFontShutdown();
	callbacksExit();
	return 0;
}
