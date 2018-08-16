#include <string>
#include <pspsdk.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <oslib/intraFont/intraFont.h>

#include "common/callbacks.h"
#include "common/gu.h"
#include "eight_input.h"

using std::string;

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
	#define MAX_X	256
	#define MAX_ROW	4096
	char row[MAX_ROW][MAX_X];
	int  select_row;
	int  x;

	// 関数
	textData() {
		select_row = 0;
		x = 0;
	}

	char* select_data(){
		return row[select_row];
	}

	char* select_data(int select){
		return row[select];
	}

	int add_data(char* c){
		int len = strlen(c);
		
		row[select_row][x] = 
	}
};




// ★★★ main ★★★
int main() {
	SceCtrlData pad;
	callbacksSetup();
	pspDebugScreenPrintf("main!");

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
	intraFontSetAltFont(fonts[0], fonts[1]); // ltn8 <= jpn0
	intraFont* font = fonts[0];

	// Gu初期化
	guInit();
	// ボタン初期化
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	// 文字列初期化
	string str;
	char test[4096] = "";
	// テスト変数
	int count = 0;

	// main loop
	while(callbacksRunning()) {
		guUpdate(GRAY);
		sceCtrlReadBufferPositive(&pad, 1); 
		size = 1.0f * (pad.Ly / 128.0);
		// size = 0.203125f;
		if (pad.Buttons & PSP_CTRL_SQUARE){
			// str += "亜";
			char buf[64] = "";
			sprintf(buf, "%d", count % 10);
			sprintf(test, "%s%s", test, buf);
			count++;
		}
        // Draw various text
        float y = 20;
		y += 17;
        intraFontPrint(font, 0, y, "font : にほﾝｶｸｶﾀｶんご★◽☂☀!!!");
		y += 17;
		//wchar_t temp[64] = L"test";
		unsigned short temp[64] = { L't', L'e', L's', L'あ', L't',  };
		unsigned short* ucs2_all = (unsigned short*)temp;
//      intraFontPrintUCS2(font, 0, y, (uint16_t*)L"font : にほﾝｶｸｶﾀｶんご★◽☂☀!!!");
        intraFontPrintUCS2(font, 0, y, temp);
        y += 17;
		intraFontPrintf(font, 0, y, "%s", test); // str.data()); 
		y += 17;
		intraFontPrintf(font, 0, y, "size : %f\n", size);
		y += 17;
		intraFontPrintf(font, 0, y, "data : %08X", (uint32_t)str.data());
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
