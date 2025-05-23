#include <iostream>
#include <memory>
#include <vector>
#include <sstream>

#include "../include/vgfuzz.h"
#include "../include/vgfuzz_predefines.hpp"

using namespace std;


namespace {
constexpr unsigned long DEFAULT_MAX_INPUT_COUNT = 1024;
constexpr long double DEFAULT_MAX_TESTCASE_TIME = 60;
constexpr double TIME_EPSILON = 1e-9;
static unsigned char* testcase = nullptr;
static size_t testcase_len = 0;
static unsigned char* current_input = nullptr;
static size_t current_input_len = 0;
}


extern "C" {
    int __afl_coverage_interesting(int x, uint32_t addr);
    void __afl_coverage_on();
    void __afl_coverage_off();
    
}

namespace vgfuzz {
    class vgFuzzMaster{
        
    };

    namespace manager{
        enum class UsingModule{
            Ijon,
            Vgin
        };

        void setTestcaseEndMode(VGFUZZ_TESTCASE_END_MODE mode)
        {
            switch (mode)
            {
            case VGFUZZ_TESTCASE_END_MODE::VGFUZZ_TEM_FINISH_INPUT:
                break;
            case VGFUZZ_TESTCASE_END_MODE::VGFUZZ_TEM_PASSED_TIME:
                break;
            case VGFUZZ_TESTCASE_END_MODE::VGFUZZ_TEM_CUSTOM:
                break;
            }
        }


        bool isTestcaseEnd(){
            if(current_input_len >= testcase_len) return true;
            else
            return false;
        }

        bool isAllTestEnd(){
            return false;
        }

        void informStringTestcaseData(unsigned char* data, size_t data_len){
            testcase = data;
            testcase_len = data_len;
            current_input = testcase;
            current_input_len = 0;
        }

        unsigned char* getchTestcase(){
            if(current_input_len < testcase_len){
                current_input_len++;
                return current_input++;
            }
            else{
                return nullptr;
            }
        }
    }

    namespace ijon{
        #ifdef VGIJON
        #define VGIJON_SET(x) __afl_coverage_interesting(1, x)
        #define COV_OFF() __afl_coverage_off()
        #define COV_ON() __afl_coverage_on()
        #else
        #define VGIJON_SET(x)
        #define COV_OFF()
        #define COV_ON()
        #endif
    }
    
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
    
    VGInData& data(){
        static VGInData data;
        return data;
    }
    
    KeyTable& keyTable(){
        static KeyTable key_table;
        return key_table;
    }

    
    void debugVgin(){
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
    
    

    bool updateVgin(double delta_time){ // false: 入力未終了, true: 入力終了
        COV_OFF();
        VGInData& d = data();
        KeyTable& t = keyTable();
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
    
    bool createVgin(const char* keys, long double max_testcase_seconds = DEFAULT_MAX_TESTCASE_TIME){
        // __AFL_COVERAGE_OFF();
        // COV_OFF();
        try{
            VGInData& d = data();
            KeyTable& t = keyTable();
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
    
            // COV_ON();
            return !updateVgin(0); // 0秒時点での入力処理
        }
        catch (const std::bad_alloc& e) {
            // COV_ON();
            // return false;
        }
        // COV_ON();
        return false;
    }
    
    // bool createVgin(const char* keys, double max_testcase_seconds = DEFAULT_MAX_TESTCASE_TIME,unsigned char* buf, size_t buf_size)
    // {
    //     VGInData& d = data();
    //     KeyTable& t = keyTable();
    //     /* == 入力キー初期化 == */
    //     for(int i=0; i<int(sizeof(keys)); i++){
    //         KeyUnit unit = KeyUnit();
    //         unit.input = keys[i];
    //         unit.remaining_time = 0;
    //         t.keys.push_back(unit);
    //     }

    //     /* == 入力ストリーム初期化 == */
    //     d.last_input_started_time = 0;
    //     d.current_time = 0;
    //     d.delta_time = 0;
    //     d.input_data = vector<InUnit>();
    //     d.last_input = 0;
    //     d.max_testcase_seconds = max_testcase_seconds;

    //     std::istringstream iss(reinterpret_cast<char*>(buf), std::ios::binary);
    //     std::streambuf* cinbuf = cin.rdbuf();
    //     cin.rdbuf(iss.rdbuf());
    //     for(size_t i=0; i<buf_size; i++){
    //         InUnit unit = InUnit();
    //         if(cin.eof()) return false;
    //         cin >> unit.input;
    //         if(cin.eof()) return false;
    //         cin >> unit.starttime_from_pre_inunit;
    //         if(cin.eof()) return false;
    //         cin >> unit.duration;
    //         d.input_data.push_back(unit);
    //         if(cin.eof()) break;
    //     }
    //     cin.rdbuf(cinbuf);

    //     // COV_ON();
    //     return !updateVgin(0); // 0秒時点での入力処理
    // }        

    char getch(){
        return data().last_input;
    }
    
    bool isPressed(char key){
        // COV_OFF();
        KeyTable& t = keyTable();
        for(auto& k : t.keys){
            if(k.input == key){
                COV_ON();
                return k.remaining_time > 0;
            }
        }
        // COV_ON();
        return false;
    }
    
    // namespace heatmap
    // {
    //     class 
    // }

}

extern "C"{
    bool vgfuzz_isTestcaseEnd(){
        return vgfuzz::manager::isTestcaseEnd();
    }
    
    bool vgfuzz_isAllTestEnd(){
        return vgfuzz::manager::isAllTestEnd();
    }
    
    // VGFUZZ_RETURN vgfuzz_createVgin(const char* keys, double max_testcase_seconds = DEFAULT_MAX_TESTCASE_TIME, unsigned char* data, size_t data_len = 0){
    //     if(data != nullptr && data_len > 0){
    //         cin.rd
    //     }
    //     bool result = vgfuzz::vgin::createVgin(keys, max_testcase_seconds);
    //     if(result) return VGFUZZ_SUCCESS;
    //     else return VGFUZZ_FAILURE;
    // }
    
    VGFUZZ_RETURN vgfuzz_updateVgin(double delta_time){
        bool result = vgfuzz::vgin::updateVgin(delta_time);
        if(result) return VGFUZZ_SUCCESS;
        else return VGFUZZ_CONTINUE;
    }
    
    char vgfuzz_getch(){
        return vgfuzz::vgin::getch();
    }
    
    int vgfuzz_isPressed(char key){
        if(vgfuzz::vgin::isPressed(key)) return 1;
        else return 0;
    }

    /* vgfuzzモジュールにAFLからのデータを入力、dataポインタはテストの初回のみ渡してください */
    void vgfuzz_informAFLData(unsigned char* data, size_t data_len)
    {
        vgfuzz::manager::informStringTestcaseData(data, data_len);
    }

    /* バニラAFL++の入力 */
    unsigned char* vgfuzz_getchDirectly(void){
        unsigned char* key = vgfuzz::manager::getchTestcase();
        if(key == nullptr) return 0;
        else return key;
    }
}
}
