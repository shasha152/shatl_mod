#include "shatl/funtions/maininit.h"
#include "shatl/funtions/world/main.h"
#include "shatl/imgui/detail/gui.h"
// #include "shatl/utils/config.h"

#include <backends/imgui_impl_android.h>
#include <backends/imgui_impl_opengl3.h>
#include <chrono>
#include <imgui.h>

namespace tl {
namespace im {

namespace Gui = ImGui;

// void player_page_gui() noexcept {
//     Gui::Checkbox("血量", GET_PTR_CONFIG_VALUE_BOOL("player.lifefrezze"));
//     Gui::SameLine();
//     Gui::Checkbox("蓝量", GET_PTR_CONFIG_VALUE_BOOL("player.manafrezze"));
//     Gui::SameLine();
//     Gui::Checkbox("秒杀", GET_PTR_CONFIG_VALUE_BOOL("player.instantkill"));

//     if (Gui::Button("mainPlayer"))
//         func::register_once_function(func::log_player_addr);
// }

// void world_page_gui() noexcept {
//     if (Gui::Button("全图吸物"))
//         func::register_once_function(func::grab_all_world_item);

//     Gui::Checkbox(
//         "双击地图传送",
//         GET_PTR_CONFIG_VALUE_BOOL("world.double_click_minimap_to_teleport"));
//     Gui::Checkbox("全图高亮(迷幻)",
//                   GET_PTR_CONFIG_VALUE_BOOL("world.highlight_entire_map"));
// }

void about_page_gui() {
    Gui::Text("代码开源, 给个star吧!");
    Gui::Text("github仓库:sha152/shatl");
    Gui::Text("b站:@大鲨鲨qwq");
}

// void definite_time_save_config(int second) noexcept {
//     using clock_type = std::chrono::system_clock;
//     static clock_type::time_point now = clock_type::now();
//     static clock_type::time_point last = now;

//     now = clock_type::now();
//     if (std::chrono::duration_cast<std::chrono::seconds>(now - last).count() >=
//         second) {
//         utils::g_config.save();
//         last = now;
//     }
// }

void detail::draw_main_gui(int w, int h) noexcept {
    static bool is_init_func_main = false;

    // definite_time_save_config(1);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(w, h);
    Gui::NewFrame();

    Gui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);
    if (Gui::Begin("泰拉瑞亚")) {

        if (!is_init_func_main) {
            if (Gui::Button("初始化", {-1, -1})) {
                func::initialize();
                is_init_func_main = true;
            }
        }

        if (is_init_func_main) {
            Gui::BeginTabBar("tab_bar_1");
            if (Gui::BeginTabItem("player")) {
                // player_page_gui();
                Gui::EndTabItem();
            }
            if (Gui::BeginTabItem("世界")) {
                // world_page_gui();
                Gui::EndTabItem();
            }
            if (Gui::BeginTabItem("关于")) {
                about_page_gui();
                Gui::EndTabItem();
            }
            Gui::EndTabBar();
        }
    }

    Gui::End();
    Gui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(Gui::GetDrawData());
}

} // namespace im
} // namespace tl