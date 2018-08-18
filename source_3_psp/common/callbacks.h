#ifndef common_callbacks_h
#define common_callbacks_h

#ifdef __cplusplus
extern "C" {
#endif

int callbacksRunning();
int callbacksSetup(void);
void callbacksExit();

#ifdef __cplusplus
}
#endif

#endif
