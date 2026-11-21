#include "packet.pb.h"
#include "shatl/funtions/detail/class/world.h"
#include "shatl/funtions/maininit.h"
#include "shatl/funtions/server/webservice.h"
#include "shatl/il2cpp/il2cpp.h"
#include "shatl/imgui/initialize.h"
#include "shatl/utils/log.h"
#include <thread>
#include <unistd.h>

static tl::func::websocket_server service(39520);

void hook_thread() noexcept {
    service.set_interceptor([](const auto &, tl::pro::confirm &confim) {
        bool ready = tl::func::world::ready();
        if (!ready) {
            confim.set_ok(false);
            confim.set_message("世界未加载");
        }
        return !ready;
    });
    service.run();

    while (!tl::il2cpp::init()) {
        sleep(1);
    }

    LOGI("imgui初始化...");
    while (!tl::im::initialize()) {
        sleep(1);
    }
    LOGI("imgui初始化完成");

    LOGI("func初始化...");
    tl::func::initialize();
    LOGI("func初始化完成");
}

// Terraria.GameContent.TextureAssets

__attribute__((constructor)) void lib_main() {
    std::thread thread(hook_thread);
    thread.detach();
}