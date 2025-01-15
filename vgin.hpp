#ifndef VGIN_HH
#define VGIN_HH

#include <iostream>
#include <memory>
#include <vector>

using namespace std;

constexpr unsigned long DEFAULT_MAX_INPUT_COUNT = 1024;
constexpr double DEFAULT_MAX_TESTCASE_TIME = 60;

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
    unsigned char last_input;
    double last_input_started_time;
    double delta_time;
    double current_time;
    double max_testcase_seconds;
    vector<InUnit> input_data;
};

namespace vgin{
    inline VGInData data;
    inline KeyTable key_table;
}

inline void debugVGIn(){
    cout << "current_time:" << vgin::data.current_time << endl;
    cout << "last_input:" << vgin::data.last_input << endl;
    cout << "last_input_started_time:" << vgin::data.last_input_started_time << endl;
    cout << "delta_time:" << vgin::data.delta_time << endl;
    cout << "max_testcase_seconds:" << vgin::data.max_testcase_seconds << endl;
    cout << "input_data:" << endl;
    for(auto& unit : vgin::data.input_data){
        cout << "  input:" << unit.input << "  starttime_from_pre_inunit:" << unit.starttime_from_pre_inunit << "  duration:" << unit.duration << endl;
    }
}

inline bool createVGIn(const char* keys, double max_testcase_seconds = DEFAULT_MAX_TESTCASE_TIME){
    try{
        /* == 入力キー初期化 == */
        for(int i=0; i<sizeof(keys); i++){
            KeyUnit unit = KeyUnit();
            unit.input = keys[i];
            unit.remaining_time = 0;
            vgin::key_table.keys.push_back(unit);
        }

        /* == 入力ストリーム初期化 == */
        vgin::data.last_input_started_time = 0;
        vgin::data.current_time = 0;
        vgin::data.delta_time = 0;
        vgin::data.input_data = vector<InUnit>();
        vgin::data.last_input = 0;

        for(int i=0; i<DEFAULT_MAX_INPUT_COUNT; i++){
            InUnit unit = InUnit();
            if(cin.eof()) return false;
            cin >> unit.input;
            if(cin.eof()) return false;
            cin >> unit.starttime_from_pre_inunit;
            if(cin.eof()) return false;
            cin >> unit.duration;
            vgin::data.input_data.push_back(unit);
            if(cin.eof()) break;
        }

        return true;
    }
    catch (const std::bad_alloc& e) {
        // std::cerr << "Memory allocation failed: " << e.what() << std::endl; // ファザー向けとして、ここでのエラーは無視する
        return false;
    }

    return false;
}

inline bool updateVGIn(double delta_time){ // false: 入力未終了, true: 入力終了
    static VGInData data;
    static KeyTable key_table;
    data.delta_time = delta_time;
    data.current_time += delta_time;

    if(data.max_testcase_seconds < data.current_time) return true; // テストケース時間超過

    // キーテーブル処理
    for(auto& key : key_table.keys){
        if(key.remaining_time > 0){
            key.remaining_time -= delta_time;
        }
        if(key.remaining_time < 0){
            key.remaining_time = 0;
        }
    }

    // 入力データを順次処理
    auto it = data.input_data.begin();
    while (it != data.input_data.end()) {
        InUnit& unit = *it;
        if(data.current_time >= unit.starttime_from_pre_inunit + data.last_input_started_time){
            data.last_input_started_time += unit.starttime_from_pre_inunit;
            if(data.current_time < unit.starttime_from_pre_inunit + data.last_input_started_time + unit.duration)
            {
                // input
                for(auto& key : key_table.keys){
                    if(key.input == unit.input){
                        key.remaining_time += unit.duration;
                        data.last_input = unit.input;
                        break;
                    }
                }
            }
            it = data.input_data.erase(it);
        }else{
            return false; // 次のupdateまで待機
        }
    }
    
    return true;
}

inline char vg_getch(){
    static VGInData data;
    return data.last_input;
}

inline char vg_ispressed(char key){
    static KeyTable key_table;
    for(auto& k : key_table.keys){
        if(k.input == key){
            return k.remaining_time > 0;
        }
    }
    return false;
}

extern "C" {
    int createVGcIn(const char* keys, double max_testcase_seconds = DEFAULT_MAX_TESTCASE_TIME){
        bool result = createVGIn(keys, max_testcase_seconds);
        if(result) return 0;
        else return -1;
    }
    int updateVGcIn(double delta_time){
        bool result = updateVGIn(delta_time);
        if(result) return 0;
        else return -1;
    }
    char vgc_getch(){
        return vg_getch();
    }
    char vgc_ispressed(char key){
        return vg_ispressed(key);
    }
}

#endif // VGIN_HH