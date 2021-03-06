#include <stdio.h>
#include <wchar.h>
#include <string>
#include <list>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdebug.h>

#include "common/callbacks.h"
#include "common/gu.h"
#include "eight_input.h"
#include "intraFont/intraFont.h"

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

namespace mylib {
// using
using std::wstring;
using std::list;
typedef list<wstring> wstrings;

class textData {
// 定数・変数
private:
	// 定数
	const static int MAX_STR = 128; 	// intraFontPrint : 256バイト制限 ｱﾘ
	const static int MAX_ROW = 4096;	// 4096行 (ひとまず)
	const static int MAX_BUF = 4096;	// 4096文字 (ひとまず)

	// 変数
	int _current_row;	// 選択中の行
	int _max_row;		// 全行数
	uint16_t out[MAX_STR];
public:
	enum current_row_func {
		ROW_FUNC_INC, ROW_FUNC_DEC	
	};
	wstring texts[MAX_ROW];
	wstrings test;

// 関数
private:
	void init() {
		_current_row	= 0;
		_max_row		= 0;
	}
public:
	textData() {
		init();
	}

	// 直読み込み(行ごとに分割)
	textData(const wchar_t str[]) {
		wchar_t str1[MAX_BUF];
		int i = 0;
		wchar_t* buf;
		wchar_t* row;

		init();
		// wcstok は 元を破壊する。コピーしておく
		wcscpy(str1, str);
		row = wcstok(str1, L"\n", &buf);
		while (row) {
			texts[i] += row; i++; // 追加していく
			row = wcstok(NULL, L"\n", &buf);
		}
	}

	// ファイル読込
	textData(const uint16_t* data, int size) {
		if (data[0] != 0xFEFF) return; // BOM確認
		data++; // BOM を飛ばす
		size /= 2; // uint8_t数 → uint16_t数

		wchar_t str1[MAX_BUF];
		int i = 0;
		wchar_t* buf;
		wchar_t* row;

		init();
		// wcstok は 元を破壊する。コピーしておく
		// ※ 32bit(wchar_t) に 変換していく 
		for (i = 0; i < size; i++) {
			str1[i] = (wchar_t)(data[i]);
		}
		row = wcstok(str1, L"\n", &buf);
		for (i = 0; row != NULL; i++) {
			wstring& s = texts[i];
			s += row; // 文字列 追加していく
			// 残った \r 消しておく
			if (s[s.length() - 1] == L'\r')
				s.erase(s.begin() + s.length() - 1); 
			row = wcstok(NULL, L"\n", &buf);
		}
		_max_row = i - 1;
	}

	// テキスト操作
	// 末尾挿入
	void push_back(wchar_t c) {
		texts[_current_row].push_back(c);
	}

	// 末尾削除
	wchar_t pop_back() {
		wstring& sss = texts[_current_row];
		if (sss.empty())
			return L'\0';
		int p = sss.length() - 1;
		wchar_t c = sss[p]; 
		sss.erase(p); 
		return c;
	}

	// wchar_t : 4バイト
	// そのままでは intraFontPrintUCS2 で 表示不可。
	// そのため 2バイト へ 変換
	const unsigned short* data_ucs2(int row) {
		int i;
		for (i = 0; i < MAX_STR && texts[row][i]; i++) {
			out[i] = (uint16_t)texts[row][i];
		}
		out[i] = L'\0';
		return (const unsigned short*)out;
	}

	// Get, Set
	int  max_row()    { return _max_row; }
	int  current_row(){ return _current_row; }
	void current_row(int n){
		if (n < 0)				{ n = 0; }
		if (n > _max_row - 1)	{ n = _max_row - 1; }
		_current_row = n;
	}
	void current_row(current_row_func func) {
		switch (func) {
			case ROW_FUNC_INC: current_row(_current_row + 1); break;
			case ROW_FUNC_DEC: current_row(_current_row - 1); break;
		}
	}
}; }

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
	eight_input_t clematis = { 0 };
	eight_input_init( &clematis, 
			(eight_input_button_16_t*)&pad.Buttons, 
			PSP_CTRL_CIRCLE,   PSP_CTRL_CROSS,    PSP_CTRL_SELECT, PSP_CTRL_START,
			PSP_CTRL_UP,       PSP_CTRL_DOWN,     PSP_CTRL_LEFT,   PSP_CTRL_RIGHT, 
			PSP_CTRL_TRIANGLE, PSP_CTRL_SQUARE,   -1,              -1, 
			PSP_CTRL_LTRIGGER, PSP_CTRL_RTRIGGER, -1,              -1 );

	// textファイル サイズ取得
	// const char file[] = "★テスト.txt";
	const char file[] = "test.txt";
	SceIoStat stat;
	sceIoGetstat(file, &stat);

	// textファイル Open
	int fdout = sceIoOpen( file, PSP_O_RDONLY | PSP_O_CREAT, 0777 );
	uint8_t* data = new uint8_t[stat.st_size];
	sceIoRead(fdout, data, stat.st_size);
	// sceIoWrite(fdout, data, 1);
 
	// ファイル 閉じる
	sceIoClose(fdout);

	// 物:テキスト と テスト変数
	mylib::textData text = mylib::textData(
//		L"test です。\n"
//		L"test aaa\n"
//		L"やっほー！！ ★★"
		(uint16_t*)data, stat.st_size
	);
	delete data;

	// ★★ main loop ★★
	while(callbacksRunning()) {
		// ループ時 初期化処理
		guUpdate(GRAY);

		sceCtrlReadBufferPositive(&pad, 1);
		eight_input_update(&clematis);
		// 参照しておく
		eight_input_button_base_t& push = clematis.btn.push;

		// ↑↓ : 縦選択
		if (push.d0) { text.current_row(text.ROW_FUNC_INC); }
		if (push.u0) { text.current_row(text.ROW_FUNC_DEC); }

		// ○×△ 等
		if (push.l0)    { text.push_back(L'←'); }
		if (push.r0)    { text.push_back(L'→'); }
		if (push.L1)    { text.push_back(L'L' ); }
		if (push.R1)    { text.push_back(L'R' ); }
		if (push.A)     { text.push_back(L'○'); }
		if (push.B)     { text.push_back(L'×' ); }
		if (push.Y)     { text.pop_back(); }
		if (push.X)     { text.push_back(L'△'); }
		if (push.sele)  { text.push_back(L'☂'); }
		// if (push.strt)  { text.push_back(L'☀'); }
		if (push.strt)  {
			for (auto i : { L'☀', L'☂', L'☁', L'㋐' } )
				text.push_back(i);
		}



		// Draw various text
		float y = 16.0f;
		for (int i = 0; i < text.max_row() - 1; i++) {
			intraFontPrintUCS2(font, 0, y, text.data_ucs2(i));
			y += 16;
		}
		
		{
			char16_t* ucs2str = u"|";
			intraFontPrintUCS2(font, 0, 16 * text.current_row() + 14, (uint16_t*)ucs2str); // u ... UTF16 を 示す
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
