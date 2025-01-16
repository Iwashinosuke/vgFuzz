#include "vgin.hpp"
#include <iostream>
#include <cassert>
#include <fstream>

using namespace vgin;

int main() {
    std::cout << "vg, input:" << std::endl;
    // std::string input;
    // std::cin >> input; // a 0.4 0.5 b 0.6 0.7 c 0.8 0.9
    
    // FILE *fp = fopen("ccc", "r");
    

    std::ifstream ifs("ccc");
    std::cin.rdbuf(ifs.rdbuf());

    const char keys[] = {'a', 'b', 'c'};
    assert(createVGIn(keys) == true);

    for(int i=0; i<100; i++){
        cout << "current:" << 0.1*i << "  input:" << vg_getch() << endl;
        if(updateVGIn(0.1)) break;
    }
    return 0;
}