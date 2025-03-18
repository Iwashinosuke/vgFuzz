#ifdef __cplusplus
extern "C" {
#endif

#include "vgfuzz_macro.h"

extern int vgfuzz_createVGIn(const char* keys, double max_testcase_seconds);
extern int vgfuzz_updateVGIn(double delta_time);
extern char vgfuzz_getch(void);
extern int vgfuzz_ispressed(char key);

#ifdef __cplusplus
}
#endif