#pragma comment(lib,"XInput.lib")
#pragma comment(lib,"user32.lib")
#include <windows.h>
#include <stdio.h>
#include <XInput.h>
#include <string>
#include <locale.h>
#include "eight_input.h"

/*
typedef struct _XINPUT_STATE {    
	DWORD dwPacketNumber;    
	XINPUT_GAMEPAD Gamepad;
} XINPUT_STATE, *PXINPUT_STATE;

typedef struct _XINPUT_GAMEPAD {    
	WORD  wButtons;    
	BYTE  bLeftTrigger;    
	BYTE  bRightTrigger;    
	SHORT sThumbLX;    
	SHORT sThumbLY;    
	SHORT sThumbRX;    
	SHORT sThumbRY;
} XINPUT_GAMEPAD, *PXINPUT_GAMEPAD;

XINPUT_GAMEPAD_DPAD_UP	    0x0001
XINPUT_GAMEPAD_DPAD_DOWN	0x0002
XINPUT_GAMEPAD_DPAD_LEFT	0x0004
XINPUT_GAMEPAD_DPAD_RIGHT	0x0008
XINPUT_GAMEPAD_START    	0x0010
XINPUT_GAMEPAD_BACK	        0x0020
XINPUT_GAMEPAD_LEFT_THUMB	0x0040
XINPUT_GAMEPAD_RIGHT_THUMB	0x0080
XINPUT_GAMEPAD_LEFT_SHOULDER	0x0100
XINPUT_GAMEPAD_RIGHT_SHOULDER	0x0200
XINPUT_GAMEPAD_A	        0x1000
XINPUT_GAMEPAD_B	        0x2000
XINPUT_GAMEPAD_X	        0x4000
XINPUT_GAMEPAD_Y	        0x8000
  参考 : https://msdn.microsoft.com/ja-jp/library/bb173045(v=vs.85).aspx
*/

using namespace std;

void convert_pshort_to_double(double pd[], short* ps[], int n){
    for(int i = 0; i < n; i++) {
        pd[i] = (double)*(ps[i]);
    }
}

void move_mouse_pos(int x, int y){
    POINT now;
    GetCursorPos(&now);
    now.x += x;
    now.y += y;
    SetCursorPos(now.x, now.y);
    return;
}

int main()
{
	setlocale( LC_ALL, "Japanese");
    // Xinput 初期化
	XINPUT_STATE state = { 0 };
    double double_arr[4];
    short* pshort_arr[4] = {
        &state.Gamepad.sThumbLX, &state.Gamepad.sThumbLY, 
        &state.Gamepad.sThumbRX, &state.Gamepad.sThumbRY, 
    };

    // くれまちす 初期化
    // 入力情報更新に必要なアドレスを最初に教えてやる必要がある。
    eight_input_t clematis = { 0 };
    eight_input_init( &clematis, 
            (eight_input_button_16_t*)&state.Gamepad.wButtons, 
            13, 12, 5, 4, 0, 1, 2, 3, 
            15, 14, 6, 7, 8, 9, -1, -1);
    eight_input_set_analog(&clematis, 
            0, &double_arr[0], &double_arr[1]);
    eight_input_set_analog(&clematis, 
            1, &double_arr[2], &double_arr[3]);
	
	// エラーチェック
	DWORD dwResult = XInputGetState(0, &state);
	if (dwResult != ERROR_SUCCESS ) {
		printf("error!\n");
		return dwResult;
	}
	
    // テスト用
    wstring str(256, L'\0');
    str = L"dir";
    SetCursorPos(0, 0);

	// 接続を確認
	wprintf(L"yes!\n");
    wprintf(L"> %s", str.data());
	
    bool loop = true;
    wchar_t buf[256]; 

	while (loop) {
		XInputGetState(0, &state);
        convert_pshort_to_double(double_arr, pshort_arr, 4);
        eight_input_update(&clematis);
        
        // ボタン配列確認
        if (1) {          
		    // printf("state.Gamepad.wButtons : %04X\n", state.Gamepad.wButtons);
            // printf("clematis.hold.all      : %04X\n", clematis.hold.all);
            // printf("clematis.st0.hold.x    : %04X\n", clematis.st0.hold.x);
            // printf("st0.push.x, st0.rlse.x : %04X, %04X\n", (WORD)clematis.st0.push.x, (WORD)clematis.st0.rlse.x);
            // if (clematis.ex.x8p >= 0) {
            //     printf("x8p : %04X\n", clematis.ex.x8p);
            // }
            if (clematis.ex.x8p >= 0) {
                wsprintf(buf, L"x8p : %04X\n", clematis.ex.x8p);
                // OutputDebugString(buf);
            }
            wsprintf(buf, L"x:%lf, y:%lf, m:%lf\n", clematis.st0.hold.x, clematis.st0.hold.y, clematis.st0ex.mag);
            // OutputDebugString(buf);
        }

        // 文字入力・コマンド確認
        if (clematis.btn.push.l1) {
            str.push_back(L'あ');
            wprintf(L"\r> %s", str.data());    // '\r' : 再びその行から書く
        }
        if (clematis.ex2.chr >= 1) {
            switch (clematis.ex2.chr) {
                default:
                    str.push_back(clematis.ex2.chr);
                    break;
                case '\b':
                    if (!str.empty()) {
                        str.pop_back();
                        wprintf(L"\r> %s ", str.data()); // 末尾文字の表示を消す
                    }
                    break;
                case '\n':
                    if (str == L"q") {
                        loop = false;
                        break;
                    }
                    _wsystem(str.data());
                    break;
            }
            
            // 文字表示
            wprintf(L"\r> %s", str.data());    // '\r' : 再びその行から書く
        }

        // カーソル移動
        if (clematis.st0ex.is_dead == false) {
            move_mouse_pos((int)clematis.st0.hold.x / 1000.0, (int)-clematis.st0.hold.y / 1000.0);
        }

		Sleep(15);
	}
	return 0;
}