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
	#define MAX_X	256
	#define MAX_ROW	4096
    #define MAX_STR 128
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
	int i;
	float size = 1.0f;

    intraFont* fonts[PGF_NUM];
	for (i = 0; i < PGF_NUM; i++){
        fonts[i] = intraFontLoad(pgfs[i], INTRAFONT_STRING_UTF8);
		intraFontSetStyle(fonts[i], 1.0f, WHITE, DGRAY, INTRAFONT_WIDTH_FIX);
    }
    intraFontSetStyle(fonts[1], 0.89f, WHITE, DGRAY, INTRAFONT_WIDTH_FIX);
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
              13, 14,  0,  3,  4,  5,  6,  7, 
              12, 15, -1, -1,  8,  9, -1, -1);
	// 物:テキスト と テスト変数
    textData text (L"");
    textData text2(L"AA BB CC DD aa bb cc dd -*=+ 0123 456 789 456 789 456 789 456 789 456 789 456 789");
    textData text3(L"日本語記号☀後☁です日本語記号日本語記号日本語記号日本語記号日本語記号日本語記号日本語記号");
    textData text4(L"ﾊﾝｶｸｶﾀｶﾅﾊﾝｶｸｶﾀｶﾅﾊﾝｶｸｶﾀｶﾅﾊﾝｶｸｶﾀｶﾅﾊﾝｶｸｶﾀｶﾅﾊﾝｶｸｶﾀｶﾅﾊﾝｶｸｶﾀｶﾅﾊﾝｶｸｶﾀｶﾅﾊﾝｶｸｶﾀｶﾅﾊﾝｶｸｶﾀｶﾅﾊﾝｶｸｶﾀｶﾅ");
    textData text5(L"int main(int argc, char* args[]) {");
    textData text6(L"    printf(\"hello world\"); // コメントです。");
    textData text7(L"}");
	int count = 0;

	// main loop
	while(callbacksRunning()) {
		guUpdate(GRAY);
		sceCtrlReadBufferPositive(&pad, 1);
        eight_input_update(&clematis);
        
		size = 1.0f * (pad.Ly / 128.0);
		// size = 0.203125f;
        // 英語 : 横69文字
		if (clematis.btn.push.A){
			// str += "亜";
            wchar_t c[2] = L"";
            swprintf(c, sizeof c, L"%d", count % 10);
			text.addStr(c[0]);
			count++;
		}
        if (clematis.btn.push.B) {
            text.addStr(L'あ');
            count++;
        }
        // Draw various text
        float y = 20;
        intraFontPrintUCS2(font, 0, y+=17, text.data_ucs2());
        intraFontPrintUCS2(font, 0, y+=17, text2.data_ucs2());
		intraFontPrintUCS2(font, 0, y+=17, text3.data_ucs2());
		intraFontPrintUCS2(font, 0, y+=17, text4.data_ucs2());
		intraFontPrintUCS2(font, 0, y+=17, text5.data_ucs2());
		intraFontPrintUCS2(font, 0, y+=17, text6.data_ucs2());
		intraFontPrintUCS2(font, 0, y+=17, text7.data_ucs2());
        // End drawing
		guFinish();
	}
	
	// Unload our fonts
    for (i = 0; i < PGF_NUM; i++) {
        intraFontUnload(fonts[i]);
    }

	// Shutdown font library
	intraFontShutdown();
	callbacksExit();
	return 0;
}
