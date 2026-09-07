#include "shatl/il2cpp/il2cpp.h"
#include "shatl/imgui/initialize.h"
#include "shatl/utils/config.h"
#include "shatl/utils/log.h"
#include <thread>
#include <unistd.h>

void hook_thread() noexcept {
    while (!tl::im::initialize()) {
        sleep(1);
    }

    while (!tl::il2cpp::init()) {
        sleep(1);
    }

    tl::func::config::init();

    LOGI("初始化成功");
}

// Terraria.GameContent.TextureAssets

__attribute__((constructor)) void lib_main() {
    std::thread thread(hook_thread);
    thread.detach();
}