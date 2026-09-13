#include "shatl/imgui/detail/gui.h"

#include <backends/imgui_impl_android.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

namespace tl {
namespace im {

void detail::draw_main_gui(int w, int h) noexcept {
    // ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplAndroid_NewFrame(w, h);
    // ImGui::NewFrame();

    // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace im
} // namespace tl