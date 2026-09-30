#include "shatl/funtions/maininit.h"
#include "shatl/funtions/service.h"
#include "shatl/il2cpp/il2cpp.h"
#include "shatl/imgui/initialize.h"
#include "shatl/utils/freeze.h"
#include "shatl/utils/log.h"
#include <thread>
#include <unistd.h>

void hook_thread() noexcept {
    sleep(2);
    while (!tl::il2cpp::init()) {
        sleep(1);
    }

    LOGI("imgui初始化");
    while (!tl::im::initialize()) {
        sleep(1);
    }

    tl::func::initialize();
    tl::utils::g_freeze_worker.run();
    LOGI("服务初始化");
    tl::func::service service(39520);
    LOGI("服务启动");
    service.run();
}

// Terraria.GameContent.TextureAssets

__attribute__((constructor)) void lib_main() {
    std::thread thread(hook_thread);
    thread.detach();
}