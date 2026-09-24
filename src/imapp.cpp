#include "shatl/funtions/detail/player.h"
#include "shatl/funtions/detail/world.h"
#include "shatl/funtions/detail/lang.h"
#include "shatl/imgui/def.h"
#include "shatl/imgui/detail/gui.h"
#include "shatl/utils/log.h"

#include <backends/imgui_impl_android.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

namespace tl {
namespace im {

#if IS_ENABLE_DEBUG_GUI
void detail::draw_main_gui(int w, int h) noexcept {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(w, h);
    ImGui::NewFrame();

    if (ImGui::Begin("Debug")) {
        if (ImGui::Button("local player bag")) {
            auto bag = func::get_local_player_bag();
            if (bag) {
                LOGI("array_size:%zu", bag->size());
                for (auto item : *bag) {
                    LOGI("type:%d,number:%d", item->type, item->stack);
                }
            }
        }
        if (ImGui::Button("item name")) {
            auto arr = func::lang::get_item_localized_texts();
            for (auto txt : *arr) {
                LOGI("%s:%s", txt->key->to_string().c_str(),
                     txt->value->to_string().c_str());
            }
        }

        if (ImGui::Button("tiandingjian")) {
            auto bag = func::get_local_player_bag();
            (*bag)[0]->stack = 1;
            func::register_once_function(
                [bag]() { (*bag)[0]->change_item(4956); });
        }
    }
    ImGui::End();
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
#else
void detail::draw_main_gui(int w, int h) noexcept {}
#endif

} // namespace im
} // namespace tl