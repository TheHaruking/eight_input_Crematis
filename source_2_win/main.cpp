#pragma comment(lib,"XInput.lib")
#pragma comment(lib,"user32.lib")
#include <windows.h>
#include <stdio.h>
#include <XInput.h>
#include <string>
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

int main()
{
    // Xinput 初期化
	XINPUT_STATE state = { 0 };

    // くれまちす 初期化
    eight_input_t clematis = { 0 };
    eight_input_init( &clematis, 
            (eight_input_button_16_t*)&state.Gamepad.wButtons, 
            13, 12, 5, 4, 0, 1, 2, 3, 
            15, 14, 6, 7, 8, 9, -1, -1);
    eight_input_set_analog(&clematis, 
            &state.Gamepad.sThumbLX, &state.Gamepad.sThumbLY,
            &state.Gamepad.sThumbRX, &state.Gamepad.sThumbRX);
	
	// エラーチェック
	DWORD dwResult = XInputGetState(0, &state);
	if (dwResult != ERROR_SUCCESS ) {
		printf("error!\n");
		return dwResult;
	}
	
    // テスト用
    string str(256, '\0');
    str = "dir";
    SetCursorPos(0, 0);

	// 接続を確認
	printf("yes!\n");
    printf("> %s", str.data());
	
    bool loop = true;
	while (loop) {
		XInputGetState(0, &state);
        eight_input_update(&clematis);
        
        // ボタン配列確認
        if (1) {
		    // printf("state.Gamepad.wButtons : %04X\n", state.Gamepad.wButtons);
            // printf("clematis.hold.all      : %04X\n", clematis.hold.all);
            // printf("clematis.st1.hold.x     : %04X\n", clematis.st1.hold.x);
            printf("st1.push.x, st1.rlse.x : %04X, %04X\n", (WORD)clematis.st1.push.x, (WORD)clematis.st1.rlse.x);
        }

        // 文字入力・コマンド確認
        if (clematis.ex2.chr >= 1) {
            switch (clematis.ex2.chr) {
                default:
                    str.push_back(clematis.ex2.chr);
                    break;
                case '\b':
                    if (!str.empty()) {
                        str.pop_back();
                        printf("\r> %s ", str.data()); // 末尾文字の表示を消す
                    }
                    break;
                case '\n':
                    if (str == "q") {
                        loop = false;
                        break;
                    }
                    system(str.data());
                    break;
            }
            
            // 文字表示
            printf("\r> %s", str.data());    // '\r' : 再びその行から書く
        }

		Sleep(10);
	}
	return 0;
}