#include "../vgfuzz.hpp"
#include <iostream>
#include <cassert>
#include <fstream>

using namespace vgin;
using namespace vgijon;

int main(void) {
    std::cout << "vg, input:" << std::endl;
    // std::string input;
    // std::cin >> input; // a 0.4 0.5 b 0.6 0.7 c 0.8 0.9
    
    // FILE *fp = fopen("ccc", "r");
    

    const char keys[] = {'a', 'b', 'c'};

    // テスト時は下をコメントアウト
    // std::ifstream ifs("in/ccc");
    // std::cin.rdbuf(ifs.rdbuf());
    // テスト時は上をコメントアウト

    createVGIn(keys);
    // createVGIn(keys);
    // __afl_coverage_off();
    VGIJON_SET(3);
    // IJON_SET(1);
    // IJON_SKIP_THIS_TEST();

    for(int i=0; i<100; i++){
        cout << "current:" << 0.1*i << "  input:" << vgc_getch() << endl;
        if(updateVGIn(0.1))
        {break;}
        else
        {VGIJON_SET(i);}
    }

    // if(vg_ispressed('a')) cout << "a is pressed" << endl;

    return 0;
}