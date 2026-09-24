#include "shatl/funtions/maininit.h"
#include "shatl/funtions/server.h"
#include "shatl/il2cpp/il2cpp.h"
#include "shatl/imgui/initialize.h"
#include "shatl/utils/log.h"
#include <memory>
#include <thread>
#include <unistd.h>

void hook_thread() noexcept {
    LOGI("服务器初始化");
    tl::func::server server(39520);
    sleep(2);
    while (!tl::il2cpp::init()) {
        sleep(1);
    }

    LOGI("imgui初始化");
    while (!tl::im::initialize()) {
        sleep(1);
    }

    tl::func::initialize();
    LOGI("服务器启动");
    server.run();
}

// Terraria.GameContent.TextureAssets

inline static std::thread *thread;

__attribute__((constructor)) void lib_main() {
    thread = new std::thread(hook_thread);
    thread->detach();
}