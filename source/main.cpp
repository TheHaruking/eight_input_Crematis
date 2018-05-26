#include <stdio.h>
#include <stdint.h>
#include <string>
#include "gba.h"
#include "eight_input.h"

using namespace std;

int main()
{
	uint16_t btn;
	eight_input_t clematis = { 0 };
	eight_input_init(&clematis, 
        (eight_input_button_16_t*)&btn, 
/*
    int  A     , B     , sele  , strt  ,
         u0    , d0    , l0    , r0    ;
    int  X     , Y     , C     , Z     ,
         L1    , R1    , L2    , R2    ;
*/
        0,  1,  2,  3,  6,  7,  5,  4, 
        -1, -1, -1, -1, 9,  8, -1, -1 );
    
    string str(256, '\0');
    str = "This is test text.";
    
	consoleDemoInit();

    printf("\033[0;0H");
    printf("%s", str.data());
	while(1)
	{	
		scanKeys();
		btn = keysHeld();
		eight_input_update(&clematis);
        
        char c = clematis.ex2.chr;
        switch (c) {
            default:
                str.push_back(c);
                printf("%c", c);
                break;
            case '\b':
                if (!str.empty()) {
                    str.pop_back();
                    printf("\r%s ", str.data()); // 末尾文字の表示を消す
                }
                break;
            case '\n':
                // 文字表示
                printf("\n%s\n", str.data());    // '\r' : 再びその行から書く 
                str.clear();
                printf("> ");
                break;
            case '\0':
                break;
        }
	}
}