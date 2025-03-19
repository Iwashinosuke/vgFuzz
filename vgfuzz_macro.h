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



#endif // VGFUZZ_MACROS_H
