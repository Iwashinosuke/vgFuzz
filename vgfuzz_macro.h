#ifndef VGFUZZ_MACRO_H
#define VGFUZZ_MACRO_H

extern bool vgfuzz_isTestcaseEnd(void);
extern bool vgfuzz_isAllTestEnd(void);


#define __VGFUZZ_TESTCASE_END vgfuzz_isTestcaseEnd()
#define __VGFUZZ_ALL_TEST_END vgfuzz_isAllTestEnd() 

typedef enum VGFUZZ_RETURN{
    VGFUZZ_SUCCESS,
    VGFUZZ_CONTINUE,
    VGFUZZ_FAILURE
} VGFUZZ_RETURN;

typedef enum VGFUZZ_TESTCASE_END_MODE{
    VGFUZZ_TEM_FINISH_INPUT,
    VGFUZZ_TEM_PASSED_TIME,
    VGFUZZ_TEM_CUSTOM
} VGFUZZ_TESTCASE_END_MODE;

#endif // VGFUZZ_MACROS_H
