#include "../include/vgin.hpp"

namespace vgfuzz{
namespace vgin{
    void VGInBuf::init(double *process_time)
    {
        if(process_time == nullptr)
        {
            std::cerr << "VGInBuf init error: process_time is null" << std::endl;
            return;
        }
        _life_cycle = vgin::VGIN_LIFE_CYCLE::STARTING;
        VGInBuf::clear_datas();
        _next_key_time_stamp = 0;
        _process_time = process_time;
        _prev_time = 0;
    }
    
    void VGInBuf::update(void)
    {
        if(!check_process_time()) return;
        if(_starttimes.empty() || _durations.empty() || _keys.empty())
        {
            std::cerr << "VGInBuf update error: empty buffer" << std::endl;
            return;
        }

        if(*_process_time >= _prev_time){
            bool all_released = true;
            for(auto& k : _processing_keys_life)
            {
                if(k.first == 0)
                {
                    continue;
                }
                if(k.second > 0)
                {
                    all_released = false;
                    k.second -= *_process_time - _prev_time;
                }
                else
                {
                    k.second = 0;
                }
            }            
            if(all_released && _life_cycle == vgin::VGIN_LIFE_CYCLE::ENDING)
            {
                _life_cycle = vgin::VGIN_LIFE_CYCLE::END;
                _processing_keys_life[0] = 1;
            }
            

            while(_duration_iter != _durations.end())
            {
                if(_next_key_time_stamp <= *_process_time)
                {
                    if(*_starttime_iter + *_duration_iter > *_process_time)
                    {
                        _processing_keys_life[*_keys_iter] = *_duration_iter - *_process_time + _next_key_time_stamp;
                    }
                    _starttime_iter++;
                    _duration_iter++;
                    _keys_iter++;
                    _next_key_time_stamp += *_starttime_iter;
                }
                else
                {
                    break;
                }
            }
            if(_duration_iter == _durations.end())
            {
                _life_cycle = vgin::VGIN_LIFE_CYCLE::ENDING;
            }       
        }

        _prev_time = *_process_time;
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
        _life_cycle = vgin::VGIN_LIFE_CYCLE::TESTING;
        return true;
    }

    bool VGInBuf::set_buf(unsigned char* fuzz_buf, size_t fuzz_len)
    {
        if(fuzz_buf == nullptr)
        {
            #ifndef VGIN
            std::cerr << "VGIn read buf error: buf is null" << std::endl;
            #endif
            return false;
        }

        std::string cur(reinterpret_cast<const char*>(fuzz_buf), fuzz_len);

        std::istringstream iss(cur);

        VGInBuf::clear_datas();

        unsigned char key;
        double starttime, duration;
        while(iss >> key >> starttime >> duration)
        {
            _keys.push_back(key);
            _starttimes.push_back(starttime);
            _durations.push_back(duration);
        }

        reset_iterators();
        _life_cycle = vgin::VGIN_LIFE_CYCLE::TESTING;

        return true;
    }

    // bool VGInBuf::set_buf(unsigned char* fuzz_buf, size_t fuzz_len)
    // {
    //     if(fuzz_buf == nullptr)
    //     {
    //         #ifndef VGIN
    //         std::cerr << "VGIn read buf error: buf is null" << std::endl;
    //         #endif
    //         return false;
    //     }

    //     std::string process="";
    //     unsigned char* ptr = fuzz_buf;

    //     process = "input count";
    //     size_t input_count = *ptr++;

    //     size_t check_size = sizeof(size_t) + input_count * (sizeof(unsigned char) + sizeof(double) * 2);
    //     if(fuzz_len <= 0 || fuzz_len != check_size){
    //         #ifndef VGIN
    //         std::cerr << "VGIn read buf error: buf size is invalid" << std::endl;
    //         #endif
    //         return false;
    //     }

    //     process = "input name";
    //     for(size_t i=0; i<input_count; i++){
    //         unsigned char input = *ptr++;
    //         _keys.push_back(input);
    //     }

    //     process = "input start time";
    //     for(size_t i=0; i<input_count; i++){
    //         double start_time = *(double*)ptr;
    //         fuzz_buf += sizeof(double);
    //         _starttimes.push_back(start_time);
    //     }

    //     process = "input duration";
    //     for(size_t i=0; i<input_count; i++){
    //         double duration = *(double*)ptr;
    //         fuzz_buf += sizeof(double);
    //         _durations.push_back(duration);
    //     }

    //     // try{

    //     // }
    //     // catch (const std:: e) {
    //     //     #ifndef VGIN
    //     //     std::cerr << "VGIn read buf error: " << e.what() << " in " << process << std::endl;
    //     //     #endif
    //     //     return false;
    //     // }

    //     return true;
    // }

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

extern "C"{
    const unsigned char * vgin_getchs(size_t* len)
    {
        std::vector<unsigned char> processings = vgfuzz::vgin::VGInBuf::get_processings();
        if(len != nullptr)
        {
            *len = processings.size();
        }
        else
        {
            #ifndef VGIN
            std::cerr << "Warning: len is null" << std::endl;
            #endif
        }
    
        return processings.data();
    }
}

}