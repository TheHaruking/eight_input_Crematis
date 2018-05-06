#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "gba.h"
#include "eight_input.h"

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
    
	consoleDemoInit();
    char str[256] = "Test text.";
    char* p = str + strlen(str);
    printf("\033[0;0H");
    printf("%s %d", str, strlen(str));
	while(1)
	{	
		scanKeys();
		btn = keysHeld();
		eight_input_update(&clematis);

	    if (clematis.ex2.chr >= 1) {
            *p = clematis.ex2.chr;
            if (*p == '\b') {
                p--;
            } else {
                p++;
            }
            *p = '\0';
            printf("\033[0;0H");
            printf("%s", str);
		}
	}
}