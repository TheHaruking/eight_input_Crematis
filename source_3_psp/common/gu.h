#ifndef common_gu_h
#define common_gu_h

#ifdef __cplusplus
extern "C" {
#endif

// void guInit(unsigned int* list);
void guInit();
void guFinish();
// void guUpdate(unsigned int* list, unsigned int color);
void guUpdate(unsigned int color);

#ifdef __cplusplus
}
#endif

#endif
