#ifndef VGFUZZ_MACRO_H
#define VGFUZZ_MACRO_H

extern int vgfuzz_isTestcaseEnd(void);
extern int vgfuzz_isAllTestEnd(void);

#define __VGFUZZ_TESTCASE_END vgfuzz_isTestcaseEnd()
#define __VGFUZZ_ALL_TEST_END vgfuzz_isAllTestEnd() 

#endif // VGFUZZ_MACROS_H
