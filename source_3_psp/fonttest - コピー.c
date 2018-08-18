#include <pspkernel.h>
#include <stdio.h>
#include <string.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspdisplay.h>

#include <oslib/intraFont/intraFont.h>

PSP_MODULE_INFO("intraFontTest", PSP_MODULE_USER, 1, 1);
static int running = 1;
static unsigned int __attribute__((aligned(16))) list[0x40000];

// コールバック関数
/*
	SetupCallbacks
		sceKernelCreateThread
			CallbackThread
				sceKernelCreateCallback
					exit_callback
				sceKernelRegisterExitCallback
				sceKernelSleepThreadCB
		sceKernelStartThread
			exit_callback
*/
int exit_callback(int arg1, int arg2, void *common) {
	running = 0;
	return 0;
}

int CallbackThread(SceSize args, void *argp) {
	int cbid = sceKernelCreateCallback(
		"Exit Callback", exit_callback, NULL
	);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();
	return 0;
}

int SetupCallbacks(void) {
	int thid = sceKernelCreateThread(
		"CallbackThread", CallbackThread, 
		0x11, 0xFA0, PSP_THREAD_ATTR_USER, 0
	);
	if (thid >= 0) {
		sceKernelStartThread(
			thid, 0, 0);
	}
	return thid;
}

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

#define PGF_NUM 17
const char pgfs[PGF_NUM][32] = {
	"flash0:/font/ltn0.pgf",
	"flash0:/font/ltn1.pgf",
	"flash0:/font/ltn2.pgf",
	"flash0:/font/ltn3.pgf",
	"flash0:/font/ltn4.pgf",
	"flash0:/font/ltn5.pgf",
	"flash0:/font/ltn6.pgf",
	"flash0:/font/ltn7.pgf",
	"flash0:/font/ltn8.pgf",
	"flash0:/font/ltn9.pgf",
	"flash0:/font/ltn10.pgf",
	"flash0:/font/ltn11.pgf",
	"flash0:/font/ltn12.pgf",
	"flash0:/font/ltn13.pgf",
	"flash0:/font/ltn14.pgf",
	"flash0:/font/ltn15.pgf",
	"flash0:/font/jpn0.pgf",
};

void GuInit(unsigned int* list) {
	// Init GU    
    sceGuInit();
	sceGuStart(GU_DIRECT, list);

	sceGuDrawBuffer(GU_PSM_8888, (void*)0, BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, (void*)0x88000, BUF_WIDTH);
	sceGuDepthBuffer((void*)0x110000, BUF_WIDTH);
 
	sceGuOffset(2048 - (SCR_WIDTH/2), 2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048, 2048, SCR_WIDTH, SCR_HEIGHT);
	sceGuDepthRange(65535, 0);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuEnable(GU_BLEND);
	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
	sceGuFinish();
	sceGuSync(0,0);
 
	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);
}

void GuFinish() {
	// End drawing
	sceGuFinish();
	sceGuSync(0,0);
	
	// Swap buffers (waiting for vsync)
	sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
}

void GuUpdate(unsigned int* list, unsigned int color) {
	sceGuStart(GU_DIRECT, list);

	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumPerspective( 75.0f, 16.0f/9.0f, 0.5f, 1000.0f);

	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();

	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();
	
	sceGuClearColor(color);
	sceGuClearDepth(0);
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
}

// 
int main() {
	SetupCallbacks();

	// Colors
	enum colors {
		RED		=	0xFF0000FF,
		GREEN	=	0xFF00FF00,
		BLUE	=	0xFFFF0000,
		WHITE	=	0xFFFFFFFF,
		LGRAY	=	0xFFBFBFBF,
		GRAY	= 	0xFF7F7F7F,
		DGRAY	=	0xFF3F3F3F,		
		BLACK	=	0xFF000000,
	};

    // Init intraFont library
    intraFontInit();

    // Load fonts
	int i;
    intraFont* fonts[PGF_NUM]; //latin fonts (large/small, with/without serif, regular/italic/bold/italic&bold)
	for (i = 0; i < PGF_NUM; i++){
        fonts[i] = intraFontLoad(pgfs[i], INTRAFONT_STRING_UTF8); //<- this is where the actual loading happens 
		intraFontSetStyle(fonts[i], 1.0f, WHITE, DGRAY, 0);
    }
	intraFontSetAltFont(fonts[8], fonts[16]); // Set alternative fonts that are used in case a certain char does not exist in the main font

	// Gu初期化
	GuInit(list);

	// main loop
	while(running) 	{
		GuUpdate(list, GRAY);

        // Draw various text
        float x, y = 20;
        intraFontSetStyle(fonts[4], 1.0f, WHITE,DGRAY,0);
		        
        y += 17;
		intraFontPrint(fonts[8], 10,  y, "LTN (UTF8):");          //(has no effect on std ascii)
		intraFontPrint(fonts[4], 110, y, "test aaiiああいい日日日日日日日日日日日日日日日日日日日日日日日日uu!!!"); //UTF-8 encoded chars with accents on top of them

		y += 17;
        intraFontPrint(fonts[16], 10, y, "JPN (UTF8):");
        x = intraFontPrint(fonts[16], 110, y, "やっほーーー|дﾟ)AABB X aabcい日日日日日日日日日日日日日日日日日日日日");            //print UTF-8 encoded string
		if (x == 110) {
			intraFontPrint(fonts[8], 110, y, "[n/a]");
		}
        
        // End drawing
		GuFinish();
	}
	
	// Unload our fonts
    for (i = 0; i < PGF_NUM; i++) {
        intraFontUnload(fonts[i]);
    }

	// Shutdown font library
	intraFontShutdown();
	sceKernelExitGame();
	return 0;
}
