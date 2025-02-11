#ifndef VGIN_HH
#define VGIN_HH

#include <iostream>
#include <memory>
#include <vector>


using namespace std;

namespace {
constexpr unsigned long DEFAULT_MAX_INPUT_COUNT = 1024;
constexpr long double DEFAULT_MAX_TESTCASE_TIME = 60;
constexpr double TIME_EPSILON = 1e-9;


extern "C" {
    int __afl_coverage_interesting(int x, uint32_t addr);
    void __afl_coverage_on();
    void __afl_coverage_off();
}
}

// extern "C" {
//     int __afl_coverage_interesting(int x, uint32_t addr);
//     void __afl_coverage_on();
//     void __afl_coverage_off();
// }

// __AFL_COVERAGE();

namespace vgijon{
    #define VGIJON_MODE 1
    #if VGIJON_MODE == 1
    #define VGIJON_SET(x) __afl_coverage_interesting(1, x)
    #define COV_OFF() __afl_coverage_off()
    #define COV_ON() __afl_coverage_on()
    #else
    #define VGIJON_SET(x)
    #define COV_OFF()
    #define COV_ON()
    #endif
}

// IJON_LIMITED_COVERAGE();
// IJON_DISABLE();

namespace vgin{

class KeyUnit{
public:
    unsigned char input; // 0番目に入力名, 1番目に入力の強弱
    double remaining_time;
};

class KeyTable{
public:
    vector<KeyUnit> keys;
};

class InUnit{
public:
    unsigned char input; // 0番目に入力名, 1番目に入力の強弱
    double starttime_from_pre_inunit;
    double duration;
};

class VGInData{
public:
    vector<char> inputtings;
    unsigned char last_input;
    double last_input_started_time;
    double delta_time;
    double current_time;
    long double max_testcase_seconds;
    vector<InUnit> input_data;
};

inline VGInData& data(){
    static VGInData data;
    return data;
}

inline KeyTable& key_table(){
    static KeyTable key_table;
    return key_table;
}

inline void debugVGIn(){
    VGInData& d = data();
    cout << "current_time:" << d.current_time << endl;
    cout << "last_input:" << d.last_input << endl;
    cout << "last_input_started_time:" << d.last_input_started_time << endl;
    cout << "delta_time:" << d.delta_time << endl;
    cout << "max_testcase_seconds:" << d.max_testcase_seconds << endl;
    cout << "input_data:" << endl;
    for(auto& unit : d.input_data){
        cout << "  input:" << unit.input << "  starttime_from_pre_inunit:" << unit.starttime_from_pre_inunit << "  duration:" << unit.duration << endl;
    }

    // if(d.inputtings.size() > 0){
    //     cout << "inputtings:";
    //     for(auto& input : d.inputtings){
    //         cout << " " << input;
    //     }
    //     cout << endl;
    // }
}

inline bool updateVGIn(double delta_time){ // false: 入力未終了, true: 入力終了
    COV_OFF();
    VGInData& d = data();
    KeyTable& t = key_table();
    d.delta_time = delta_time;
    d.current_time += delta_time;

    if(d.max_testcase_seconds < d.current_time) {COV_ON(); return true;} // テストケース時間超過

    // 入力データを順次処理
    bool is_input_stream_finished = true;
    auto it = d.input_data.begin();
    while (it != d.input_data.end()) {
        InUnit& unit = *it;
        if(d.current_time >= unit.starttime_from_pre_inunit + d.last_input_started_time){
            d.last_input_started_time += unit.starttime_from_pre_inunit;
            if(d.current_time < unit.starttime_from_pre_inunit + d.last_input_started_time + unit.duration)
            {
                // input
                for(auto& key : t.keys){
                    if(key.input == unit.input){
                        key.remaining_time += unit.duration;
                        d.last_input = unit.input;
                        d.inputtings.push_back(unit.input);
                        break;
                    }
                }
            }
            it = d.input_data.erase(it);
        }
        else
        {
            is_input_stream_finished = false;
            break;
        }
    }
    
    // キーテーブル処理
    bool is_all_key_released = true;
    for(auto& key : t.keys){
        if(key.remaining_time > TIME_EPSILON){
            key.remaining_time -= delta_time;
            is_all_key_released = false;
        }
        else
        {
            key.remaining_time = 0;
            if(d.last_input == key.input){
                d.last_input = 0;
            }
        }
    }

    if(is_all_key_released) d.last_input = 0;
    else if(d.last_input == 0) { // 何かのキーが押されている場合、登録されているかつ押されているキーの中で最初のキーをlast_inputに設定
        for(auto& key : t.keys){
            if(key.remaining_time > TIME_EPSILON){
                d.last_input = key.input;
                break;
            }
        }
    }

    COV_ON();
    return is_input_stream_finished && is_all_key_released;
}

inline bool createVGIn(const char* keys, long double max_testcase_seconds = DEFAULT_MAX_TESTCASE_TIME){
    // __AFL_COVERAGE_OFF();
    COV_OFF();
    try{
        VGInData& d = data();
        KeyTable& t = key_table();
        /* == 入力キー初期化 == */
        for(int i=0; i<int(sizeof(keys)); i++){
            KeyUnit unit = KeyUnit();
            unit.input = keys[i];
            unit.remaining_time = 0;
            t.keys.push_back(unit);
        }

        /* == 入力ストリーム初期化 == */
        d.last_input_started_time = 0;
        d.current_time = 0;
        d.delta_time = 0;
        d.input_data = vector<InUnit>();
        d.last_input = 0;
        d.max_testcase_seconds = max_testcase_seconds;

        for(long unsigned int i=0; i<DEFAULT_MAX_INPUT_COUNT; i++){
            InUnit unit = InUnit();
            if(cin.eof()) return false;
            cin >> unit.input;
            if(cin.eof()) return false;
            cin >> unit.starttime_from_pre_inunit;
            if(cin.eof()) return false;
            cin >> unit.duration;
            d.input_data.push_back(unit);
            if(cin.eof()) break;
        }

        COV_ON();
        // __AFL_COVERAGE_ON();
        return !updateVGIn(0); // 0秒時点での入力処理
    }
    catch (const std::bad_alloc& e) {
        // std::cerr << "Memory allocation failed: " << e.what() << std::endl; // ファザー向けとして、ここでのエラーは無視する
        // __AFL_COVERAGE_ON();
        COV_ON();
        return false;
    }
    // __AFL_COVERAGE_ON();
    COV_ON();
    return false;
}

inline char vg_getch(){
    return data().last_input;
}

inline bool vg_ispressed(char key){
    COV_OFF();
    KeyTable& t = key_table();
    for(auto& k : t.keys){
        if(k.input == key){
            COV_ON();
            return k.remaining_time > 0;
        }
    }
    COV_ON();
    return false;
}

extern "C" {

    int createVgcIn(const char* keys, double max_testcase_seconds = DEFAULT_MAX_TESTCASE_TIME){
        // IJON_DISABLE();
        bool result = createVGIn(keys, max_testcase_seconds);
        // IJON_ENABLE();
        if(result) return 0;
        else return -1;
    }
    int updateVgcIn(double delta_time){
        bool result = updateVGIn(delta_time);
        if(result) return 0;
        else return -1;
    }
    char vgc_getch(){
        return vg_getch();
    }
    int vgc_ispressed(char key){
        if(vg_ispressed(key)) return 1;
        else return 0;
    }
}
}

// IJON_ENABLE();

#endif // VGIN_HH