#include "../include/vgin.hpp"
#include "../include/vgfuzz.h"
#include "../include/vgfuzz_predefines.hpp"

namespace vgfuzz{
namespace vgin{
    void VGInBuf::init(double *process_time)
    {
        if(process_time == nullptr)
        {
            std::cerr << "VGInBuf init error: process_time is null" << std::endl;
            return;
        }
        VGInBuf::clear_datas();
        _process_time = process_time;
        _prev_time = 0;
    }
    
    void VGInBuf::update(void)
    {
        if(!check_process_time()) return;
        
        if(*_process_time > _prev_time){
            while(_starttime_iter != _starttimes.end())
            {
                if(*_starttime_iter <= *_process_time)
                {
                    if(*_starttime_iter + *_duration_iter > *_process_time)
                    {
                        _processing_keys_life[*_keys_iter] = *_duration_iter - *_process_time + *_starttime_iter;
                    }
                    _starttime_iter++;
                    _duration_iter++;
                    _keys_iter++;
                }
                else
                {
                    break;
                }
            }
        }

        _is_updated = true;
    }

    bool VGInBuf::set_buf(std::vector<unsigned char>& keys, std::vector<double>& starttimes, std::vector<double>& durations)
    {
        _keys = keys;
        _starttimes = starttimes;
        _durations = durations;
        _keys_iter = _keys.begin();
        _starttime_iter = _starttimes.begin();
        _duration_iter = _durations.begin();
        return true;
    }

    bool VGInBuf::set_buf(unsigned char* fuzz_buf, size_t fuzz_len)
    {
        std::string process="";
        unsigned char* ptr = fuzz_buf;
        try{
            process = "input count";
            size_t input_count = *ptr++;

            size_t check_size = sizeof(size_t) + input_count * (sizeof(unsigned char) + sizeof(double) * 2);
            if(fuzz_len != check_size){
                #ifndef VGIN
                std::cerr << "VGIn read buf error: buf size is invalid" << std::endl;
                #endif
                return false;
            }

            process = "input name";
            for(size_t i=0; i<input_count; i++){
                unsigned char input = *ptr++;
                _keys.push_back(input);
            }

            process = "input start time";
            for(size_t i=0; i<input_count; i++){
                double start_time = *(double*)ptr;
                fuzz_buf += sizeof(double);
                _starttimes.push_back(start_time);
            }

            process = "input duration";
            for(size_t i=0; i<input_count; i++){
                double duration = *(double*)ptr;
                fuzz_buf += sizeof(double);
                _durations.push_back(duration);
            }
        }
        catch (const std::bad_alloc& e) {
            #ifndef VGIN
            std::cerr << "VGIn read buf error: " << e.what() << " in " << process << std::endl;
            #endif
            return false;
        }

        return true;
    }

    std::vector<unsigned char> VGInBuf::get_processings()
    {
        if(_is_updated)
        {
            _processing_keys.clear();
            for(auto& k : _processing_keys_life)
            {
                if(k.second > 0)
                {
                    _processing_keys.push_back(k.first);
                }
            }
            _is_updated = false;
        }
        else
        {
            #ifndef VGIN
            std::cerr << "Warning: get_processings called without update" << std::endl;
            #endif
        }
        return _processing_keys;
    }   
}
}