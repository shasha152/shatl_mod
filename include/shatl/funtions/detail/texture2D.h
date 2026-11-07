#pragma once

#include "shatl/detail/counter.inc.h"
#include "shatl/il2cpp/il2cpp.h"
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>

namespace tl {
namespace func {

template <typename T> struct asset : il2cpp::object<> {
    il2cpp::string *name;
    int state;
    bool is_disposed;
    void *source;
    T value;
};

struct texture2D : public il2cpp::object<texture2D> {
    inline static il2cpp::_class *_main_class = nullptr;

    static void init() noexcept {
        _main_class =
            il2cpp::assembly::create("Assembly-CSharp.dll")
                ->mclass("Microsoft.Xna.Framework.Graphics", "Texture2D");
    }

    static texture2D *create(const std::string &source_asset) noexcept {
        static auto m =
            _main_class->mmethod(".ctor", {"UnityEngine.Texture2D"});

        static auto load =
            il2cpp::assembly::create("UnityEngine.ImageConversionModule.dll")
                ->mclass("UnityEngine", "ImageConversion")
                ->mmethod("LoadImage");
        static auto unity_class =
            il2cpp::assembly::create("UnityEngine.CoreModule.dll")
                ->mclass("UnityEngine", "Texture2D");
        static auto ctor =
            unity_class->mmethod(".ctor", {"System.Int32", "System.Int32"});
        auto unity_tex =
            unity_class
                ->object_new<il2cpp::object<il2cpp::tag<TL_AUTO_TAG_ID>> *>(
                    ctor, 2, 2);
        if (!load->invoke<bool>(unity_tex, read_data(source_asset), false))
            return nullptr;
        unity_class->parent()
            ->mmethod("set_filterMode")
            ->invoke<void>(unity_tex, 0);
        unity_class->parent()
            ->mmethod("set_wrapMode")
            ->invoke<void>(unity_tex, 1);

        auto obj = _main_class->object_new<texture2D *>(m, unity_tex);
        return obj;
    }

    static il2cpp::array<char> *
    read_data(const std::string &source_asset) noexcept {
        static auto byte_class =
            il2cpp::assembly::create("mscorlib.dll")->mclass("System", "Byte");

        std::string data;
        std::ifstream stream(source_asset);

        data.assign(std::istreambuf_iterator<char>(stream.rdbuf()),
                    std::istreambuf_iterator<char>());
        auto bytes = il2cpp::array<char>::create(byte_class, data.size());
        std::memcpy(bytes->begin(), data.data(), data.size());

        return bytes;
    }
};

struct texture_assets : il2cpp::object<texture_assets> {
    inline static il2cpp::_class *_main_class = nullptr;

    using value_type = asset<texture2D *> *;
    using array_type = il2cpp::array<value_type> *;

    static void init() noexcept {
        _main_class = il2cpp::assembly::create("Assembly-CSharp.dll")
                          ->mclass("Terraria.GameContent", "TextureAssets");
    }

    static array_type get_asset_array(std::string_view name) noexcept {
        return _main_class->mfield(name)->static_value<array_type>();
    }
};

} // namespace func
namespace tex {
void init() noexcept;
}
} // namespace tl