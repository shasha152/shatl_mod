#pragma once

#include "shatl/il2cpp/il2cpp.h"
namespace tl {
namespace func {
struct localized_text : il2cpp::object<localized_text> {
    il2cpp::string *key;   // 0x10
    il2cpp::string *value; // 0x18
    il2cpp::string *english_value;
};
struct lang : il2cpp::object<lang> {
    inline static il2cpp::_class *kls = nullptr;
    static void init() noexcept {
        kls = il2cpp::_class::create(
            il2cpp::assembly::create("Assembly-CSharp.dll"), "Terraria",
            "Lang");
    }

    static il2cpp::array<localized_text *> *
    get_item_localized_texts() noexcept {
        return kls->mfield("_itemNameCache")
            ->static_value<il2cpp::array<localized_text *> *>();
    }
};
} // namespace func
} // namespace tl