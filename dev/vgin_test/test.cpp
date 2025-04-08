#include <iostream>
#include <string>
#include <dlfcn.h>       // dlopen, dlsym, dlclose
#include <cstdlib>       // exit()
#include "../../include/vgin.hpp"

using namespace std;

// ファクトリ関数の型定義
typedef vgfuzz::vgin::VGInBuf* (*CreateVGInBuf_t)();
// 解放関数の型定義
typedef void (*DestroyVGInBuf_t)(vgfuzz::vgin::VGInBuf*);

int main(void)
{
    // 動的ライブラリのパス
    string lib_path = "../../libvgfuzz.so";

    // ライブラリをロード（RTLD_LAZYで遅延解決）
    void* handle = dlopen(lib_path.c_str(), RTLD_LAZY);
    if (!handle) {
        cerr << "ライブラリのロードに失敗しました: " << dlerror() << endl;
        return 1;
    }
    
    // dlerror()で既存のエラーをクリア
    dlerror();

    // ファクトリ関数 createVGInBuf を取得
    CreateVGInBuf_t create_vgin = (CreateVGInBuf_t)dlsym(handle, "createVGInBuf");
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        cerr << "シンボル 'createVGInBuf' の取得に失敗しました: " << dlsym_error << endl;
        dlclose(handle);
        return 1;
    }

    // 解放関数 destroyVGInBuf を取得
    DestroyVGInBuf_t destroy_vgin = (DestroyVGInBuf_t)dlsym(handle, "destroyVGInBuf");
    dlsym_error = dlerror();
    if (dlsym_error) {
        cerr << "シンボル 'destroyVGInBuf' の取得に失敗しました: " << dlsym_error << endl;
        dlclose(handle);
        return 1;
    }

    // VGInBufのインスタンス生成
    vgfuzz::vgin::VGInBuf* vgin = create_vgin();
    if (!vgin) {
        cerr << "VGInBufインスタンスの生成に失敗しました。" << endl;
        dlclose(handle);
        return 1;
    }
    
    // テスト用バッファの準備
    double time = 0;
    unsigned char keys[] = {'a', 'b', 'c'};
    double starttimes[] = {0.0, 1.0, 2.0};
    double durations[] = {0.5, 1.5, 2.5};
    unsigned char count = sizeof(keys) / sizeof(keys[0]);
    size_t len = count + sizeof(keys) + sizeof(starttimes) + sizeof(durations);
    unsigned char* fuzz_buf = new unsigned char[len];
    
    // バッファ先頭にキー数を格納
    fuzz_buf[0] = count;
    unsigned char* ptr = fuzz_buf + 1;
    for (size_t i = 0; i < count; i++) {
        *ptr++ = keys[i];
    }
    for (size_t i = 0; i < count; i++) {
        *(double*)ptr = starttimes[i];
        ptr += sizeof(double);
    }
    for (size_t i = 0; i < count; i++) {
        *(double*)ptr = durations[i];
        ptr += sizeof(double);
    }
    
    cout << "fuzz_buf: invalid" << endl;
    vgin->init(nullptr);
    vgin->set_buf(nullptr, 0);
    // vgin->update();  // （ここはコメントアウトしたまま）

    cout << "fuzz_buf: valid" << endl;
    vgin->init(&time);
    vgin->set_buf(fuzz_buf, len);
    vgin->update();
    time += 0.5;
    vgin->update();
    
    string answer = "";
    for (auto& k : vgin->get_processings())
    {
        answer += k;
    }
    cout << "Answer: " << answer << endl;
    
    // クリーンアップ
    destroy_vgin(vgin);
    dlclose(handle);
    delete[] fuzz_buf;
    
    return 0;
}
