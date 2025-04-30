/*
ハーネス上での使用を想定
*/
#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>
#include <sstream>

namespace vgfuzz{
    namespace vgin{
        enum class VGIN_LIFE_CYCLE{
            STARTING,
            TESTING,
            ENDING,
            END
        };

        class VGInBuf{
            // 必要要素: 入力名、入力開始時刻、入力時間
            public:
                VGInBuf()
                {
                    _process_time = nullptr;
                    _prev_time = 0;
                    _is_updated = false;
                }

                /*
                データ初期化
                */
                void init(double *process_time);

                /*
                入力値更新
                */
                void update(void);

                /*
                AFL++から入力バフを受け取り(取り扱い注意)
                返り値: 読み込みできたかどうか
                

                想定する入力のデータ構造
                
                unsigned char *fuzz_bufの構成:
                {size_t 入力数}{unsigned char 入力名のリスト}{double 入力開始時刻のリスト}{double入力時間のリスト}

                */
                bool set_buf(unsigned char* fuzz_buf, size_t fuzz_len);

                /*
                直で入力バフを受け取り
                */
                bool set_buf(std::vector<unsigned char>& keys, std::vector<double>& starttimes, std::vector<double>& durations);

                /*
                入力が完了したか
                返り値: 完了したかどうか
                */
                bool is_que_complete(void)
                { return _keys.empty(); } 

                /*
                該当フレームでの入力を取得
                返り値: 入力値 取得失敗なら空のvector                
                */
                std::vector<unsigned char> get_processings();

            private:
                void reset_iterators()
                {
                    _keys_iter = _keys.begin();
                    _starttime_iter = _starttimes.begin();
                    _duration_iter = _durations.begin();
                }

                bool clear_datas()
                {
                    _keys.clear();
                    _starttimes.clear();
                    _durations.clear();
                    _processing_keys_life.clear();
                    _processing_keys.clear();
                    reset_iterators();
                    return true;
                }

                bool check_process_time()
                {
                    if(_process_time == nullptr)
                    {
                        std::cerr << "VGInBuf check_process_time error: process_time is null" << std::endl;
                        return false;
                    }
                    return true;
                }

                VGIN_LIFE_CYCLE _life_cycle;
                std::vector<unsigned char> _keys;
                std::vector<unsigned char>::iterator _keys_iter;
                std::vector<double> _starttimes;
                std::vector<double>::iterator _starttime_iter;
                std::vector<double> _durations;
                std::vector<double>::iterator _duration_iter;
                bool _is_updated;
                std::unordered_map<unsigned char, double> _processing_keys_life;
                std::vector<unsigned char> _processing_keys;
                double *_process_time;
                double _next_key_time_stamp;
                double _prev_time;
        };     
    }
}