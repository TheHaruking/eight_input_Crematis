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

/*
class wstring2 : public wstring {
private:
	uint16_t _data_ucs2[1024];
public:
	// 既定初期化(wchar_t*)
	wstring2(const wchar_t* _) : wstring(_) {}

	// 文字列を utf-16 で 返す
	const uint16_t* data_ucs2() {
		const wstring2& _this = *this; // this : wstring2 の ポインタ. 参照化
		// if (sizeof(wchar_t) == 2)
			return (const uint16_t*)_this.data();

		int i = 0;
		for (auto x : _this) {
			_data_ucs2[i++] = (unsigned short)x;
		}
		_data_ucs2[i] = L'\0';
		return _data_ucs2;
	}
}; */
}

void data_ucs2(uint16_t* buf, const wchar_t* _){
	for(int i = 0; _[i] != L'\0'; i++)
		buf[i] = (uint16_t)(_[i]);
}

// intraFont 用 便利ラッパークラス
class intraConsole {
private:
	// intraFont 関連
	intraFont* font;
	intraFont* font_jpn;
	const char* pgf     = "flash0:/font/ltn8.pgf";
	const char* pgf_jpn = "flash0:/font/jpn0.pgf";

	// Console
	std::wstring	text;
	uint16_t		text_ucs2[512];
	bool			_update;
	float			x, y;
	int				column, row;

	// 定数
	const float		Y_NUM = 16.0;
	const int		Y_MAX = 17;
	const int		TEXT_MAX = 256;

	// Colors
	enum colors {
		RED  =0xFF0000FF, GREEN=0xFF00FF00, BLUE =0xFFFF0000,
		GRAY =0xFF7F7F7F, LGRAY=0xFFBFBFBF, DGRAY=0xFF1E1E1E, // VSCode の背景		
		WHITE=0xFFFFFFFF, BLACK=0xFF000000,
	};

public:
	// 初期化
	intraConsole(){
		// Gu初期化
		guInit();
		guUpdate(DGRAY);
		
		// Init intraFont library
		// Load fonts
		intraFontInit();
		// 英字 : 60字 480px, 日本語 : 60字 540px
		font     = intraFontLoad(pgf    , INTRAFONT_STRING_UTF8 | INTRAFONT_CACHE_ALL );
		font_jpn = intraFontLoad(pgf_jpn, INTRAFONT_STRING_UTF8 | INTRAFONT_CACHE_ALL );
		intraFontSetStyle(font    , 1.0f, WHITE, 0, INTRAFONT_WIDTH_FIX);
		intraFontSetStyle(font_jpn, (480.0f / 540.0f), WHITE, 0, INTRAFONT_WIDTH_FIX);
		intraFontSetAltFont(font, font_jpn); // ltn8 <= jpn0

		// console
		x = 0.0f;		y = 14.0f;
		column = 0;		row = 0;
		text = L"";
	}

	// 終了
	~intraConsole() {
		// Unload our fonts
		intraFontUnload(font);
		intraFontUnload(font_jpn);

		// Shutdown font library
		intraFontShutdown();
	}

	void update(bool force) { _update = force; update(); }
	void update(){
		if(!_update)
			return;
		print();
		guFinish();
		guUpdate(DGRAY);
		_update = false;
	}

	void push_back(wchar_t _){
		if (text.size() >= TEXT_MAX)
			return;
		text.push_back(_);
		_update = true;
	}

	void pop_back(){
		if (text.empty())
			return;
		text.pop_back();
		_update = true;
	}

	// 全角/半角 判定 (かなり大雑把)
	int  zenhan(wchar_t c) {
		
		if ((0x00 <= c) && (c < 0xff))
			return 1;
		else
			return 2;  
	}
	// print(utf-16)
	void print(){
		int i = 0;
		column = 0;
		// \0 は含まれない
		for (auto x : text) {
			column += zenhan(x);
			// 改行
			if (column >= 60) {
				text_ucs2[i++] = (uint16_t)L'\n';
				column = 0;		row++;
			}

			text_ucs2[i++] = (uint16_t)x;

		}
		text_ucs2[i] = L'\0';
		intraFontPrintUCS2Ex(font, x, y, text_ucs2, i);
	}

	// printf
	template <typename... Args>
	void printf(const char* _, Args... args) {
		intraFontPrintf(font, 0, y, _, args...);
		y += Y_NUM;
	}
};

// ★★★ main ★★★
int main() {
	SceCtrlData pad;
	callbacksSetup();
	
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

	// testtext
	intraConsole console;
	console.update(true);
	
	// ★★ main loop ★★
	while( callbacksRunning() ) {
		// ループ時 初期化処理
		// guUpdate(DGRAY);

		// 入力関連
		sceCtrlReadBufferPositive(&pad, 1);
		eight_input_update(&clematis);
		// 参照しておく
		eight_input_button_base_t& push = clematis.btn.push;
		// ○×△ 等
		if (push.A)     { console.push_back(L'A'); }
		if (push.B)     { console.push_back(L'あ' ); }
		if (push.Y)     { console.pop_back(); }
		if (push.sele)  { console.push_back(L'☂'); }
		// if (push.strt)  { console.push_back(L'☀'); }
		if (push.strt)  {
			for (auto i : { L'a', L'A', L'あ', L'㋐' } )
				console.push_back(i);
		}

		// End drawing
		console.update();
	}

	callbacksExit();
	return 0;
}
