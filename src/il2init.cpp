#include "shatl/il2cpp/il2cpp.h"
#include <xdl.h>

bool tl::il2cpp::init() noexcept {
    handle = xdl_open("libil2cpp.so", XDL_DEFAULT);
    if (!handle) {
        LOGE("%s", dlerror());
        return false;
    }

    domain = il2cpp_invoke<void *>("il2cpp_domain_get");
    if (domain == nullptr) {
        LOGE("il2cpp_domain_get 返回空");
        return false;
    }

    il2cpp_invoke<void *>("il2cpp_thread_attach", domain);

    return true;
}

void *tl::il2cpp::get_handle_sym(std::string_view str) noexcept {
    return xdl_sym(handle, str.data(), nullptr);
}