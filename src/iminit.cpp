#include "shatl/imgui/def.h"
#include "shatl/imgui/detail/gui.h"
#include "shatl/imgui/initialize.h"
#include "shatl/utils/log.h"

#include <android/api-level.h>
#include <android/input.h>
#include <dlfcn.h>

#include <EGL/egl.h>
#include <backends/imgui_impl_android.h>
#include <backends/imgui_impl_opengl3.h>
#include <dobby.h>
#include <imgui.h>

namespace tl {
namespace im {

static void *egl_handle = nullptr;
static void *egl_swapbuffer_symbol = nullptr;
static void *input_symbol = nullptr;

static bool is_initialized = false;

bool initialize_input() noexcept;
bool initialize_imgui(float w, float h) noexcept;
bool initialize_egl() noexcept;

install_hook_name(eglSwapBuffers, EGLBoolean, EGLDisplay dpy, EGLSurface surf) {
    EGLint w, h;
    eglQuerySurface(dpy, surf, EGL_WIDTH, &w);
    eglQuerySurface(dpy, surf, EGL_HEIGHT, &h);
    if (!is_initialized)
        is_initialized = initialize_imgui(w, h);

    if (is_initialized) [[likely]]
        detail::draw_main_gui(w, h);

    return orig_eglSwapBuffers(dpy, surf);
}

install_hook_name(initializeMotionEvent, void, AInputEvent *event, void *_p1) {
    orig_initializeMotionEvent(event, _p1);
    if (is_initialized)
        ImGui_ImplAndroid_HandleInputEvent(event);
}

install_hook_name(MotionEventCopy, bool, AInputEvent *self, void *other,
                  bool keepHistory) {
    bool res = orig_MotionEventCopy(self, other, keepHistory);
    if (is_initialized)
        ImGui_ImplAndroid_HandleInputEvent(self);
    return res;
}

bool initialize_input() noexcept {
    int apilv = android_get_device_api_level();

    if (apilv < 35) {
        input_symbol = DobbySymbolResolver(
            "libinput.so", "_ZN7android13InputConsumer21initializeMotionEventEP"
                           "NS_11MotionEventEPKNS_12InputMessageE");
        if (input_symbol)
            install_hook_initializeMotionEvent(input_symbol);

    } else {
        input_symbol = DobbySymbolResolver(
            "libinput.so", "_ZN7android11MotionEvent8copyFromEPKS0_b");
        if (input_symbol)
            install_hook_MotionEventCopy(input_symbol);
    }

    if (!input_symbol) {
        LOGE("Not Found Symbol");
        return false;
    }

    return true;
}

bool initialize_egl() noexcept {
    egl_handle = dlopen("libEGL.so", RTLD_NOW);
    if (egl_handle == nullptr) {
        LOGE("%s", dlerror());
        return false;
    }

    egl_swapbuffer_symbol = dlsym(egl_handle, "eglSwapBuffers");
    if (egl_swapbuffer_symbol == nullptr) {
        dlclose(egl_handle);
        LOGE("%s", dlerror());

        return false;
    } else
        install_hook_eglSwapBuffers(egl_swapbuffer_symbol);

    return true;
}

bool initialize_imgui(float w, float h) noexcept {
    auto ctx = ImGui::CreateContext();
    if (!ctx) {
        LOGI("Failed to create context");
        return false;
    }

    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(w, h);
    // io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.IniFilename = nullptr;
    // enable docking
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    initialScreenSize.x = w;
    initialScreenSize.y = h;

    // Setup Platform/Renderer backends
    ImGui_ImplAndroid_Init();
    ImGui_ImplOpenGL3_Init("#version 300 es");
    ImGui::StyleColorsLight();

    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(3);

    return true;
}

bool initialize() noexcept {
#if IS_ENABLE_DEBUG_GUI
    if (!initialize_input()) {
        destroy();
        return false;
    }
#endif

    if (!initialize_egl()) {
        destroy();
        return false;
    }

    return true;
}

void destroy() noexcept {
    if (egl_handle) {
        dlclose(egl_handle);
        DobbyDestroy(egl_swapbuffer_symbol);
    }
    DobbyDestroy(input_symbol);

    egl_swapbuffer_symbol = nullptr;
    input_symbol = nullptr;
}

} // namespace im
} // namespace tl