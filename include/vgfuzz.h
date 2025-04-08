#ifdef __cplusplus
extern "C" {
#endif

#include "vgfuzz_macro.h"
// #include "vgfuzz_predefines.hpp"

extern int vgfuzz_init(void);
extern int vgfuzz_createVGIn(const char* keys, double max_testcase_seconds, int is_informed);
extern int vgfuzz_updateVGIn(double delta_time);
extern char vgfuzz_getch(void);
extern int vgfuzz_ispressed(char key);
extern void vgfuzz_informAFLData(unsigned char* data, size_t data_len);
extern unsigned char* vgfuzz_getchDirectly(void);


#ifdef __cplusplus
}
#endif